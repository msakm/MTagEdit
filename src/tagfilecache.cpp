/***************************************************************************
     Copyright          : (C) 2017 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "tagfilecache.h"
#include "tagreaderthread.h"
#include "tagwriterthread.h"

#include <QDebug>

TagFileCache::~TagFileCache() {
    clearCache();
}

TagFile*TagFileCache::getTagFile(const QString& fname) {
    if(fname.isEmpty()) {
        return nullptr;
    }
    
    if( ! m_tagfiles.contains(fname)) {
        TagFile* tf = new TagFile(fname);
        m_tagfiles[fname] = tf;
        TagReaderThreadInst.enqueue(tf);
    }
    
    return m_tagfiles.value(fname);
}

TagFile*TagFileCache::getTagFile(QFileInfo finfo) {
    if(finfo.isFile() && finfo.exists()) {
        QString fname = finfo.absoluteFilePath();
        return getTagFile(fname);
    }
    
    return nullptr;     // file doesn't exist
}

void TagFileCache::keepInCache(TagFile* tf, bool keep) {
    if (keep) {
        m_lstKeepInCache += tf;
    } else {
        m_lstKeepInCache.removeOne(tf);
    }
}

bool TagFileCache::contains(TagFile* tf) {
    if (!tf) return false;
    if (m_tagfiles.values().contains(tf)) return true;
    
    return false;
}

TagDirInfo* TagFileCache::getDirInfo(const QString& dname) {
    if (dname.isEmpty()) {
        return nullptr;
    }
    
    if ( ! m_tdirs.contains(dname)) {
        TagDirInfo* td = new TagDirInfo();
        td->setPath(dname);
        m_tdirs[dname] = td;
    }
    
    return m_tdirs.value(dname);
}

uint TagFileCache::clearTagFileCache(QString keepPath) {
    QList<QString> lst = m_tagfiles.keys();
    
    qDebug() << __PRETTY_FUNCTION__ << " keep =" << keepPath << ", size =" << m_tagfiles.size();

    uint nremoved = 0;
    for(int i = 0; i < lst.size(); ++i) {
        QString file = lst.at(i);
        TagFile* tf = m_tagfiles.value(file);
        if (!tf) { m_tagfiles.remove(file); continue; }
        
        if (m_lstKeepInCache.contains(tf)) {
            // do not remove if the TagFile is in keep list
            continue;
        }

        if(keepPath.compare(tf->fileInfo().absolutePath(), Qt::CaseSensitive) != 0) {
            // do not remove if tag info is changed or modified
            if(tf->isChanged() || tf->isModified()) {
                // FIXME: keep changed files (changed = modified and saved)
                qDebug() << "CACHE: not removing changed file" << tf->fileInfo().absoluteFilePath() 
                         << "changed:" << tf->isChanged() << "modified:" << tf->isModified();
            } else {
                //qDebug() << "removing from cache:" << tf->fileInfo().absoluteFilePath();
                TagReaderThreadInst.remove(tf);
                delete m_tagfiles.take(file);
                nremoved++;
            }
        }
    }
    
    qDebug() << "TagFileCache:clearTagFileCache: finished, current size =" << m_tagfiles.size()
             << ", removed =" << nremoved;
    return nremoved;
}

uint TagFileCache::clearAllTagFileCache(bool keepChanged) {
    uint cnt = 0;
    QList<QString> files = m_tagfiles.keys();
    qDebug() << "TagFileCache: clear all file cache: " << files.size();
    for(int i = 0; i < files.size(); ++i) {
        QString file = files.at(i);
        TagFile* tf = m_tagfiles.value(file);
        if(!tf) {
            m_tagfiles.remove(file);
            continue;
        }
        
        // do not remove changed
        if(tf->isChanged() && keepChanged) continue;
        
        TagReaderThreadInst.remove(tf);
        delete m_tagfiles.take(file);
        cnt++;
    }
    
    return cnt;
}

void TagFileCache::clearDirInfoCache() {
    QList<QString> dirs = m_tdirs.keys();
    qDebug() << __PRETTY_FUNCTION__ << dirs.size();
    
    for (int i = 0; i < dirs.size(); ++i) {
        QString dname = dirs.at(i);
        TagDirInfo* td = m_tdirs.value(dname);
        
        m_tdirs.remove(dname);
        delete td;
    }
}

void TagFileCache::clearCache() {
    clearDirInfoCache();
    clearAllTagFileCache(false);
}

void TagFileCache::updateFilename(const QString& oldName, const QString& newName) {
    if(oldName == newName) return;  // no change
    TagFile* tf = getTagFile(oldName);
    m_tagfiles.remove(oldName);
    if(newName.isEmpty()) {
        if (tf) delete tf;
        return;
    }
    if (tf) m_tagfiles[newName] = tf;
}

void TagFileCache::emitTagFileChanged(TagFile* tf) {
    if (m_tagfiles.values().contains(tf))
        emit tagFileChanged(tf);
}

void TagFileCache::emitFileRenamed(QString oldName, QString newName) {
    updateFilename(oldName, newName);
    emit fileRenamed(oldName, newName);
}

QList<TagFile*> TagFileCache::changedFiles() {
    QList<TagFile*> lst;
    
    QList<TagFile*> tflst = m_tagfiles.values();
    
    
    for(int i = 0; i < tflst.size(); ++i) {
        TagFile* tf = tflst.at(i);
        if(!tf) continue;

        if(tf->isChanged()) {
            lst += tf;
        }
    }
    
    qDebug() << "TagfileCache: changed files:" << lst.size() << "of" << tflst.size();
    return lst;
}

int TagFileCache::numCachedFiles() const {
    return m_tagfiles.size();
}

int TagFileCache::numChangedFiles() const {
    QList<TagFile*> tflst = m_tagfiles.values();
    int n = 0;
    
    for(int i = 0; i < tflst.size(); ++i) {
        TagFile* tf = tflst.at(i);
        if(!tf) continue;

        if(tf->isChanged()) n++;
    }
    
    return n;
}

int TagFileCache::numModifiedFiles() const {
    QList<TagFile*> tflst = m_tagfiles.values();
    int n = 0;
    
    for(int i = 0; i < tflst.size(); ++i) {
        TagFile* tf = tflst.at(i);
        if(!tf) continue;

        if(tf->isModified()) n++;
    }
    
    return n;
}

int TagFileCache::numReading() const {
    return TagReaderThreadInst.queueSize();
}

int TagFileCache::numWriting() const {
    return TagWriterThreadInst.queueSize();
}

TagFileCache::TagFileCache() {
    
}

