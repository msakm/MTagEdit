/***************************************************************************
     Copyright          : (C) 2008 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TAGFILE_H
#define TAGFILE_H

#include "taginfo.h"

#include <QFileInfo>
#include <QObject>
#include <QStringList>

class TagDirInfo {
public:
    TagDirInfo();
    
    void setPath(const QString& spath);
    QString path() const;
    QString dirName() const;
    QString parentDirName() const;
    
    QStringList dirItems();
    QStringList parentDirItems();
    
private:
    QString m_path;
    QString m_dir;          // simple directory name
    QString m_parent_dir;   // simple parent directory name
    
    void updateInfo();
};

class TagFile : public QObject {
    Q_OBJECT
public:
    explicit TagFile(QString filename = "");
    explicit TagFile(const QFileInfo& finfo);
    ~TagFile();

    enum TFILETYPE {
        TF_None     = 0,
        TF_Dir      = 1,
        TF_WAV      = 2,
        TF_MP3      = 3,
        TF_Vorbis   = 4,
        TF_Opus     = 5,
        TF_FLAC     = 6,
        
        TF_Other    = 7
    };
    
    enum TAGTYPE {
        TT_None  = 0,
        TT_ID3v1 = 1,
        TT_ID3v2 = 2,
        TT_APE   = 4,
        TT_MP4   = 8,
        TT_XIPH  = 16,
        TT_RIFF  = 32,    // wav
        TT_Other = 64
    };
    
    void setFilename(QString filename); // set filename and reset everything without saving
    
    QString filename() const;           // current filename
    QString originalFilename() const;   // old original filename
    QString newFilename() const;        // new filename (rename to)
    
    QString simplifiedFilename() const; // current filename without path and extension
    QString simplifiedNewFilename() const;  // new filename without path and extension
    QString simplifiedOriginalFilename() const; // old filename without path and extension
    
    void setSimplifiedFilename(QString sfname); // set new filename (without path and extension)
    QString replaceInvalidChars(const QString& fname);
    
    QString renameByPattern(QString rpattern);
    void capitalizeFilename();
    void capitalizeTags();
    void capitalizeArtist();
    void capitalizeTitle();
    void capitalizeAlbum();
    void convertToUtf8();
    
    TagInfo originalTagInfo() const;
    TagInfo savedTagInfo() const;
    TagInfo tagInfo() const;
    QFileInfo fileInfo() const;
    
    int tagsUsed();
    void useTags(TAGTYPE ttypes);
    void updateTags();  // force save, to update id3v2 from v1
    bool hasId3v1Tag();
    bool hasId3v2Tag();
    
    void resetToOriginal();
    void resetToSaved();
    void resetFilename();
    
    bool readTags();
    bool saveTags();
    bool renameFile();  // rename file on disk to new filename
    
    void emitChanged();
    
    // ntinfo
    QString artist() const;
    QString title() const;
    QString album() const;
    unsigned int track() const;
    QString genre() const;
    unsigned int year() const;
    QString comment(QString description = "", QString lang = "") const;
    QString BPM() const;
    
    void setBPM(QString bpm);
    void setArtist(QString text);
    void setTitle(QString text);
    void setAlbum(QString text);
    void setTrack(unsigned int num);
    void setGenre(QString text);
    void setYear(unsigned int num);
    void setComment(QString text, QString description = "", QString lang = "");

    bool isChanged() const;
    bool isModified() const;
    bool isSupported() const;
    
    QString parentDir() const;
    QStringList itemSeparators();
    QStringList wordSeparators();
    void setItemSeparators(QStringList isepl);
    void setWordSeparators(QStringList wsepl);   
    
    void setDirAsArtist();      // set artist to directory name
    void setDirAsAlbum();       // set album to directory name
    void setDirAsArtistAlbum(); // set Artist - Album from directory name
    
    void swapArtistTitle();
    void swapAlbumTitle();
    void swapAlbumArtist();
    
    QString dirName();
    QString extractFirstFilenameItem();
    
    void printInfo();
    
protected:
    
signals:
    void tagsUpdated();
    
private:
    QFileInfo m_finfo;  // file info
    
    bool m_hasOriginfo;
    TagInfo m_originfo; // original tag info
    TagInfo m_tinfo;    // saved tag info
    TagInfo m_ntinfo;   // new tag info
    
    bool m_changed;     // tags changed and not saved
    TFILETYPE m_ftype;
    int m_ttype;        // tag type
    
    QStringList m_isepl;    // item separator list
    QStringList m_wsepl;    // word separator list
    
    void parseFilename();
    void checkFType();
    
    bool readTagsMPEG(QString fname);
    bool readTagsOggVorbis(QString fname);
    bool readTagsOggOpus(QString fname);
    bool readTagsFLAC(QString fname);
    bool readTagsWav(QString fname);
    
    bool saveTagsMPEG(QString fname);
    bool saveTagsOggVorbis(QString fname);
    bool saveTagsOggOpus(QString fname);
    bool saveTagsFLAC(QString fname);
    bool saveTagsWav(QString fname);
};

#endif
