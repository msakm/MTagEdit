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

#include "stringfilter.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>

StringFilter::StringFilter() {}

StringFilter::StringFilter(const QString& str) : QString(str) {
    createLists();
}

QStringList StringFilter::getSeparators() {
    QStringList lst;                // separators list
    QChar prevChar;                 // previous character
    QString csep;                   // current separator
    
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);    // char at i index
        if(i == 0) {                // first string char
            prevChar = c;
            if(isSeparatorChar(c)) csep = c;
            continue;
        }
        
        if(isSeparatorChar(c)) {
            csep += c;
        } else if(!isSeparatorChar(c) && isSeparatorChar(prevChar)){
            lst += csep;
            csep = "";
        }
        
        prevChar = c;
    }
    
    // add last separator to the list
    if (csep.length() > 0) lst += csep;
    
    m_separators = lst; // save the list
    return lst;
}

QString StringFilter::getWordSeparator() {
    QString sep;
    if (m_wordSepLst.size() > 0) sep = m_wordSepLst.first();
    
    return sep;
}

QStringList StringFilter::getWordSeparators() {
    return m_wordSepLst;
}

QString StringFilter::getItemSeparator() {
    QString sep;
    if (m_itemSepLst.size() > 0) sep = m_itemSepLst.first();

    return sep;
}

QStringList StringFilter::getItemSeparators() {
    return m_itemSepLst;
}

QString StringFilter::getExtension(QString str) {
    str = str.toLower();
    QStringList extList = {".mp4", ".webm", ".flv", ".mp3", ".ogg", ".opus", ".wav", ".avi"};
    foreach(QString ext, extList) {
        if (str.endsWith(ext)) {
            return ext.replace(".", "");
        }
    }
    
    return QString();
}

QString StringFilter::removeExtension() {
    QString ext = getExtension(*this);
    if (!ext.isEmpty()) {
        replace("."+ext, "", Qt::CaseInsensitive);
    }
    return ext;
}

QString StringFilter::removeID() {
    QRegExp rx(" \\[[-_\\w]*\\]$");
    int idx = indexOf(rx);
    if (idx < 0) return QString();
    
    QString str = left(idx);
    QString idstr = mid(idx+1);
    
    operator=(str);
    createLists();
    return idstr;
}

QString StringFilter::removeTrackNumber() {
    QChar prevChar;                 // previous character
    QString csep;                   // current separator
    QString nstr;
    
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);    // char at i index
        if(i == 0) {                // first string char
            prevChar = c;
            if (c.isDigit()) nstr = c;
            else return QString();  // first char is not a digit

            continue;
        }
        
        if (c.isDigit() && prevChar.isDigit()) {
            nstr += c;
        } else if (isSeparatorChar(c)) {
            csep += c;
        } else {
            break;  // not a digit or separator char
        }
        
        prevChar = c;
    }
    
    if (csep.length() == 0) return QString();
    
    if (nstr.length() > 0) {
        replace(nstr + csep, "");
        return nstr;
    }
    
    return QString();
}

/*
void StringFilter::separatorsSplit(QStringList& wordsepl, QStringList& itemsepl) {
    wordsepl.clear();
    itemsepl.clear();
    QStringList sepl = getSeparators();
    QStringList seplsingle = sepl;
    seplsingle.removeDuplicates();
    
    QStringList multicharsepl;
    QStringList singlecharsepl;
    
    QList<float> wweight;
    QList<float> iweight;
    
    for(int i = 0; i < seplsingle.size() ;++i) {
        QString sep = seplsingle.at(i);
        
        float ww = 1;
        float iw = 1;
        
        if(sep.length() > 1) {
            multicharsepl += sep;
            iw *= 2;
        } else if(sep.length() == 1) {
            singlecharsepl += sep;
            ww *= 2;
        }
        
        int cnt = sepl.count(sep);
        // NOTE: sepl size should always be > 0, otherwise loop will be skipped
        float cntrel = float(cnt) / sepl.size();
        
        if(cntrel > 0)  iw += 1.0f / cntrel;
        ww += cntrel;
        
        wweight << ww;
        iweight << iw;
        
        if(sep.contains("-")) iw *= 2;
        
        qDebug() << "Separator count:" << sep << cnt << ", weights (item  word):" << iw << ww;
        
        // weights  '-' => item sep, ' ' => word sep
        // other chars  '_' '.' ',' ';' '%20' ...
        // compare items with tags to decide (if there are tags)
        
    }
    
    // split separators in word and item separator list
    for(int i = 0; i < seplsingle.size(); ++i) {
        QString sep = seplsingle.at(i);
        
        if(sep.contains("-")) {
            itemsepl += sep;
            continue;
        }
        
        wordsepl += sep;
    }
    
    qDebug() << "Single char separators:" << singlecharsepl;
    qDebug() << "Multi char separators: " << multicharsepl;
}
*/

void StringFilter::setSeparators(QStringList wordsepl, QStringList itemsepl) {
    m_wordSepLst = wordsepl;
    m_itemSepLst = itemsepl;
}

QStringList StringFilter::getNumbers() {
    QStringList lst;
    QRegExp rx("([\\d]+)", Qt::CaseInsensitive);
    matchAll(rx, lst);
    return lst;
}

QStringList StringFilter::getWords() {
    QStringList lst;

    QChar pc; // previous character
    QString ctext;
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);
        if(i == 0) {
            pc = c;
            if(isLetterOrNumberChar(c)) ctext = c;
            continue;
        }
        
        if(isLetterOrNumberChar(c)) {
            ctext += c;
        } else if(!isLetterOrNumberChar(c) && isLetterOrNumberChar(pc)){
            lst += ctext;
            ctext = "";
        }
        
        if(i == length() - 1 && isLetterOrNumberChar(c)) {
            lst += ctext;
        }
        pc = c;
    }
    
    return lst;
}

QStringList StringFilter::getBracketStrings() {
    QStringList lst;
    QRegExp rx("[\\(\\[\\{]+(.*)+[\\)\\]\\}]+", Qt::CaseInsensitive);
    rx.setMinimal(true);
    matchAll(rx, lst);
    return lst;
}

QStringList StringFilter::getItems() {
    QString itemsep;
    QStringList itemlst;
    
    QString str = *this;
    QString ext = getExtension(str);
    if (!ext.isEmpty()) {
        str.replace("."+ext, "", Qt::CaseInsensitive);
    }
    
    itemlst += str;     // 
    if(m_itemSepLst.size() == 0) {
        if (!ext.isEmpty()) itemlst += ext;             // append ext to item list
        return itemlst;    // no item separators
    }
    
   
    itemlst.clear();
    QChar prevChar;                 // previous character
    QString csep;                   // current separator
    QString itemstr;                // current item text
    
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);    // char at i index
        if(i == 0) {                // first string char
            prevChar = c;
            if(isSeparatorChar(c)) csep = c;
            else itemstr = c;
            continue;
        }
        
        if(isSeparatorChar(c)) {    // separator char
            //if (!isSeparatorChar(prevChar)) csep = "";  // first sep char after text
            csep += c;
        } else {                    // text char
            if (isSeparatorChar(prevChar)) { // sep complete, check type
                if (m_itemSepLst.contains(csep)) {
                    // item complete, add to list
                    itemlst += itemstr;
                    itemstr = "";
                } else {
                    // append csep to itemstr and continue
                    itemstr += csep;
                }
            }
            
            itemstr += c;
            csep = "";
        }
        
        prevChar = c;
    }
    
    //if (!csep.isEmpty() && !m_itemSepLst.contains(csep)) itemstr += csep;     // skip ending sep?
    if (!itemstr.isEmpty()) itemlst += itemstr; // add last item to the list
    
    if (!ext.isEmpty()) itemlst += ext;             // append ext to item list
    
    return itemlst;
}

QString StringFilter::extractFirstItem(QString itemsep) {
    // !!! FIXME !!!  - only one item no separators, save item separator
    
    QString oldstr = *this;
    
    QString sep;
    if(!itemsep.isEmpty()) {
        sep = itemsep;
    } else {
        sep = getItemSeparator();
    }

    QString item;
    int pos = -1;
    if(!sep.isEmpty()) {
        pos = indexOf(sep, 0, Qt::CaseSensitive);
    }
    if(pos >= 0) {
        item = left(pos);
        *this = mid(pos + sep.length());
    } else {
        item = *this;
        *this = "";
    }
    
    qDebug() << "Extract first item from:" << oldstr << "=" << item << " newstr =" << *this << " sep =" << sep;
    return item;
}

QString StringFilter::extractNumber() {
    QString num;
    int pos = 0;
    bool ok = false;
    for(int i = 0; i < length(); ++i) {
        QChar c = at(i);
        if(isNumberChar(c)) {
            ok = true;
            continue;
        } else if(isSeparatorChar(c)) {
            if(ok) {
                num = mid(pos, i - pos);
                // find end of separator
                for(;i < length(); ++i) {
                    c = at(i);
                    if(!isSeparatorChar(c)) {
                        *this = mid(i, -1);
                        return num;
                    }
                }
            } else {
                pos = i;
            }
        }
    }
    return num;
}

StringFilter& StringFilter::operator=(const QString& str) {
    QString::operator=(str);
    createLists();
    return *this;
}

QString StringFilter::capitalizeWords() {
    QString str = toLower();

    bool capnext = true;
    
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);    // at(i)
        
        if(isSeparatorChar(c)) {
            capnext = true;
            continue;
        }
        
        if(capnext && isLetterChar(c)) {
            str = str.replace(i, 1, c.toUpper());
            capnext = false;
        }
    }
    
    // FIXME:
    return str;
}

QString StringFilter::firstWordUppercase() {
    QString str = toLower();
    
    for(int i = 0; i < length(); ++i) {
        QChar c = operator[](i);    // at(i)
        
        if(isLetterChar(c)) {
            str = str.replace(i, 1, c.toUpper());
            break;
        }
    }
    
    // TODO:

    return str;
}

QString StringFilter::underscoreToSpace() {
    if (!contains(" ")) replace("_", " ");
    return *this;
}

QString StringFilter::spaceToUnderscore() {
    return replace(" ", "_");
}

void StringFilter::createLists() {
    clearLists();
    m_separators = getSeparators();
    splitSeparators();
}

void StringFilter::clearLists() {
    m_separators.clear();
    m_itemSepLst.clear();
    m_wordSepLst.clear();
}

int StringFilter::matchAll(const QRegExp& rx, QStringList& lst, bool clear) {
    int pos = 0, cnt = 0;
    if(clear) lst.clear();
    
    QString str = *this;
    
    while((pos = rx.indexIn(str, pos)) != -1) {
        lst += rx.cap(1);
        pos += rx.matchedLength();
        cnt++;
    }
    return cnt;
}

QStringList StringFilter::getStrings(QRegExp& rx) {
    QStringList lst;
    matchAll(rx, lst);
    return lst;
}

bool StringFilter::isLetterChar(const QChar& c) {
    return (c.category() == QChar::Letter_Lowercase)
        || (c.category() == QChar::Letter_Uppercase)
        || (c.category() == QChar::Letter_Titlecase);
}

bool StringFilter::isNumberChar(const QChar& c) {
    bool b = (c.category() == QChar::Number_Letter);
    //b = b || (c.category() == QChar::Number_DecimalDigit);

    return b;
}

bool StringFilter::isLetterOrNumberChar(QChar& c) {
    return isLetterChar(c) || isNumberChar(c);
}

bool StringFilter::isSeparatorChar(const QChar& c) {
    bool b = (c.category() == QChar::Separator_Space)
          || (c.category() == QChar::Punctuation_Connector)
          || (c.category() == QChar::Punctuation_Dash)
          || (c.category() == QChar::Punctuation_Other)
          || (c.category() == QChar::Symbol_Math);
    
    return b;
}

void StringFilter::splitSeparators() {
    QStringList multiCharLst;
    QStringList singleCharLst;
    m_itemSepLst.clear();
    m_wordSepLst.clear();
    QString spacechars = "_ %&,.";
    QString itemchars = "-|;–：—";
    
    QStringList sepLstSimple = m_separators;
    sepLstSimple.removeDuplicates();
    
    // single / multi char sep split
    foreach(QString sep, sepLstSimple) {
        if(sep.length() > 1)        multiCharLst  += sep;
        else if(sep.length() == 1)  singleCharLst += sep;
    }
    
    //qDebug() << __PRETTY_FUNCTION__ << m_separators;
    //qDebug() << "duplicates removed:" << sepLstSimple;
    //qDebug() << "single char list:" << singleCharLst;
    //qDebug() << "multi char list:" << multiCharLst;
    
    bool manualCheck = false;
    int scharcnt = singleCharLst.size();
    int mcharcnt = multiCharLst.size();
    
    // one single char separator
    if (scharcnt == 1) {
        QString sep = singleCharLst.at(0);
        if (mcharcnt == 1) {        // single multi char separator
            QString msep = multiCharLst.at(0);
            bool isItemSep = containChars(msep, itemchars);
            
            if (spacechars.contains(sep)) {
                // Artist Name - Song Title (Ext Mix)
                m_wordSepLst += sep;
                
                if (! isItemSep) { 
                    m_wordSepLst += msep;
                } else {
                    m_itemSepLst += msep;
                }
            } else {
                // Artist-Name - Song-Title-(Ext-Mix)
                if(m_separators.count(sep) > 2) m_wordSepLst += sep;
                else {
                    m_itemSepLst += sep;
                    manualCheck = true;
                }
                
                if (! isItemSep) { 
                    m_wordSepLst += msep;
                } else {
                    m_itemSepLst += msep;
                }
            }
        } else if (mcharcnt > 1) {  // multiple multi char separators
            if (spacechars.contains(sep)) {
                // Artist & Artist2 Name - Song Title (Ext Mix)
                m_wordSepLst += sep;
                foreach (QString msep, multiCharLst) {
                    if (containChars(msep, itemchars)) {
                        if (msep.contains(".")) {
                            QString nmsep = msep;
                            nmsep.replace(".", ""); // can also contain '-' or other item characters
                            if (containChars(nmsep, itemchars)) m_itemSepLst += msep;
                            else m_wordSepLst += msep;
                            manualCheck = true;
                        } else m_itemSepLst += msep;
                    } else { m_wordSepLst += msep; }
                }
                if (m_itemSepLst.size() == 0) manualCheck = true;
            } else {
                // Artist & Artist2-Name - Song-Title-(Ext-Mix)
                foreach (QString msep, multiCharLst) {
                    if (containChars(msep, itemchars)) { m_itemSepLst += msep; }
                    else { m_wordSepLst += msep; }
                }
                
                manualCheck = true;
                if (m_itemSepLst.size() == 0) { m_itemSepLst += sep; }
                else { m_wordSepLst += sep; }
            }
        } else {                    // no multi char separator
            if (spacechars.contains(sep)) m_wordSepLst += sep;
            else if (itemchars.contains(sep)) m_itemSepLst += sep;
            else {
                manualCheck = true;
                m_itemSepLst += sep;
            }
        }
    // multiple single char separator
    } else if (scharcnt > 1) {
        if (mcharcnt == 1) {        // single multi char separator
            QString msep = multiCharLst.at(0);
            if (containChars(msep, itemchars)) {
                m_itemSepLst += msep;
            } else {
                m_wordSepLst += msep;
                manualCheck = true;
            }
            foreach(QString sep, singleCharLst) {
                if (spacechars.contains(sep)) {
                    // Artist_Name - Song Title
                    m_wordSepLst += sep;
                } else {
                    // Artist-Name - Song_Title
                    manualCheck = true;
                    if (m_itemSepLst.size() == 0) {
                        if (itemchars.contains(sep)) m_itemSepLst += sep;
                        else m_wordSepLst += sep;
                    } else {
                        if (itemchars.contains(sep)) {
                            if (m_separators.count(sep) > 2) m_wordSepLst += sep;
                            else {
                                if (m_separators.last() == sep) m_itemSepLst += sep;
                                else m_wordSepLst += sep;
                            }
                        } else m_wordSepLst += sep;
                    }
                }
            }
        } else if (mcharcnt > 1) {  // multiple multi char separators
            // Artist & Artist2-Name - Song_Title (Ext--mix)
            foreach(QString msep, multiCharLst) {
                if (containChars(msep, itemchars)) {
                    // 05. Artist feat. Artist2 - Title
                    if (msep.contains(".")) {
                        m_wordSepLst += msep;
                        manualCheck = true;
                    } else m_itemSepLst += msep;
                } else m_wordSepLst += msep;
                
                if (m_itemSepLst.size() > 1) manualCheck = true;
            }
            
            foreach(QString sep, singleCharLst) {
                if (m_itemSepLst.size() == 0) {
                    if (itemchars.contains(sep)) m_itemSepLst += sep;
                    else m_wordSepLst += sep;
                } else {
                    if (itemchars.contains(sep)) {
                        if (m_separators.count(sep) > 2) m_wordSepLst += sep;
                        else {
                            if (sep == "-" && m_itemSepLst.size() > 0) {
                                m_wordSepLst += sep;
                                manualCheck = true;
                            } else m_itemSepLst += sep;
                        }
                    } else m_wordSepLst += sep;
                }
            }
        } else {                    // no multi char separator
            foreach(QString sep, singleCharLst) {
                if (itemchars.contains(sep)) {
                    m_itemSepLst += sep;
                    if (m_itemSepLst.size() > 1) manualCheck = true;
                } else {
                    m_wordSepLst += sep;
                }
            }
        }
    // no single char separator
    } else {
        if (mcharcnt == 1) {        // single multi char separator
            QString msep = multiCharLst.at(0);
            if (containChars(msep, itemchars)) {
                // Artist - Title
                m_itemSepLst += msep;
            } else {
                // Artist   Title
                if (containChars(msep, ",&")) {
                    m_wordSepLst += msep;
                } else {
                    m_itemSepLst += msep;
                }
                manualCheck = true;
            }
        } else if (mcharcnt > 1) {  // multiple multi char separators
            // Artist & Artist2 feat. Artist3 - Title
            foreach(QString msep, multiCharLst) {
                if (containChars(msep, itemchars)) {
                    m_itemSepLst += msep;
                    continue;
                } else if (msep.contains(".")) {
                    // 01. Title1, Title2
                    m_itemSepLst += msep;
                    manualCheck = true;
                } else {
                    m_wordSepLst += msep;
                    manualCheck = true;
                }
                
                if (m_itemSepLst.size() > 1) manualCheck = true;
            }
        }
    }
    
    //qDebug() << "item sep list:" << m_itemSepLst;
    //qDebug() << "word sep list:" << m_wordSepLst;
    //qDebug() << "need to check:" << manualCheck;
    m_manualCheck = manualCheck;
}

bool StringFilter::containChars(QString str, QString chars) {
    foreach (QChar c, chars) {
        if (str.contains(c)) return true;
    }
    return false;
}

QString StringFilter::bracket_sep_replace(QString text, QString sep, QString nsep) {
    if (sep.isEmpty() || nsep.isEmpty()) return QString();
    QRegExp rx("[\\(\\[\\{]+(.*)+[\\)\\]\\}]+", Qt::CaseInsensitive);
    rx.setMinimal(true);
    
    if(! text.contains(sep)) return text;   // nothing to replace
    QString ntext = text;
    
    rx.setPattern("[\\(]+(.*)+[\\)]+");
    int pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1) {
        QString btext = rx.cap(1);  // bracket text
        QString btext2 = btext;
        btext2.replace(sep, nsep);
        pos += rx.matchedLength();
        ntext.replace(btext, btext2);
    }
    
    rx.setPattern("[\\[]+(.*)+[\\[]+");
    pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1) {
        QString btext = rx.cap(1);  // bracket text
        QString btext2 = btext;
        btext2.replace(sep, nsep);
        pos += rx.matchedLength();
        ntext.replace(btext, btext2);
    }
    
    rx.setPattern("[\\{]+(.*)+[\\}]+");
    pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1) {
        QString btext = rx.cap(1);  // bracket text
        QString btext2 = btext;
        btext2.replace(sep, nsep);
        pos += rx.matchedLength();
        ntext.replace(btext, btext2);
    }
    
    //qDebug() << "--REPLACE-- bracket sep:" << text << "-->" << ntext;
    return ntext;
}
