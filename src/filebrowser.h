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
#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QDir>
#include <QFileInfo>
#include <QObject>

class TagFile;

class FileBrowser : public QObject {
    Q_OBJECT
public:
    FileBrowser();
    ~FileBrowser();
    
    QString currentDir() const; 
    bool setCurrentDir(const QString& dname);
    void setPath(const QString& path);
    bool cdUp();
    
    bool showHiddenFiles() const;
    void setShowHiddenFiles(bool bshow);
    
    QList<QFileInfo> dirs();
    QList<QFileInfo> files();
    int dirCount() const;
    int fileCount() const;
    
    TagFile* getTagFile(const QString& fname);
    TagFile* getTagFile(QFileInfo finfo);
    
    void refresh();

signals:
    void updated();
    void pathChanged(QString path);
    
protected:
    QDir m_dir;
    bool m_hiddenFiles = false;
    
    QList<QFileInfo> m_lstDirs;
    QList<QFileInfo> m_lstFiles;
};

#endif // FILEBROWSER_H
