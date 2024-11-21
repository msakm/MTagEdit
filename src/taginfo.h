/***************************************************************************
     Copyright          : (C) 2014 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TAGINFO_H
#define TAGINFO_H

#include <QDateTime>
#include <QString>

class TagComment;

class TagInfo {
public:
    TagInfo();
    //TagInfo(const TagInfo& t);
    
    void clear();
    bool isEmpty();
    
    void setAbsoluteFilename(QString fname);
    QString absoluteFilename() const;
    QString simplifiedFilename() const;
    void setSimplifiedFilename(QString sfname);
    
    bool tagsChanged(const TagInfo& t) const;
    bool filenameChanged(const TagInfo& t) const;
    bool operator==(const TagInfo& t) const;
    bool operator!=(const TagInfo& t) const;
    //TagInfo& operator=(const TagInfo& t);
    
    QString BPM;
    unsigned int track;
    QString artist;
    QString title;
    QString album;
    QString genre;
    unsigned int year;
    QString comment;

    int rating;
    
    void printTagInfo();
private:
    QString m_absFilename;
    //QHash<QString> m_customTags;
};

class TagComment {
public:
    TagComment(QString desc, QString txt, QString lng);
    ~TagComment();
    
    QString description;
    QString lang;
    QString text;
};

#endif // TAGINFO_H
