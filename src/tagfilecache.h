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

#ifndef TAGFILECACHE_H
#define TAGFILECACHE_H

#include "singleton.h"
#include "tagfile.h"

#include <QFileInfo>
#include <QHash>
#include <QObject>
#include <QString>

#define TagFileCacheI TagFileCache::instance()

class TagFileCache : public QObject, public Singleton<TagFileCache> {
    friend class Singleton<TagFileCache>;
    Q_OBJECT
public:
    ~TagFileCache();
    TagFile* getTagFile(const QString& fname);
    TagFile* getTagFile(QFileInfo finfo);
    void keepInCache(TagFile* tf, bool keep = true);
    bool contains(TagFile* tf);
    
    TagDirInfo* getDirInfo(const QString& dname);
    
    // clear all unchanged files from cache except files in keepPath
    uint clearTagFileCache(QString keepPath);
    // clear all files from cache 
    uint clearAllTagFileCache(bool keepChanged = true);
    void clearDirInfoCache();
    void clearCache();
    
    void emitTagFileChanged(TagFile* tf);
    void emitFileRenamed(QString oldName, QString newName);
    
    QList<TagFile*> changedFiles();
    int numCachedFiles() const;
    int numChangedFiles() const;
    int numModifiedFiles() const;
    int numReading() const;
    int numWriting() const;
    
protected:
    TagFileCache();
    void updateFilename(const QString& oldName, const QString& newName);
    
signals:
    void fileRenamed(const QString& oldName, const QString& newName);
    void tagFileChanged(TagFile* tf);
    
private:
    QHash<QString,TagFile*> m_tagfiles;
    QHash<QString, TagDirInfo*> m_tdirs;
    QList<TagFile*> m_lstKeepInCache;   // do not remove from cache
};

#endif // TAGFILECACHE_H
