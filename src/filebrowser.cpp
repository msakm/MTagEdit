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
#include "filebrowser.h"
#include "tagfilecache.h"

FileBrowser::FileBrowser() {
    m_dir.setPath(QDir::homePath());
    refresh();
}

FileBrowser::~FileBrowser() {
    TagFileCacheI.clearAllTagFileCache(false);
}

QString FileBrowser::currentDir() const {
    return m_dir.absolutePath();
}

bool FileBrowser::setCurrentDir(const QString& dname) {
    bool ret = m_dir.cd(dname);
    refresh();
    return ret;
}

void FileBrowser::setPath(const QString& path) {
    m_dir.setPath(path);
    
    if (!m_dir.exists()) {
        QString existingPath(path);
        while (!QFileInfo::exists(existingPath)) {
            const QString previousPath(existingPath);
            existingPath = QFileInfo(existingPath).dir().absolutePath();
            if (existingPath == previousPath) {
                break;
            }
        }
        m_dir.setPath(existingPath);
    }
    
    refresh();
}

bool FileBrowser::cdUp() {
    bool ret = m_dir.cdUp();
    refresh();
    return ret;
}

bool FileBrowser::showHiddenFiles() const {
    return m_hiddenFiles;
}

void FileBrowser::setShowHiddenFiles(bool bshow) {
    if (bshow != m_hiddenFiles) {
        m_hiddenFiles = bshow;
        refresh();
    }
}

QList<QFileInfo> FileBrowser::dirs() {
    return m_lstDirs;
}

QList<QFileInfo> FileBrowser::files() {
    return m_lstFiles;
}

int FileBrowser::dirCount() const {
    return m_lstDirs.size();
}

int FileBrowser::fileCount() const {
    return m_lstFiles.size();
}

TagFile* FileBrowser::getTagFile(const QString& fname) {
    return TagFileCacheI.getTagFile(fname);
}

TagFile* FileBrowser::getTagFile(QFileInfo finfo) {
    return TagFileCacheI.getTagFile(finfo);
}

void FileBrowser::refresh() {
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
