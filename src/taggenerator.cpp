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

#include "taggenerator.h"

#include "stringfilter.h"
#include "tagfile.h"

#include <QDebug>

TagGenerator::TagGenerator() {
    
}

void TagGenerator::setTagFile(TagFile* tf) {
    if(!tf) {
        m_tagfile = nullptr;
        return;
    }
    
    m_tagfile = tf;
    m_sfilter = m_tagfile->simplifiedNewFilename();
}

void TagGenerator::generateTags() {
    if (!m_tagfile) return;
    qDebug() << "TagGenerator: Generating tags for file:" << m_sfilter;
    
    m_sfilter.removeExtension();
    QString strid = m_sfilter.removeID();
    if (!strid.isEmpty()) m_tagfile->setComment(strid);
    
    // replace _ with space (only if text doesn't contain spaces)
    m_sfilter.underscoreToSpace();
    // get and identify separators
    //m_sfilter.createLists();
    
    m_sfilter.replace("u0026", "&");
    m_sfilter.replace("%026", "&");
    m_sfilter.replace("%26", "&");
    m_sfilter.replace("%20", " ");
    
    QString tracknum = m_sfilter.removeTrackNumber();   // remove number from the begining
    QStringList numberLst = m_sfilter.getNumbers();
    
    if (tracknum.length() > 0) {
        uint tnum = tracknum.toUInt();
        if (tnum > 0) m_tagfile->setTrack(tnum);
    } else if (numberLst.size() > 0) { // try next number
        QString num = numberLst.first();
        int idx = m_sfilter.indexOf(num);
        //qDebug() << "first number is" << num << num.toUInt() << "at idx =" << idx;
        if (idx >= 0 && idx < m_sfilter.length() / 2) {
            uint n = num.toUInt();
            if (n < 300) m_tagfile->setTrack(n);
        }
    }
    
    m_sfilter.createLists();
    // get items
    QStringList itemLst = m_sfilter.getItems();
    QStringList tmpLst = itemLst;
    // remove extensions and youtube ids
    foreach (QString itemstr, tmpLst) {
        if (isExtension(itemstr)) itemLst.removeAll(itemstr);
    }
    
    foreach (QString num, numberLst) {  // find year
        int n = num.toInt();
        if (n > 1900 && n < 3000) m_tagfile->setYear(uint(n));
    }
    
    // remove first number item (track)
    if (itemLst.size() > 0) {
        QString itemstr = itemLst.at(0);
        if (itemstr.toUInt() > 0 && itemLst.size() > 1) {
            itemLst.removeFirst();
        }
    }
    
    // test only
    if (itemLst.size() == 1) {
        m_tagfile->setTitle(itemLst.at(0));
    } else if (itemLst.size() == 2) {
        m_tagfile->setArtist(itemLst.at(0));
        m_tagfile->setTitle(itemLst.at(1));
    } else if (itemLst.size() == 3) {
        m_tagfile->setArtist(itemLst.at(0));
        m_tagfile->setTitle(itemLst.at(1));
        m_tagfile->setAlbum(itemLst.at(2));
    } else if (itemLst.size() == 0) {
        // no items
    } else {
        m_tagfile->setArtist(itemLst.at(0));
        m_tagfile->setTitle(itemLst.at(1));
        m_tagfile->setAlbum(itemLst.at(2));
        
    }
    
    // use stored tags
    
    // rename file by tags
}

void TagGenerator::test() {
    detectSeparators();
}


QStringList TagGenerator::separators() {
    return QStringList();   // TODO
    
}

QStringList TagGenerator::wordSeparators() {
    return QStringList();   // TODO
    
}

QStringList TagGenerator::itemSeparators() {
    return QStringList();   // TODO
    
}

void TagGenerator::detectSeparators() {
    if(!m_tagfile) return;
    
    QStringList lst_separators;
    QStringList lst_words;
    QStringList lst_nums;
    QStringList lst_bracketComs;
    StringFilter stringf;
    
    stringf = m_tagfile->simplifiedFilename();
    
    lst_nums = stringf.getNumbers();
    lst_separators = stringf.getSeparators();
    lst_words = stringf.getWords();
    lst_bracketComs = stringf.getBracketStrings();
    
    qDebug() << "=====================";
    qDebug() << "Parsed filename:" << m_tagfile->filename();
    qDebug() << " Separators:" << lst_separators;
    qDebug() << " Words:     " << lst_words;
    qDebug() << " Numbers:   " << lst_nums;
    qDebug() << " Brackets:  " << lst_bracketComs;
    qDebug() << "=====================";
}

void TagGenerator::capitalizeFilename() {
    if(!m_tagfile) return;
    
}

void TagGenerator::capitalizeTags() {
    if(!m_tagfile) return;
    
}

bool TagGenerator::isExtension(QString str) {
    str = str.toLower();
    if (str.startsWith("mp4")) return true;
    if (str.startsWith("webm")) return true;
    if (str.startsWith("flv")) return true;
    
    return false;
}
