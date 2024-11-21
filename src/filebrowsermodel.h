/***************************************************************************
     Copyright          : (C) 2023 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef FILEBROWSERMODEL_H
#define FILEBROWSERMODEL_H

#include "filebrowser.h"

#include <QAbstractTableModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QList>

typedef enum {
    Filename    = 0,
    Extension   = 1,
    FileSize    = 2,
    DateModified = 3,
    LastColumn
} column_t;

class FileBrowserModel : public QAbstractTableModel {
    Q_OBJECT
public:
    FileBrowserModel();
    void setFileBrowser(FileBrowser* pBrowser);
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QFileInfo fileInfo(const QModelIndex& index) const;
    int getFileIndex(QFileInfo finfo) const;
    
    void setShowDirs(bool bShow);
    void setShowFiles(bool bShow);
    
protected slots:
    void slotUpdate();
    
private:
    FileBrowser* m_browser = nullptr;
    QFileIconProvider m_iconProvider;
    QList<QFileInfo> m_fileInfoList;

    bool m_showFiles = true;
    bool m_showDirs = true;
    bool m_showIcons = true;
    
    QString humanReadableSize(long long sbytes) const;
    void refreshList();
    void readTags();
};

#endif // FILEBROWSERMODEL_H
