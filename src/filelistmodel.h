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
#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractTableModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QList>
#include <QObject>

class FileBrowser;
class TagFile;

class FileListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    FileListModel();
    void setFileBrowser(FileBrowser* browser);
    
    typedef enum {
        Filename = 0,
        Extension,
        FileSize,
        Status,
        TagTypes,
        
        Artist,
        Title,
        Album,
        Genre,
        Year,
        Track,
        Comment,
        LastColumn
    } column;
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QFileInfo getFileInfo(const QModelIndex& index);
    TagFile* getTagFile(const QModelIndex& index);
    
    QList<column> columnList() const;
    int columnWidth(int c) const;
    QString columnName(int c) const;
    
protected slots:
    void slotUpdate();
    void slotFileRenamed(const QString& oldName, const QString& newName);
    void slotTagFileChanged(TagFile* tf);
    
private:
    FileBrowser* m_browser;
    QFileIconProvider m_iconProvider;
    QList<QFileInfo> m_fileInfoList;
    QList<column> m_columns;
    
    bool m_showFiles = true;
    bool m_showDirs = true;
    bool m_showIcons = true;
    
    void refreshList();
    QString humanReadableSize(long long sbytes) const;
    void updateRow(int rowidx);
};

#endif // FILELISTMODEL_H
