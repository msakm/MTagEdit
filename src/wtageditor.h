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
#ifndef WTAGEDITOR_H
#define WTAGEDITOR_H

#include "ui_wtageditor.h"

#include "tagfile.h"

#include <id3/globals.h>

#include <QFileInfo>
#include <QWidget>

class WTagEditor : public QWidget, Ui_WTagEditor {
    Q_OBJECT
public:
    explicit WTagEditor(QString filename = "", QWidget *parent = nullptr);
    explicit WTagEditor(QWidget *parent);
    ~WTagEditor();
    
    void setFilename(QString fname);
    void setFilenames(QStringList fnames);
    QString filename() const;
    QStringList filenames() const;
    void setTagFile(TagFile* tf);
    void setTagFiles(QList<TagFile*> tfl);
    void refresh();
    
    void showButtons(bool sh);
    void clearAll();
    void updateChanges();
    bool isChanged() const;

public slots:
    void slotFilenameSetFocus();
    
protected:
    void readTags();

protected slots:
    void slotOKClicked();
    void slotCancelClicked();
    
    void slotArtistChanged(QString text);
    void slotAlbumChanged(QString text);
    void slotTitleChanged(QString text);
    void slotGenreChanged(QString text);
    void slotYearChanged(QString text);
    void slotTrackChanged(QString text);
    void slotBPMChanged(QString text);
    void slotCommentChanged(QString text);
    void slotFilenameChanged(QString text);
    void slotResetFilename();
    void slotRefresh();
    
private:
    bool        m_multifile_mode = false;
    TagInfo     m_savedTagInfo;
    TagFile*    m_tfile = nullptr;  // single file mode
    QList<TagFile*> m_tflielst;     // multiple files mode
    
    void createGenreList();
    bool isFilenameValid(const QString& fname);
};

#endif
