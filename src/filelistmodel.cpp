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
#include "filelistmodel.h"
#include "filebrowser.h"
#include "tagfilecache.h"

#include <QDebug>

FileListModel::FileListModel() {
    m_columns = {Filename, Extension, FileSize, Status, TagTypes, Artist, Title, Album, Track, Genre, Year, Comment};
    
    connect(&TagFileCacheI, &TagFileCache::fileRenamed, this, &FileListModel::slotFileRenamed);
    connect(&TagFileCacheI, &TagFileCache::tagFileChanged, this, &FileListModel::slotTagFileChanged);
}

void FileListModel::setFileBrowser(FileBrowser* browser) {
    if (m_browser) disconnect(m_browser);
    
    m_browser = browser;
    
    if (m_browser) {
        connect(m_browser, SIGNAL(updated()), this, SLOT(slotUpdate()));
        qDebug() << __PRETTY_FUNCTION__ << m_browser->currentDir();
        slotUpdate();
    }
}

int FileListModel::rowCount(const QModelIndex&) const {
    return m_fileInfoList.size();
}

int FileListModel::columnCount(const QModelIndex&) const {
    return m_columns.size();
}

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (section < 0 || section > m_columns.size()) return QVariant();
        
        if (role == Qt::DisplayRole) {
            return columnName(m_columns.at(section));
        }
    }
    
    return QVariant();
}

QVariant FileListModel::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= m_fileInfoList.size()) return QVariant();  // invalid index
    if (index.column() < 0 || index.column() >= m_columns.size()) return QVariant();
    
    column c = m_columns.at(index.column());
    
    QFileInfo fi = m_fileInfoList.at(index.row());
    if (fi.isDir()) {
        switch (c) {
        case Filename: 
            if (role == Qt::DisplayRole) return fi.fileName();
            if (role == Qt::DecorationRole) return m_iconProvider.icon(fi);
            break;
        case FileSize:
            if (role == Qt::DisplayRole) return QString("<DIR>");
            break;
        default: return QVariant();
        }
    }
    
    if (fi.isFile()) {
        TagFile* tf = TagFileCacheI.getTagFile(fi);
        switch (c) {
        case Filename: 
            if (role == Qt::DisplayRole) {
                if (tf) return tf->simplifiedNewFilename();
                return fi.fileName();
            }
            if (role == Qt::DecorationRole) return m_iconProvider.icon(fi);
            break;
        case Extension:
            if (role == Qt::DisplayRole) return fi.suffix();
            break;
        case FileSize:
            if (role == Qt::DisplayRole) return humanReadableSize(fi.size());
            break;
        default: break;
        }
        
        if (!tf) return QVariant(); // tag file info not available
        
        switch (c) {
        case Status:
            if (role == Qt::DisplayRole) {
                if (tf->isChanged()) return QString("Ch");
                if (tf->isModified()) return QString("M");
                return QString("S");
            }
            break;
        case Artist:
            if (role == Qt::DisplayRole) return tf->artist();
            break;
        case Title:
            if (role == Qt::DisplayRole) return tf->title();
            break;
        case Album:
            if (role == Qt::DisplayRole) return tf->album();
            break;
        case Genre:
            if (role == Qt::DisplayRole) return tf->genre();
            break;
        case Comment:
            if (role == Qt::DisplayRole) return tf->comment();
            break;
        case Year:
            if (role == Qt::DisplayRole) {
                if (tf->year() > 0) return tf->year();
            }
            break;
        case Track:
            if (role == Qt::DisplayRole) {
                if (tf->track() > 0) return tf->track();
            }
            break;

        case TagTypes:
            if (role == Qt::DisplayRole) {
                QString tagstr;
                int tags = tf->tagsUsed();
                if (tags == TagFile::TT_None)   tagstr = "-";
                if (tags & TagFile::TT_ID3v2)   tagstr += "2 ";
                if (tags & TagFile::TT_ID3v1)   tagstr += "1 ";
                if (tags & TagFile::TT_APE)     tagstr += "A ";
                if (tags & TagFile::TT_XIPH)    tagstr += "X ";
                if (tags & TagFile::TT_RIFF)    tagstr += "R ";
                if (tags & TagFile::TT_MP4)     tagstr += "MP4 ";
                return tagstr;
            }
            break;
        default: return QVariant();
        }
    }
    
    return QVariant();
}

QFileInfo FileListModel::getFileInfo(const QModelIndex& index) {
    if (index.row() < 0 || index.row() >= m_fileInfoList.size()) return QFileInfo();
    
    return m_fileInfoList.at(index.row());
}

TagFile* FileListModel::getTagFile(const QModelIndex& index) {
    QFileInfo fi = getFileInfo(index);
    return TagFileCacheI.getTagFile(fi);
}

QList<FileListModel::column> FileListModel::columnList() const {
    return m_columns;
}

int FileListModel::columnWidth(int c) const {
    switch (c) {
    case Filename:  return 300;
    case Extension: return 40;
    case FileSize:  return 60;
    case Status:    return 20;
    case TagTypes:  return 50;
    case Track:     return 40;
    case Year:      return 40;
    case Genre:     return 70;
    case Artist:    return 250;
    case Title:     return 400;
    case Album:     return 250;
    default:        return 200;
    }
}

void FileListModel::slotUpdate() {
    refreshList();
    emit layoutChanged();
}

void FileListModel::slotFileRenamed(const QString& oldName, const QString& newName) {
    qDebug() << __PRETTY_FUNCTION__ << oldName << "-->" << newName;
    if (oldName.isEmpty()) return;
    
    for (int i = 0; i < m_fileInfoList.size(); ++i) {
        QFileInfo fi = m_fileInfoList.at(i);
        if (fi.absoluteFilePath() == oldName) {
            m_fileInfoList.removeAt(i);
            if (newName.isEmpty()) {
                qDebug() << __PRETTY_FUNCTION__ << "new name is empty";
            }
            m_fileInfoList.insert(i, QFileInfo(newName));
            updateRow(i);
            break;
        }
    }
}

void FileListModel::slotTagFileChanged(TagFile* tf) {
    if (!tf && !TagFileCacheI.contains(tf)) return;
    qDebug() << __PRETTY_FUNCTION__ << tf->fileInfo().absoluteFilePath();
    int idx = m_fileInfoList.indexOf(tf->fileInfo());
    
    if (idx >= 0)  updateRow(idx);
}

void FileListModel::refreshList() {
    beginResetModel();
    m_fileInfoList.clear();
    if (!m_browser) {
        endResetModel();
        return;
    }
    
    if (m_showDirs) {
        QList<QFileInfo> dirlst = m_browser->dirs();
        m_fileInfoList.append(QFileInfo(".."));
        m_fileInfoList.append(dirlst);
    }
    
    if (m_showFiles) {
        QList<QFileInfo> filelst = m_browser->files();
        m_fileInfoList.append(filelst);
        
        // create TagFile in cache and read tags (in reader thread)
        foreach (QFileInfo fi, filelst)  { TagFileCacheI.getTagFile(fi); }
    }
    
    endResetModel();
}

QString FileListModel::humanReadableSize(long long sbytes) const {
    QString strsize = "";
    
    int unit = 0;
    
    double ds = sbytes;
    while (ds > 1000 && unit < 4) {
        ds /= 1024;
        unit++;
    }
    
    if (unit == 0) strsize = QString::number(int(ds));
    else strsize = QString::number(ds, 'f', 2);
    
    switch(unit) {
    case 0: strsize += " B"; break;
    case 1: strsize += " kB"; break;
    case 2: strsize += " MB"; break;
    case 3: strsize += " GB"; break;
    default: strsize += " TB"; break;
    }
    
    return strsize;
}

void FileListModel::updateRow(int rowidx) {
    if (rowidx < 0 || rowidx > m_fileInfoList.size()) return;
    
    emit dataChanged(index(rowidx, 0), index(rowidx, m_columns.size() - 1));
}

QString FileListModel::columnName(int c) const {
    switch (c) {
    case Filename:  return QString("Filename");
    case Extension: return QString("Ext");
    case FileSize:  return QString("Size");
    case Status:    return QString("St");
    case TagTypes:  return QString("Tags");
    
    case Artist:    return QString("Artist");
    case Title:     return QString("Title");
    case Album:     return QString("Album");
    case Genre:     return QString("Genre");
    case Year:      return QString("Year");
    case Track:     return QString("Track");
    case Comment:   return QString("Comment");
    default:        return QString();
    }
}
