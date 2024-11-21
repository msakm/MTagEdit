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

#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QString>

class TrackInfo {
public:
    TrackInfo();
    
    int64_t rowid;
    
    QString m_absfilename;
    
    QString artist;
    QString title;
    QString album;
    QString genre;
    QString year;
    QString trackn;
    QString comment;
    
    QString rating;
    
    int srate;
    int bitrate;
    int channels;
    int len_sec;
    
};

#endif // TRACKINFO_H
