/***************************************************************************
     Copyright          : (C) 2008 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "wtageditor.h"
#include "tagfilecache.h"

#include <id3/tag.h>

#include <tag.h>
#include <fileref.h>
#include <mpegfile.h>
#include <vorbisfile.h>
#include <flacfile.h>
#include <wavfile.h>
#include <id3v2tag.h>
#include <commentsframe.h>
#include <textidentificationframe.h>

#include <QDebug>
#include <QDesktopWidget>


constexpr char kChangedStyle[] = "background-color: #3350b4";
constexpr char kInvalidStyle[] = "background-color: #aa3333";

WTagEditor::WTagEditor(QString fname, QWidget *parent) : QWidget(parent) {
    setupUi(this);
    m_tfile = nullptr;
    setFilename(fname);
    
    adjustSize();
    //move(QApplication::desktop()->screen()->rect().center() - rect().center());
    createGenreList();
    
    const QPalette pal = palette();
    QColor baseColor = pal.color(QPalette::Base);
    QColor textColor = pal.color(QPalette::Text);
    qDebug() << "WTagEditor base color:" << baseColor << ", text color:" << textColor;
}

WTagEditor::WTagEditor(QWidget* parent) : QWidget(parent) {
    setupUi(this);
    
    createGenreList();
}

WTagEditor::~WTagEditor() {
    if (m_tfile) {
        TagFileCacheI.keepInCache(m_tfile, false);
        m_tfile = nullptr;
    }
}

void WTagEditor::setFilename(QString fname) {
    if (m_tfile) {
        TagFileCacheI.keepInCache(m_tfile, false);
        m_tfile = nullptr;
    }
    
    m_tfile = TagFileCacheI.getTagFile(fname);
    TagFileCacheI.keepInCache(m_tfile, true);
    refresh();
}

void WTagEditor::setFilenames(QStringList fnames) {
    
}

QString WTagEditor::filename() const {
    if(!m_tfile) return QString();
    return m_tfile->filename();
}

QStringList WTagEditor::filenames() const {
    
    return QStringList();
}

void WTagEditor::setTagFile(TagFile* tf) {
    if (m_tfile) {
        disconnect(m_tfile, SIGNAL(tagsUpdated()), this, SLOT(slotRefresh()));
        // auto update values if buttons are hidden
        if (!btnContainer->isVisible()) updateChanges();
    }
    m_tfile = tf;
    refresh();
    if (m_tfile) connect(m_tfile, SIGNAL(tagsUpdated()), this, SLOT(slotRefresh()));
}

void WTagEditor::setTagFiles(QList<TagFile*> tfl) {
    qDebug() << __PRETTY_FUNCTION__ << "not implemented";
    m_tflielst = tfl;
    //refresh();
}

void WTagEditor::refresh() {
    clearAll();
    if(!m_tfile) return;

    m_savedTagInfo = m_tfile->savedTagInfo();
    TagInfo ti = m_tfile->tagInfo();
    txtFilename->setText(ti.simplifiedFilename());
    txtArtist->setText(ti.artist);
    txtTitle->setText(ti.title);
    txtAlbum->setText(ti.album);
    if (ti.year > 0) txtYear->setText(QString::number(ti.year));
    if (ti.track > 0) txtTrack->setText(QString::number(ti.track));
    txtBPM->setText(ti.BPM);
    cbGenre->setEditText(ti.genre);
    txtComment->setPlainText(ti.comment);
}

void WTagEditor::showButtons(bool sh) {
    btnContainer->setVisible(sh);
}

void WTagEditor::clearAll() {
    txtFilename->setText("");
    txtArtist->setText("");
    txtTitle->setText("");
    txtAlbum->setText("");
    txtYear->setText("");
    txtTrack->setText("");
    txtBPM->setText("");
    cbGenre->setEditText("");
    txtComment->setPlainText("");
}

void WTagEditor::readTags() {
    if (!m_tfile) return;
    
    m_tfile->readTags();
    refresh();
}

void WTagEditor::updateChanges() {
    if(!m_tfile) return;
    
    qDebug() << __PRETTY_FUNCTION__ << m_tfile->simplifiedFilename();
    
    m_tfile->setArtist(txtArtist->text().trimmed());
    m_tfile->setTitle(txtTitle->text().trimmed());
    m_tfile->setAlbum(txtAlbum->text().trimmed());
    
    uint num = txtTrack->text().toUInt();
    m_tfile->setTrack(num);
    
    num = txtYear->text().toUInt();
    m_tfile->setYear(num);
    
    m_tfile->setGenre(cbGenre->currentText());
    m_tfile->setComment(txtComment->toPlainText());
    
    QString nfname = txtFilename->text();
    if (isFilenameValid(nfname)) m_tfile->setSimplifiedFilename(nfname);
    
    m_tfile->emitChanged();
}

bool WTagEditor::isChanged() const {
    if (txtFilename->text() != m_savedTagInfo.simplifiedFilename()) return true;
    if (txtArtist->text() != m_savedTagInfo.artist) return true;
    if (txtTitle->text() != m_savedTagInfo.title) return true;
    if (txtAlbum->text() != m_savedTagInfo.album) return true;
    if (txtComment->toPlainText() != m_savedTagInfo.comment) return true;
    if (cbGenre->currentText() != m_savedTagInfo.genre) return true;
    if (txtTrack->text().toUInt() != m_savedTagInfo.track) return true;
    if (txtYear->text().toUInt() != m_savedTagInfo.year) return true;
    
    return false;
}

void WTagEditor::slotOKClicked() {
    updateChanges();

    //if(m_tfile) { m_tfile->saveTags(); }
    close();
}

void WTagEditor::slotCancelClicked() {
    m_tfile = nullptr;
    clearAll();
    close();
}

void WTagEditor::slotArtistChanged(QString) {
    if(!m_tfile) return;
    
    if(txtArtist->text() != m_savedTagInfo.artist) {
        txtArtist->setStyleSheet(kChangedStyle);
    } else {
        txtArtist->setStyleSheet("");
    }
}

void WTagEditor::slotAlbumChanged(QString) {
    if(!m_tfile) return;
    
    if(txtAlbum->text() != m_savedTagInfo.album) {
        txtAlbum->setStyleSheet(kChangedStyle);
    } else {
        txtAlbum->setStyleSheet("");
    }
}

void WTagEditor::slotTitleChanged(QString) {
    if(!m_tfile) return;
    
    if(txtTitle->text() != m_savedTagInfo.title) {
        txtTitle->setStyleSheet(kChangedStyle);
    } else {
        txtTitle->setStyleSheet("");
    }
}

void WTagEditor::slotGenreChanged(QString) {
    if(!m_tfile) return;
    
    if(cbGenre->currentText() != m_savedTagInfo.genre) {
        cbGenre->setStyleSheet(kChangedStyle);
    } else {
        cbGenre->setStyleSheet("");
    }
}

void WTagEditor::slotYearChanged(QString) {
    if(!m_tfile) return;
    
    QString tstr;
    if (m_savedTagInfo.year > 0) tstr = QString::number(m_savedTagInfo.year);
    if(txtYear->text() != tstr) {
        txtYear->setStyleSheet(kChangedStyle);
    } else {
        txtYear->setStyleSheet("");
    }
}

void WTagEditor::slotTrackChanged(QString) {
    if(!m_tfile) return;
    
    QString tstr;
    if (m_savedTagInfo.track > 0) tstr = QString::number(m_savedTagInfo.track);
    if(txtTrack->text() != tstr) {
        txtTrack->setStyleSheet(kChangedStyle);
    } else {
        txtTrack->setStyleSheet("");
    }
}

void WTagEditor::slotBPMChanged(QString text) {
    
}

void WTagEditor::slotCommentChanged(QString) {
    if(!m_tfile) return;
    
    if(txtComment->toPlainText() != m_savedTagInfo.comment) {
        txtComment->setStyleSheet(kChangedStyle);
    } else {
        txtComment->setStyleSheet("");
    }
}

void WTagEditor::slotFilenameChanged(QString text) {
    // TODO: red background if filename is invalid or empty and reset to current filename
    if (! isFilenameValid(text)) {
        txtFilename->setStyleSheet(kChangedStyle);
        return;
    }
    
    if (text != m_tfile->simplifiedFilename()) {
        txtFilename->setStyleSheet(kChangedStyle);
    } else {
        txtFilename->setStyleSheet("");
    }
}

void WTagEditor::slotFilenameSetFocus() {
    txtFilename->setFocus();
}

void WTagEditor::slotResetFilename() {
    if (!m_tfile) {
        txtFilename->setText("");
        return;
    }
    
    m_tfile->resetFilename();
    txtFilename->setText(m_tfile->simplifiedNewFilename());
}

void WTagEditor::slotRefresh() {
    refresh();
}
       
void WTagEditor::createGenreList() {
    cbGenre->clear();
    for (int i = 0; i < ID3_NR_OF_V1_GENRES; ++i) {
        QString genre = ID3_v1_genre_description[i];
        cbGenre->addItem(genre);
    }
}

bool WTagEditor::isFilenameValid(const QString& fname) {
    if (fname.isEmpty()) return false;
    
    // TODO: check for invalid characters in filename  (':', '/', '\\', * ? \n \r ...)
    
    return true;
}
