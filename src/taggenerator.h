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

#ifndef TAGGENERATOR_H
#define TAGGENERATOR_H

#include <QString>
#include "stringfilter.h"

class TagFile;

class TagGenerator {
public:
    TagGenerator();
    void setTagFile(TagFile* tf);
    void generateTags();
    
    void test();    // detect and print separators, generated tags, filename

    QStringList separators();
    QStringList wordSeparators();
    QStringList itemSeparators();
    
    QStringList items();
    
    
protected:
    void detectSeparators();
    
    void capitalizeFilename();
    void capitalizeTags();

private:
    TagFile* m_tagfile;
    StringFilter m_sfilter;
    
    bool isExtension(QString str);
};

#endif // TAGGENERATOR_H
