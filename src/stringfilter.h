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

#ifndef STRINGFILTER_H
#define STRINGFILTER_H

#include <QHash>
#include <QString>
#include <QStringList>

class StringFilter : public QString {
public:
    StringFilter();
    explicit StringFilter(const QString& str);
    
    QStringList getSeparators();
    
    QString getWordSeparator();
    QStringList getWordSeparators();
    QString getItemSeparator();
    QStringList getItemSeparators();
    QString getExtension(QString str);
    QString removeExtension();
    QString removeID(); // remove Youtube ID from the end
    QString removeTrackNumber();    // remove first number (if exists) and separator after it
    
    //void separatorsSplit(QStringList& wordsepl, QStringList& itemsepl);
    void setSeparators(QStringList wordsepl, QStringList itemsepl);
    
    QStringList getNumbers();
    QStringList getWords();
    QStringList getBracketStrings();
    QStringList getItems();
    QString extractFirstItem(QString itemsep = "");
    QString extractNumber();
    
    StringFilter& operator=(const QString& str);
    
    QString capitalizeWords();
    QString firstWordUppercase();
    
    QString underscoreToSpace();
    QString spaceToUnderscore();
    
    void createLists();
    void clearLists();
    
private:
    QStringList m_separators;
    QStringList m_wordSepLst;    // word separators
    QStringList m_itemSepLst;    // item separators
    bool m_manualCheck = false;  // need to be checked
    
    int matchAll(const QRegExp& rx, QStringList& lst, bool clear = false);
    QStringList getStrings(QRegExp& rx);
    
    bool isLetterChar(const QChar& c);
    bool isNumberChar(const QChar& c);
    bool isLetterOrNumberChar(QChar& c);
    bool isSeparatorChar(const QChar& c);
    
    void splitSeparators();
    
    bool containChars(QString str, QString chars);
    QString bracket_sep_replace(QString text, QString sep, QString nsep = "===");
};

#endif // STRINGFILTER_H
