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
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "filebrowser.h"
#include "filebrowsermodel.h"
#include "filelistmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QWidget>
#include <QTimer>
#include <QTreeWidgetItem>

#include "ui_mainwidget.h"

class WTagEditor;

class MainWidget : public QWidget, public Ui_MainWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    QStringList renamePatternList();
    void setRenamePattern(QString rpattern);
    QString renamePattern() const;
    
public slots:
    void slotRenameByPattern();
    
protected slots:
    void slotBackPressed();
    //void slotFileDoubleClicked(QString filename);
    //void slotCurrentFileChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous);
    void slotFilenameChanged();
    void slotFilenameReturnPressed();
    void slotFileMenuPopup();
    void slotCopyMenuPopup();
    void slotSwapMenuPopup();
    void slotClearMenuPopup();
    void slotCapitalizeMenuPopup();
    void slotResetMenuPopup();
    void slotPathChanged(QString path);
    
    void slotGenerateTags();
    void slotEditTags();
    void slotUpdateTags();
    
    void slotCopyDArtist();
    void slotCopyDArtistAlbum();
    void slotCopyDAlbum();
    
    void slotExtractTrack();
    void slotExtractTitle();
    void slotExtractArtist();
    void slotExtractAlbum();
    void slotExtractYear();
    
    void slotSetAsArtist();
    void slotSetAsTitle();
    void slotSetAsAlbum();
    void slotSetAsTrack();
    void slotSetAsYear();
    void slotSetAsComment();
    void slotSetAsGenre();
    
    void slotSwapArtistTitle();
    void slotSwapAlbumTitle();
    void slotSwapAlbumArtist();
    
    void slotResetFilename();
    void slotRestetAllToSaved();
    void slotResetAllToOriginal();
    void slotResetArtist();
    void slotResetTitle();
    void slotResetAlbum();
    
    void slotClearAlbum();
    void slotClearArtist();
    void slotClearTitle();
    void slotClearComment();
    void slotClearTrack();
    void slotClearGenre();
    
    void slotCapitalizeFilename();
    void slotCapitalizeTags();
    void slotCapitalizeArtist();
    void slotCapitalizeTitle();
    void slotCapitalizeAlbum();
    void slotConvertToUtf8();
    
    void slotFnameSelectionChanged();
    void slotSeparatorChange(QTreeWidgetItem* item);
    void slotItemMenuPopup(const QPoint p);
    void slotOriginalFnameMenuPopup(const QPoint p);
    void slotTmpTextMenuPopup();

    void slotDirDoubleClicked(const QModelIndex& idx);
    void slotFileDoubleClicked(const QModelIndex& idx);
    void slotFileClicked(const QModelIndex& idx);
    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slotCurrentFileChanged(const QModelIndex& current, const QModelIndex& previous);
    
    void slotSaveAll();
    void slotUpdateGui();
    
protected:
    FileBrowser m_browser;
    FileBrowserModel m_browserModel;
    FileListModel m_fileListModel;
    
    TagFile* m_currentFile = nullptr;
    QList<TagFile*> m_selectedFiles;
    
    QMenu* m_filenameMenu = nullptr;
    QMenu* m_fileMenu = nullptr;
    QMenu* m_extractMenu = nullptr;
    QMenu* m_copyMenu = nullptr;
    QMenu* m_swapMenu = nullptr;
    QMenu* m_clearMenu = nullptr;
    QMenu* m_capitalizeMenu = nullptr;
    QMenu* m_resetMenu = nullptr;
    
    WTagEditor* m_tagEditor = nullptr;
    QTimer m_guiTimer;
    
    QString m_renamepattern;
    
    QString m_autoselectiontext;
    QString m_selectedtext;     // text for filename ("set as") menu
    
    
    void createMenus();
    void deleteMenus();
    
    void parseFilename(QString fname, const TagFile* tf);
    void keyPressEvent(QKeyEvent* e);
    
    //TagFile* tagFileFromItem(QTreeWidgetItem* item);
};

#endif
