/***************************************************************************
     Copyright          : (C) 2015 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "filesystembrowser.h"
#include "tagfilecache.h"

FilesystemBrowser::FilesystemBrowser(const QString& path, QObject* parent) : QObject(parent) {
    m_dir.setPath(path);
    refresh();
}

FilesystemBrowser::~FilesystemBrowser() {
    TagFileCacheI.clearAllTagFileCache(false);
}

bool FilesystemBrowser::setCurrentDir(const QString& dirName) {
    bool ret = m_dir.cd(dirName);
    refresh();
    return ret;
}

bool FilesystemBrowser::cdUp() {
    bool ret = m_dir.cdUp();
    refresh();
    return ret;
}

void FilesystemBrowser::setPath(const QString& path) {
    m_dir.setPath(path);
    refresh();
}

bool FilesystemBrowser::showHiddenFiles() const {
    return m_hiddenFiles;
}

void FilesystemBrowser::setShowHiddenFiles(bool bshow) {
    if (bshow != m_hiddenFiles) {
        m_hiddenFiles = bshow;
        refresh();
    }
}

QList<QFileInfo> FilesystemBrowser::dirs() {
    return m_lstDirs;
}

QList<QFileInfo> FilesystemBrowser::files() {
    return m_lstFiles;
}


TagFile* FilesystemBrowser::getTagFile(QString fname) {
    return TagFileCacheI.getTagFile(fname);
}

TagFile* FilesystemBrowser::getTagFile(QFileInfo finfo) {
    return TagFileCacheI.getTagFile(finfo);
}

void FilesystemBrowser::refresh() {
    QDir::Filters dfilters = QDir::Drives | QDir::Dirs | QDir::AllDirs | QDir::NoDotAndDotDot;
    if (m_hiddenFiles) dfilters |= QDir::Hidden;
    
    QDir::Filters ffilters = QDir::Files;
    if (m_hiddenFiles) ffilters |= QDir::Hidden;
    
    m_lstDirs  = m_dir.entryInfoList(dfilters, QDir::Name | QDir::IgnoreCase | QDir::DirsFirst);
    m_lstFiles = m_dir.entryInfoList(ffilters, QDir::Name | QDir::IgnoreCase);
    
    emit pathChanged(m_dir.absolutePath());
    emit updated();
    TagFileCacheI.clearTagFileCache(m_dir.absolutePath());
}
