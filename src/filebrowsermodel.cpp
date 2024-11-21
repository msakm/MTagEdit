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
#include "filebrowsermodel.h"

#include <QDebug>
#include <QFileIconProvider>
#include <QRandomGenerator>

FileBrowserModel::FileBrowserModel() {
    
}

void FileBrowserModel::setFileBrowser(FileBrowser* pBrowser) {
    if (m_browser) disconnect(m_browser);
    
    m_browser = pBrowser;
    
    if (m_browser) {
        connect(m_browser, SIGNAL(updated()), this, SLOT(slotUpdate()));
        qDebug() << __PRETTY_FUNCTION__ << m_browser->currentDir();
        slotUpdate();
    }
}

int FileBrowserModel::rowCount(const QModelIndex& parent) const {
    (void) parent;
    int rows = m_fileInfoList.size();
    
    return rows;
}

int FileBrowserModel::columnCount(const QModelIndex& parent) const {
    (void) parent;
    
    return 3;   // FIXME
}

QVariant FileBrowserModel::data(const QModelIndex& index, int role) const {
    int rowidx = index.row();
    if (rowidx >= m_fileInfoList.size()) return QVariant();
    //QFileInfo fi = m_browser->getFileInfo(rowidx, m_showDirs, m_showFiles);
    QFileInfo fi = m_fileInfoList.at(rowidx);
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Filename:
            if (fi.isDir()) return fi.fileName();
            else return fi.completeBaseName();
            break;
        case Extension:
            if (fi.isDir()) return QString("");
            else return fi.suffix();
            break;
        case FileSize:
            if (fi.isDir()) return QString("<DIR>");
            else return humanReadableSize(fi.size());
            break;
        }
    }
    
    if (role == Qt::DecorationRole) {
        if (index.column() == Filename) {
            return m_iconProvider.icon(fi);
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == FileSize) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    
    return QVariant();
}

QVariant FileBrowserModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) return QVariant();
    
    if (role == Qt::DisplayRole) {
        switch (section) { // TODO: column names
        case 0: return QString("Filename");  break;
        case 1: return QString("Ext");       break;
        case 2: return QString("Size");      break;
        default: return QString::number(section+1); break;
        }
    }
    
    return QVariant();
}

QFileInfo FileBrowserModel::fileInfo(const QModelIndex& index) const {
    //QFileInfo fi = m_browser->getFileInfo(index.row(), m_showDirs, m_showFiles);
    if (index.row() >= m_fileInfoList.size()) return QFileInfo();
    
    QFileInfo fi = m_fileInfoList.at(index.row());
    return fi;
}

int FileBrowserModel::getFileIndex(QFileInfo finfo) const {
    return m_fileInfoList.indexOf(finfo);
}

void FileBrowserModel::setShowDirs(bool bShow) {
    m_showDirs = bShow;
    slotUpdate();
}

void FileBrowserModel::setShowFiles(bool bShow) {
    m_showFiles = bShow;
    slotUpdate();
}

void FileBrowserModel::slotUpdate() {
    refreshList();
    emit layoutChanged();
}

QString FileBrowserModel::humanReadableSize(long long sbytes) const {
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

void FileBrowserModel::refreshList() {
    m_fileInfoList.clear();
    if (!m_browser) return;
    
    if (m_showDirs) {
        QList<QFileInfo> dirlst = m_browser->dirs();
        m_fileInfoList.append(QFileInfo(".."));
        m_fileInfoList.append(dirlst);
    }
    
    if (m_showFiles) {
        QList<QFileInfo> filelst = m_browser->files();
        m_fileInfoList.append(filelst);
    }
}
