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
#include "mainwidget.h"
#include "stringfilter.h"
#include "wtageditor.h"
#include "settings.h"
#include "taggenerator.h"
#include "tagfilecache.h"
#include "tagwriterthread.h"

#include <QCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QStringList>
#include <QRegExp>
#include <QFileIconProvider>

#include <QLineEdit>
#include <QMenu>

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
    setupUi(this);
    m_renamepattern = "{Artist} - {Title}";
    
    // create menus
    createMenus();
    
    // TagFileID3 infotrack;   infotrack.showKnownFrameInfo();  // list id3v2 frames
    
    QList<int> sizes; sizes << 180 << 800;
    splitter->setSizes(sizes);
    wTagEdit->showButtons(false);
    
    connect(&m_browser, SIGNAL(pathChanged(QString)), this, SLOT(slotPathChanged(QString)));
    //connect(txtFilename, SIGNAL(textEdited(QString)), this, SLOT(slotFilenameChanged()));
    
    m_browser.setPath(settings.recentDir());
    m_browserModel.setShowFiles(false);
    m_browserModel.setFileBrowser(&m_browser);
    twDirs->setModel(&m_browserModel);
    twDirs->setColumnWidth(0, 400);
    
    m_fileListModel.setFileBrowser(&m_browser);
    twFiles->setModel(&m_fileListModel);

    QItemSelectionModel* selectionModel = twFiles->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWidget::slotSelectionChanged);
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &MainWidget::slotCurrentFileChanged);
    
    // set file list column widths
    auto clist = m_fileListModel.columnList();
    for(int i = 0; i < clist.size(); ++i) {
        int c = clist.at(i);
        twFiles->setColumnWidth(i, m_fileListModel.columnWidth(c));
    }
    
    splitter->setHandleWidth(5);
    QList<int> splitSizes = {0, 1000};
    splitter->setSizes(splitSizes);
    
    connect(&m_guiTimer, &QTimer::timeout, this, &MainWidget::slotUpdateGui);
    m_guiTimer.start(1000);
}

MainWidget::~MainWidget() {
    if(m_tagEditor) delete m_tagEditor;
    deleteMenus();
}

QStringList MainWidget::renamePatternList() {
    QStringList lst;
    // TODO
    lst += "{Title}";
    lst += "{01} - {Title}";
    lst += "{01}. {Title}";
    lst += "{Artist} - {Title}";
    lst += "{01} - {Artist} - {Title}";
    lst += "{01}. {Artist} - {Title}";
    lst += "{01} - {Artist} - {Album} - {Title}";
    lst += "{01}. {Artist} - {Album} - {Title}";
    
    return lst;
}


void MainWidget::setRenamePattern(QString rpattern) {
    // TODO: check pattern (should contain at least one field)
    qDebug() << __PRETTY_FUNCTION__ << "rename pattern changed to" << rpattern;
    m_renamepattern = rpattern;
}

QString MainWidget::renamePattern() const {
    return m_renamepattern;
}

void MainWidget::slotBackPressed() {
    wTagEdit->updateChanges();
    m_browser.cdUp();
}

void MainWidget::slotFilenameReturnPressed() {
    if (!m_currentFile) return;
    
    QString filename = txtFilename->text();
    if (filename.isEmpty()) return;
    
    m_currentFile->setSimplifiedFilename(filename);
}

void MainWidget::slotFileMenuPopup() {
    m_fileMenu->exec(QCursor::pos());
}

void MainWidget::slotCopyMenuPopup() {
    m_copyMenu->exec(QCursor::pos());
}

void MainWidget::slotSwapMenuPopup() {
    m_swapMenu->exec(QCursor::pos());
}

void MainWidget::slotClearMenuPopup() {
    m_clearMenu->exec(QCursor::pos());
}

void MainWidget::slotCapitalizeMenuPopup() {
    m_capitalizeMenu->exec(QCursor::pos());
}

void MainWidget::slotResetMenuPopup() {
    m_resetMenu->exec(QCursor::pos());
}

void MainWidget::slotPathChanged(QString path) {
    //QString path = m_browser.absolutePath();
    settings.setRecentDir(path);
    if(!path.endsWith("/")) path += "/";
    txtPath->setText(path);
}

void MainWidget::slotGenerateTags() {
    TagGenerator tgen;
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tgen.setTagFile(tf);
        tgen.generateTags();
        tf->emitChanged();
    }
}

void MainWidget::slotEditTags() {
    wTagEdit->updateChanges();
    if(!m_tagEditor) m_tagEditor = new WTagEditor();
    
    m_tagEditor->setTagFiles(m_selectedFiles);
    m_tagEditor->show();
}

void MainWidget::slotUpdateTags() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->updateTags();
        tf->emitChanged();
    }
}

void MainWidget::slotRenameByPattern() {
    wTagEdit->updateChanges();
    QString pattern = "{00}. {Artist} - {Title}";
    // check if rename pattern contains at least one field (i. e. {Title})
    if(!m_renamepattern.isEmpty() && m_renamepattern.contains("{Title}")) {
        pattern = m_renamepattern;
    }
    
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->renameByPattern(pattern);
        tf->emitChanged();
    }
}

void MainWidget::slotCopyDArtist() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setDirAsArtist();
        tf->emitChanged();
    }
}

void MainWidget::slotCopyDArtistAlbum() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setDirAsArtistAlbum();
        tf->emitChanged();
    }
}

void MainWidget::slotCopyDAlbum() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setDirAsAlbum();
        tf->emitChanged();
    }
}

void MainWidget::slotExtractTrack() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        StringFilter sf;
        sf= tf->simplifiedNewFilename();
        QStringList nlist = sf.getNumbers();
        if (nlist.size()) {
            QString snum = nlist.first();
            tf->setTrack(snum.toUInt());
        }
        
        // QString str = tf->extractFirstFilenameItem();
        // qDebug() << "extracting to track ->" << str;
        // tf->setTrack(str.toUInt());
        tf->emitChanged();
    }
}

void MainWidget::slotExtractTitle() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        QString str = tf->extractFirstFilenameItem().trimmed();
        qDebug() << "extracting to title ->" << str;
        tf->setTitle(str);
        tf->emitChanged();
    }
}

void MainWidget::slotExtractArtist() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        QString str = tf->extractFirstFilenameItem().trimmed();
        qDebug() << "extracting to artist ->" << str;
        tf->setArtist(str);
        tf->emitChanged();
    }
}

void MainWidget::slotExtractAlbum() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        QString str = tf->extractFirstFilenameItem().trimmed();
        qDebug() << "extracting to album ->" << str;
        tf->setAlbum(str);
        tf->emitChanged();
    }
}

void MainWidget::slotExtractYear() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        QString str = tf->extractFirstFilenameItem();
        qDebug() << "extracting to year ->" << str;
        tf->setYear(str.toUInt());
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsArtist() {
    if (m_selectedtext.isEmpty()) return;

    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setArtist(m_selectedtext);
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsTitle() {
    if (m_selectedtext.isEmpty()) return;

    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setTitle(m_selectedtext);
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsAlbum() {
    if (m_selectedtext.isEmpty()) return;
    
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setAlbum(m_selectedtext);
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsTrack() {
    if (m_selectedtext.isEmpty()) return;
    
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setTrack(m_selectedtext.toUInt());
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsYear() {
    if (m_selectedtext.isEmpty()) return;
    
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setYear(m_selectedtext.toUInt());
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsComment() {
    if (m_selectedtext.isEmpty()) return;
    
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setComment(m_selectedtext);
        tf->emitChanged();
    }
}

void MainWidget::slotSetAsGenre() {
    if (m_selectedtext.isEmpty()) return;
    
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setGenre(m_selectedtext);
        tf->emitChanged();
    }
}

void MainWidget::slotSwapArtistTitle() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->swapArtistTitle();
        tf->emitChanged();
    }
}

void MainWidget::slotSwapAlbumTitle() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->swapAlbumTitle();
        tf->emitChanged();
    }
}

void MainWidget::slotSwapAlbumArtist() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->swapAlbumArtist();
        tf->emitChanged();
    }
}

void MainWidget::slotResetFilename() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->resetFilename();
        tf->emitChanged();
    }
}

void MainWidget::slotRestetAllToSaved() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->resetToSaved();
        tf->emitChanged();
    }
}

void MainWidget::slotResetAllToOriginal() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->resetToOriginal();
        tf->emitChanged();
    }
}

void MainWidget::slotResetArtist() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setArtist(tf->savedTagInfo().artist);
        tf->emitChanged();
    }
}

void MainWidget::slotResetTitle() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setTitle(tf->savedTagInfo().title);
        tf->emitChanged();
    }
}

void MainWidget::slotResetAlbum() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setAlbum(tf->savedTagInfo().album);
        tf->emitChanged();
    }
}

void MainWidget::slotClearAlbum() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setAlbum("");
        tf->emitChanged();
    }
}

void MainWidget::slotClearArtist() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setArtist("");
        tf->emitChanged();
    }
}

void MainWidget::slotClearTitle() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setTitle("");
        tf->emitChanged();
    }
}

void MainWidget::slotClearComment() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setComment("", "");
        tf->emitChanged();
    }
}

void MainWidget::slotClearTrack() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setTrack(0);
        tf->emitChanged();
    }
}

void MainWidget::slotClearGenre() {
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->setGenre("");
        tf->emitChanged();
    }
}

void MainWidget::slotCapitalizeFilename() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->capitalizeFilename();
        tf->emitChanged();
    }
}

void MainWidget::slotCapitalizeTags() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->capitalizeTags();
        tf->emitChanged();
    }
}

void MainWidget::slotCapitalizeArtist() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->capitalizeArtist();
        tf->emitChanged();
    }
}

void MainWidget::slotCapitalizeTitle() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->capitalizeTitle();
        tf->emitChanged();
    }
}

void MainWidget::slotCapitalizeAlbum() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->capitalizeAlbum();
        tf->emitChanged();
    }
}

void MainWidget::slotConvertToUtf8() {
    wTagEdit->updateChanges();
    foreach (TagFile* tf, m_selectedFiles) {
        if (!tf) continue;
        tf->convertToUtf8();
        tf->emitChanged();
    }
}

void MainWidget::slotFnameSelectionChanged() {
    QString text = txtOriginalFilename->text();
    QString stext = txtOriginalFilename->selectedText();
    
    if(stext == m_autoselectiontext) return;
    int sstart = txtOriginalFilename->selectionStart();
    int slength = stext.length();
    int send = sstart + slength - 1;
    
    if(sstart < 0) return;
    
    QChar c = text[sstart];
    // find new selection start
    if(c.isLetterOrNumber()) {
        // search to the left for selection start
        for(; sstart >= 0; --sstart) {
            c = text[sstart];
            if(c.isLetterOrNumber()) continue;
            if((c == '(') && stext.contains(')')) continue;
            
            break;
        }
    } else {
        // search to the right
        for(; sstart < text.length(); ++sstart) {
            c = text[sstart];
            if(c.isLetterOrNumber()) continue;
            if((c == '(') && stext.contains(")")) break;
            
            break;
        }
    }
    sstart++;
    
    // search for selection end
    c = text[send];
    //qDebug() << "Selection end char, index:" << c << send;
    
    if(c.isLetterOrNumber()) {
        // search to the right
        for(; send < text.length(); ++send) {
            c = text[send];
            if(c.isLetterOrNumber()) continue;
            if((c == ')') && stext.contains("(")) continue;
            
            break;
        }
    } else {
        // search to the left
        for(; send > 0; --send) {
            c = text[send];
            if(c.isLetterOrNumber()) continue;
            if((c == ')') && stext.contains("(")) break;

            break;
        }
    }
    if(sstart > send) sstart = send;
    slength = send - sstart;
    
    m_autoselectiontext = text.mid(sstart, slength);
    txtOriginalFilename->setSelection(sstart, slength);
    //qDebug() << "Final selected text:" << m_autoselectiontext;
}

void MainWidget::slotSeparatorChange(QTreeWidgetItem* item) {
    if(!item || !m_currentFile) return;
    
    QStringList isepl = m_currentFile->itemSeparators();
    QStringList wsepl = m_currentFile->wordSeparators();
    if(item->text(2).toUpper() == "ITEM") {
        QString sep = item->text(0);
        isepl.removeAll(sep);
        wsepl += sep;
        item->setText(2, "WORD");
    } else {
        QString sep = item->text(0);
        wsepl.removeAll(sep);
        isepl += sep;
        item->setText(2, "ITEM");
    }
    
    m_currentFile->setItemSeparators(isepl);
    m_currentFile->setWordSeparators(wsepl);
}

void MainWidget::slotItemMenuPopup(const QPoint) {
    QListWidgetItem* item = lstItems->currentItem();
    if(!item || item->text().startsWith("-----")) return;
    
    m_selectedtext = item->text();
    if(!m_selectedtext.isEmpty()) {
        m_filenameMenu->popup(QCursor::pos());
    }
}

void MainWidget::slotOriginalFnameMenuPopup(const QPoint) {
    qDebug() << "TODO: Popup filename menu  (set as)";
    
    m_selectedtext = txtOriginalFilename->selectedText();
    if(!m_selectedtext.isEmpty()) {
        m_filenameMenu->popup(QCursor::pos());
    }
}

void MainWidget::slotTmpTextMenuPopup() {
    m_selectedtext = tmpText->selectedText();
    if (m_selectedtext.isEmpty()) m_selectedtext = tmpText->text();
    
    if(!m_selectedtext.isEmpty()) {
        m_filenameMenu->popup(QCursor::pos());
    }
}

void MainWidget::slotDirDoubleClicked(const QModelIndex& idx) {
    QFileInfo fi = m_browserModel.fileInfo(idx);
    if (fi.isDir()) {
        m_browser.setCurrentDir(fi.fileName());
    }
}

void MainWidget::slotFileDoubleClicked(const QModelIndex& idx) {
    QFileInfo fi = m_fileListModel.getFileInfo(idx);
    if (fi.isDir()) {
        m_browser.setCurrentDir(fi.fileName());
    }
    
    if (fi.isFile()) {
        if(!m_tagEditor) m_tagEditor = new WTagEditor();
        
        m_currentFile = TagFileCacheI.getTagFile(fi);
        m_tagEditor->setFilename(fi.absoluteFilePath());
        m_tagEditor->show();
    }
}

void MainWidget::slotFileClicked(const QModelIndex& idx) {
    QFileInfo fi = m_fileListModel.getFileInfo(idx);

    if (fi.isFile()) {
        TagFile* tf = TagFileCacheI.getTagFile(fi);
        if (m_currentFile == tf) return;
        
        qDebug() << "FileClicked:" << idx.row() << fi.absoluteFilePath();
        m_currentFile = tf;
        if(!tf) return;
        
        //wTagEdit->updateChanges(); // called by setTagFile
        wTagEdit->setTagFile(tf);
        
        txtFilename->setText(tf->simplifiedNewFilename());
        txtOriginalFilename->setText(tf->simplifiedOriginalFilename());
        txtNewFilename->setText(tf->simplifiedFilename());
        
        TagGenerator gen;
        gen.setTagFile(tf);
        gen.test();
        
        // TODO: remove - parsing separators, item list, word list ...
        parseFilename(tf->simplifiedNewFilename(), tf);
    } else {
        m_currentFile = nullptr;
        wTagEdit->setTagFile(m_currentFile);
        txtFilename->setText("");
        txtOriginalFilename->setText("");
        txtNewFilename->setText("");
        lstItems->clear();
        treeSeparators->clear();
    }
}

void MainWidget::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
    (void) deselected;
    (void) selected;
    // clear the list and recreate it instead of adding selected and removing deselected
    m_selectedFiles.clear();
    
    foreach (const QModelIndex idx, twFiles->selectionModel()->selectedRows()) {
        if (idx.column() == 0) {
            TagFile* tf = m_fileListModel.getTagFile(idx);
            m_selectedFiles += tf;
        }
    }
    
    wTagEdit->setTagFiles(m_selectedFiles);
}

void MainWidget::slotCurrentFileChanged(const QModelIndex& current, const QModelIndex& previous) {
    (void) previous;
    slotFileClicked(current);
}

void MainWidget::slotSaveAll() {
    wTagEdit->updateChanges();
    QList<TagFile*> tfl = TagFileCacheI.changedFiles();
    qDebug() << "MainWidget::slotSaveAll: files to save:" << tfl.size();
    
    //TODO: show progress if more than one files are in the list
    for(int i = 0; i < tfl.size(); ++i) {
        TagFile* tf = tfl.at(i);
        if(!tf) continue;

        TagWriterThreadInst.enqueue(tf);
    }
}

void MainWidget::slotUpdateGui() {
    lblCachedFiles->setText(QString::number(TagFileCacheI.numCachedFiles()));
    lblChangedFiles->setText(QString::number(TagFileCacheI.numChangedFiles()));
    lblReading->setText(QString::number(TagFileCacheI.numReading()));
    lblWriting->setText(QString::number(TagFileCacheI.numWriting()));
}

void MainWidget::createMenus() {
    deleteMenus();
    
    m_filenameMenu = new QMenu("Filename");
    m_fileMenu = new QMenu();
    m_extractMenu = new QMenu("Extract");
    m_copyMenu = new QMenu("Copy");
    m_swapMenu = new QMenu("Swap");
    m_clearMenu = new QMenu("Clear");
    m_resetMenu = new QMenu("Reset");
    m_capitalizeMenu = new QMenu("Capitalize");
    
    // copy menu
    m_copyMenu->addAction("Directory --> Artist", this, SLOT(slotCopyDArtist()));
    m_copyMenu->addAction("Directory --> Artist - Album", this, SLOT(slotCopyDArtistAlbum()));
    m_copyMenu->addAction("Directory --> Album", this, SLOT(slotCopyDAlbum()));
    
    m_clearMenu->addAction("Album", this, SLOT(slotClearAlbum()));
    m_clearMenu->addAction("Artist", this, SLOT(slotClearArtist()));
    m_clearMenu->addAction("Title", this, SLOT(slotClearTitle()));
    m_clearMenu->addAction("Comment", this, SLOT(slotClearComment()));
    m_clearMenu->addAction("Track", this, SLOT(slotClearTrack()));
    m_clearMenu->addAction("Genre", this, SLOT(slotClearGenre()));
    m_clearMenu->addSeparator();
    // TODO: remove from clear menu, moved to reset menu
    m_clearMenu->addAction("Reset to saved tags", this, SLOT(slotResetToSaved()));
    m_clearMenu->addAction("Reset filename", this, SLOT(slotResetFilename()));
    
    m_swapMenu->addAction("Artist <-> Title", this, SLOT(slotSwapArtistTitle()));
    m_swapMenu->addAction("Artist <-> Album", this, SLOT(slotSwapArtistAlbum()));
    m_swapMenu->addAction("Album  <-> Title", this, SLOT(slotSwapAlbumTitle()));
    
    m_extractMenu->addAction("Track", this, SLOT(slotExtractTrack()));
    m_extractMenu->addAction("Artist", this, SLOT(slotExtractArtist()));
    m_extractMenu->addAction("Title", this, SLOT(slotExtractTitle()));
    m_extractMenu->addAction("Album", this, SLOT(slotExtractAlbum()));
    m_extractMenu->addAction("Year", this, SLOT(slotExtractYear()));
    
    m_resetMenu->addAction("Reset all to saved", this, SLOT(slotResetAllToSaved()));
    m_resetMenu->addAction("Reset all to original", this, SLOT(slotResetAllToOriginal()));
    m_resetMenu->addSeparator();
    m_resetMenu->addAction("Reset filename", this, SLOT(slotResetFilename()));
    m_resetMenu->addSeparator();
    m_resetMenu->addAction("Reset artist", this, SLOT(slotResetArtist()));
    m_resetMenu->addAction("Reset title", this, SLOT(slotResetTitle()));
    m_resetMenu->addAction("Reset album", this, SLOT(slotResetAlbum()));
    
    m_capitalizeMenu->addAction("Capitalize filename", this, SLOT(slotCapitalizeFilename()));
    m_capitalizeMenu->addAction("Capitalize tags", this, SLOT(slotCapitalizeTags()));
    m_capitalizeMenu->addAction("Capitalize Artist", this, SLOT(slotCapitalizeArtist()));
    m_capitalizeMenu->addAction("Capitalize Title", this, SLOT(slotCapitalizeTitle()));
    m_capitalizeMenu->addAction("Capitalize Album", this, SLOT(slotCapitalizeAlbum()));
    
    m_fileMenu->addAction("Generate tags", this, SLOT(slotGenerateTags()));
    m_fileMenu->addAction("Edit tags", this, SLOT(slotEditTags()));
    m_fileMenu->addAction("Update tags (v1 -> v2)", this, SLOT(slotUpdateTags()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Rename", this , SLOT(slotRenameByPattern()));
    m_fileMenu->addMenu(m_copyMenu);
    m_fileMenu->addMenu(m_extractMenu);
    m_fileMenu->addMenu(m_clearMenu);
    m_fileMenu->addMenu(m_swapMenu);
    m_fileMenu->addMenu(m_resetMenu);
    m_fileMenu->addSeparator();
    m_fileMenu->addMenu(m_capitalizeMenu);
    m_fileMenu->addAction("Convert tags to UTF8", this, SLOT(slotConvertToUtf8()));
    m_fileMenu->addAction("Capitalize filename", this, SLOT(slotCapitalizeFilename()));
    m_fileMenu->addAction("Capitalize tags", this, SLOT(slotCapitalizeTags()));
    
    
    m_filenameMenu->addAction("Set as Artist", this, SLOT(slotSetAsArtist()));
    m_filenameMenu->addAction("Set as Title", this, SLOT(slotSetAsTitle()));
    m_filenameMenu->addAction("Set as Album", this, SLOT(slotSetAsAlbum()));
    m_filenameMenu->addAction("Set as Year", this, SLOT(slotSetAsYear()));
    m_filenameMenu->addAction("Set as Track", this, SLOT(slotSetAsTrack()));
    m_filenameMenu->addAction("Set as Comment", this, SLOT(slotSetAsComment()));
    m_filenameMenu->addAction("Set as Genre", this, SLOT(slotSetAsGenre()));
}

void MainWidget::deleteMenus() {
    delete m_filenameMenu;  m_filenameMenu = nullptr;
    delete m_fileMenu;      m_fileMenu = nullptr;
    delete m_copyMenu;      m_copyMenu = nullptr;
    delete m_extractMenu;   m_extractMenu = nullptr;
    delete m_swapMenu;      m_swapMenu = nullptr;
    delete m_clearMenu;     m_clearMenu = nullptr;
    delete m_resetMenu;     m_resetMenu = nullptr;
    delete m_capitalizeMenu; m_capitalizeMenu = nullptr;
}

void MainWidget::slotFilenameChanged() {
    QString nfname = txtFilename->text();
    if (nfname.isEmpty() || !m_currentFile) return;
    
    m_currentFile->setSimplifiedFilename(nfname);
}

void MainWidget::parseFilename(QString fname, const TagFile* tf) {
    QStringList lst_separators;
    QStringList lst_words;
    QStringList lst_nums;
    QStringList lst_bracketComs;
    StringFilter stringf;
    stringf = fname;
    
    //if(tf) stringf = tf->simplifiedOriginalFilename();
    if(tf) stringf = tf->simplifiedNewFilename();
    
    stringf.createLists();
    
    lst_nums = stringf.getNumbers();
    lst_separators = stringf.getSeparators();
    lst_words = stringf.getWords();
    lst_bracketComs = stringf.getBracketStrings();
    
    qDebug() << "=====================";
    qDebug() << "Parsed filename:" << fname;
    qDebug() << " Separators:" << lst_separators;
    qDebug() << " Words:     " << lst_words;
    qDebug() << " Numbers:   " << lst_nums;
    qDebug() << " Brackets:  " << lst_bracketComs;
    qDebug() << "=====================";
    
    lstItems->clear();

    // add items to the list if isep is not empty
    QString ext = stringf.removeExtension();
    QString strid = stringf.removeID();
    QString num = stringf.removeTrackNumber();
    QStringList lst = stringf.getItems();
    if (!num.isEmpty()) lst.prepend(num);
    if (!strid.isEmpty()) lst += strid;
    //if (!ext.isEmpty()) lst += ext;
    lstItems->addItems(lst);
    
    if (lst_nums.size() > 0) {
        lstItems->addItem("--------");  // FIXME: remove
        lstItems->addItems(lst_nums);   // add numbers to list
    }
    
    if (lst_bracketComs.size() > 0) {
        lstItems->addItem("--------");  // FIXME: remove
        lstItems->addItems(lst_bracketComs);   // brackets
    }
    
    QStringList lst_sep1 = lst_separators;
    lst_sep1.removeDuplicates();
    
    treeSeparators->clear();
    QStringList itemsepl = stringf.getItemSeparators();
    
    for(int i = 0; i < lst_sep1.size(); ++i) {
        QString sep = lst_sep1.at(i);
        int cnt = lst_separators.count(sep);
        QTreeWidgetItem* item = new QTreeWidgetItem;
        item->setText(0, "\"" + sep + "\"");
        item->setText(1, QString::number(cnt));
        if(itemsepl.contains(sep)) {
            item->setText(2, "ITEM");
        } else {
            item->setText(2, "WORD");
        }
        treeSeparators->addTopLevelItem(item);
    }
}

void MainWidget::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
    case Qt::Key_Backspace: 
        wTagEdit->updateChanges();
        m_browser.cdUp();
        break;
    case Qt::Key_F1:
        slotGenerateTags();
        break;
    case Qt::Key_F2:
        wTagEdit->slotFilenameSetFocus();
        break;
    case Qt::Key_F3:
        slotRenameByPattern();
        break;
    case Qt::Key_F4:
        
        break;
    }
    
}
