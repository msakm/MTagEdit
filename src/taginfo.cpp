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

#include "taginfo.h"

#include <QDebug>
#include <QFileInfo>

TagInfo::TagInfo() {
    clear();
}

void TagInfo::clear() {
    m_absFilename = "";
    BPM = "";
    track = 0;
    artist = "";
    title = "";
    album = "";
    genre = "";
    year = 0;
    comment = "";
    rating = 0;
}

bool TagInfo::isEmpty() {
    if(m_absFilename.isEmpty()) return true;
    if(BPM.isEmpty()) return true;
    if(artist.isEmpty()) return true;
    if(title.isEmpty()) return true;
    if(album.isEmpty()) return true;
    if(genre.isEmpty()) return true;
    if(year != 0) return true;
    if(track != 0) return true;
    if(comment.isEmpty()) return true;
    if(rating != 0) return true;
    
    return false;
}

void TagInfo::setAbsoluteFilename(QString fname) {
    m_absFilename = fname;
}

QString TagInfo::absoluteFilename() const {
    return m_absFilename;
}

QString TagInfo::simplifiedFilename() const {
    QFileInfo fi(m_absFilename);
    return fi.completeBaseName();
}

void TagInfo::setSimplifiedFilename(QString sfname) {
    if(sfname.isEmpty()) return;
    QFileInfo fi(m_absFilename);
    QString fname = fi.absolutePath() + "/";
    fname += sfname;
    fname += "." + fi.suffix();
    
    m_absFilename = fname;
}

bool TagInfo::tagsChanged(const TagInfo& t) const {
    if(BPM != t.BPM) return true;
    if(track != t.track) return true;
    if(artist != t.artist) return true;
    if(title != t.title) return true;
    if(album != t.album) return true;
    if(genre != t.genre) return true;
    if(year != t.year) return true;
    if(comment != t.comment) return true;
    if(rating != t.rating) return true;
    
    return false;
}

bool TagInfo::filenameChanged(const TagInfo& t) const {
    if(m_absFilename != t.absoluteFilename()) return true;
    
    return false;
}

bool TagInfo::operator==(const TagInfo& t) const {
    if(m_absFilename != t.absoluteFilename()) return false;
    if(BPM != t.BPM) return false;
    if(track != t.track) return false;
    if(artist != t.artist) return false;
    if(title != t.title) return false;
    if(album != t.album) return false;
    if(genre != t.genre) return false;
    if(year != t.year) return false;
    if(comment != t.comment) return false;
    if(rating != t.rating) return false;
    
    return true;
}

bool TagInfo::operator!=(const TagInfo& t) const {
    return !(*this == t);
}

void TagInfo::printTagInfo() {
    qDebug() << "----------------------------";
    qDebug() << "Tilename:" << m_absFilename;
    qDebug() << "Artist:" << artist;
    qDebug() << "Title:" << title;
    qDebug() << "Album:" << album;
    qDebug() << "Genre:" << genre;
    qDebug() << "Comment:" << comment;
    qDebug() << "----------------------------";
}

/*
TagInfo& TagInfo::operator=(const TagInfo& t) {
    m_absFilename = t.absoluteFilename();
    BPM = t.BPM;
    track = t.track;
    artist = t.artist;
    title = t.title;
    album = t.album;
    genre = t.genre;
    year = t.year;
    comment = t.comment;
    rating = t.rating;
    
    return *this;
}
*/

TagComment::TagComment(QString desc, QString txt, QString lng) {
    description = desc;
    text = txt;
    lang = lng;
}

TagComment::~TagComment() {}
