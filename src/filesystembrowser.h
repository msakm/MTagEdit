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

#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QObject>
#include <QString>

class TagFile;

class FilesystemBrowser : public QObject {
    Q_OBJECT
public:
    FilesystemBrowser(const QString& path = QString(), QObject* parent = nullptr);
    ~FilesystemBrowser();
    
    bool setCurrentDir(const QString& dirName);
    bool cdUp();
    void setPath(const QString& path);
    
    bool showHiddenFiles() const;
    void setShowHiddenFiles(bool bshow);
    
    QList<QFileInfo> dirs();
    QList<QFileInfo> files();
    
    TagFile* getTagFile(QString fname);
    TagFile* getTagFile(QFileInfo finfo);
    
signals:
    void pathChanged(QString path);
    void updated();
    
public slots:

private:
    QDir m_dir;
    bool m_hiddenFiles = false;
        
    QList<QFileInfo> m_lstDirs;
    QList<QFileInfo> m_lstFiles;
    
    void refresh();
    
};

extern FilesystemBrowser* GlobalBrowser;

#endif // FILESYSTEMBROWSER_H
