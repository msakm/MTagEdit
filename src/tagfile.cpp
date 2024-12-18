/***************************************************************************
     Copyright          : (C) 2018 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "tagfile.h"

#include "stringfilter.h"
#include "tagfilecache.h"

#include <fileref.h>
#include <taglib.h>
#include <mpegfile.h>
#include <oggfile.h>
#include <vorbisfile.h>
#include <flacfile.h>
#include <wavfile.h>
#include <opusfile.h>

#include <id3v1genres.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2framefactory.h>
#include <xiphcomment.h>
#include <apetag.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QtGlobal>
#include <QStringList>
#include <QTextCodec>

//#define QStringToTString(s) TagLib::String(s.utf8().data(), TagLib::String::UTF8)
//#define TStringToQString(s) QString::fromUtf8(s.toCString(true))
#define TStringToQString2(s) (s.isLatin1() ? QString::fromAscii(s.toCString(false)) : QString::fromUtf8(s.toCString(false)))

TagFile::TagFile(QString fname) {
    setFilename(fname);
}

TagFile::TagFile(const QFileInfo& finfo) {
    TagFile(finfo.absoluteFilePath());
}

TagFile::~TagFile() {}

void TagFile::setFilename(QString fname) {
    m_finfo.setFile(fname);
    m_ftype = TF_None;
    m_ttype = TT_None;
    m_hasOriginfo = false;
    m_changed = false;
    m_originfo.clear();
    m_tinfo.clear();
    m_ntinfo.clear();
    m_tinfo.setAbsoluteFilename(m_finfo.absoluteFilePath());
    m_ntinfo = m_originfo = m_tinfo;

}

QString TagFile::filename() const {
    return m_tinfo.absoluteFilename();
}

QString TagFile::originalFilename() const {
    return m_originfo.absoluteFilename();
}

QString TagFile::newFilename() const {
    return m_ntinfo.absoluteFilename();
}

QString TagFile::simplifiedFilename() const {
    return m_tinfo.simplifiedFilename();
}

QString TagFile::simplifiedNewFilename() const {
    return m_ntinfo.simplifiedFilename();
}

QString TagFile::simplifiedOriginalFilename() const {
    QFileInfo fi(originalFilename());
    return fi.completeBaseName();
}

void TagFile::setSimplifiedFilename(QString sfname) {
    if (sfname.isEmpty()) return;
    
    sfname = replaceInvalidChars(sfname);
    m_ntinfo.setSimplifiedFilename(sfname);
    emitChanged();
}

QString TagFile::replaceInvalidChars(const QString& fname) {
    QString outFname = fname;
    outFname.replace("*", "_");
    outFname.replace("?", "_");
    outFname.replace("\\", "_");
    outFname.replace("/", "_");
    outFname.replace(":", "_");
    outFname.replace("\r", "");
    outFname.replace("\n", "");
    return outFname;
}

QString TagFile::renameByPattern(QString rpattern) {
    QString nfilename;
    bool skipseparator = true;
    for(int pos = 0; pos < rpattern.length(); ++pos) {
        QChar c = rpattern.at(pos);
        if(c == '{') { // field start
            skipseparator = true;
            // find end of the field and replace it with info from tag
            int endpos = rpattern.indexOf('}', pos);
            // break if matching '}' can not be found
            if(endpos <= 0) break;
            QString fldname = rpattern.mid(pos+1, endpos-pos-1).toLower();
            pos = endpos;   // continue after '}'
                        
            if(fldname.compare("artist") == 0) {
                if(!m_ntinfo.artist.simplified().isEmpty()) {
                    nfilename += m_ntinfo.artist;
                    skipseparator = false;
                }
                continue;
            } else if(fldname.compare("title") == 0) {
                if(!m_ntinfo.title.simplified().isEmpty()) {
                    nfilename += m_ntinfo.title;
                    skipseparator = false;
                }
                continue;
            } else if(fldname.compare("album") == 0) {
                if(!m_ntinfo.album.simplified().isEmpty()) {
                    nfilename += m_ntinfo.album;
                    skipseparator = false;
                }
                continue;
            }
            
            // insert track number if fldname contains only digits and if the track number is present
            int numdigits = 0;
            bool trackfield = true;
            for(int i = 0; i < fldname.length(); ++i) {
                QChar c = fldname.at(i);
                if(!c.isDigit()) {
                    trackfield = false;
                    break; // not a track field
                }
                
                numdigits++;
            }
            if(trackfield && numdigits > 0) {
                if(m_ntinfo.track > 0) {
                    QString trackstr = QString::number(m_ntinfo.track);
                    trackstr = trackstr.rightJustified(numdigits, '0');
                    skipseparator = false;
                    nfilename += trackstr;
                }
                continue;
            }
            
            qDebug() << "renameByPattern: fldname not used:" << fldname;
            
        } else {
            // copy the character from pattern to filename or skip if previous field was empty
            if(!skipseparator) nfilename += c;
        }
    }
    
    // TODO: check for special characters in filename (remove/replace)
    
    qDebug() << "renameByPattern: new name =" << nfilename;
    
    if(nfilename.simplified().isEmpty()) {
        // filename should not be empty
        return simplifiedOriginalFilename();
    }
    
    setSimplifiedFilename(nfilename);
    return nfilename;
}

void TagFile::capitalizeFilename() {
    StringFilter sf = StringFilter(m_ntinfo.simplifiedFilename());
    m_ntinfo.setSimplifiedFilename(sf.capitalizeWords());
    emitChanged();
}

void TagFile::capitalizeTags() {
    StringFilter sf;
    
    sf = m_ntinfo.artist;
    m_ntinfo.artist = sf.capitalizeWords();
    
    sf = m_ntinfo.title;
    m_ntinfo.title = sf.capitalizeWords();
    
    sf = m_ntinfo.album;
    m_ntinfo.album = sf.capitalizeWords();
    
    emitChanged();
}

void TagFile::capitalizeArtist() {
    StringFilter sf;
    
    sf = m_ntinfo.artist;
    m_ntinfo.artist = sf.capitalizeWords();
    
    emitChanged();
}

void TagFile::capitalizeTitle() {
    StringFilter sf;
    
    sf = m_ntinfo.title;
    m_ntinfo.title = sf.capitalizeWords();
    
    emitChanged();
}

void TagFile::capitalizeAlbum() {
    StringFilter sf;
    
    sf = m_ntinfo.album;
    m_ntinfo.album = sf.capitalizeWords();
    
    emitChanged();
}

void TagFile::convertToUtf8() {
    qDebug() << __PRETTY_FUNCTION__;
    QTextDecoder* decoder = QTextCodec::codecForName("Windows-1250")->makeDecoder();
    QString s = decoder->toUnicode(m_tinfo.artist.toLatin1());
    //qDebug() << m_tinfo.artist << "-->" << s;
    m_ntinfo.artist = s.toUtf8();
    
    //string = codec->toUnicode(m_tinfo.artist.toLatin1().constData());
    s = decoder->toUnicode(m_tinfo.title.toLatin1());
    //qDebug() << m_tinfo.title << "-->" << s;
    m_ntinfo.title = s.toUtf8();
    s = decoder->toUnicode(m_tinfo.album.toLatin1());
    //qDebug() << m_tinfo.album << "-->" << s;
    m_ntinfo.album = s.toUtf8();
    delete decoder;
    emitChanged();
}

TagInfo TagFile::originalTagInfo() const {
    return m_originfo;
}

TagInfo TagFile::savedTagInfo() const {
    return m_tinfo;
}

TagInfo TagFile::tagInfo() const {
    return m_ntinfo;
}

QFileInfo TagFile::fileInfo() const {
    return m_finfo;
}

int TagFile::tagsUsed() {
    return m_ttype;
}

void TagFile::useTags(TagFile::TAGTYPE ttypes) {
    m_ttype = ttypes;
}

void TagFile::updateTags() {
    m_changed = true;
}

bool TagFile::hasId3v1Tag() {
    return m_ttype & TT_ID3v1;
}

bool TagFile::hasId3v2Tag() {
    return m_ttype & TT_ID3v2;
}

void TagFile::resetToOriginal() {
    m_ntinfo = m_originfo;
}

void TagFile::resetToSaved() {
    m_ntinfo = m_tinfo;
}

void TagFile::resetFilename() {
    setSimplifiedFilename(simplifiedFilename());    // reset to saved filename
}

// NOTE: tag will not be read if they are already modified
bool TagFile::readTags() {
    QString fname = m_tinfo.absoluteFilename();
    if(fname.isEmpty()) return false;
    QByteArray fnameba = fname.toUtf8();
    
    // FIXME: do not read tags if tags are already changed
    if(isChanged()) return true;
    
    QString extension = m_finfo.suffix().toLower();
    
    m_ftype = TF_None;
    bool ret = true;
    
    // MP3 files
    if(extension == "mp3") {
        m_ftype = TF_MP3;
        ret = readTagsMPEG(fname);
    } else if(extension == "ogg") {
        m_ftype = TF_Vorbis;
        ret = readTagsOggVorbis(fname);
    } else if(extension == "opus") {
        m_ftype = TF_Opus;
        ret = readTagsOggOpus(fname);
    } else if(extension == "wav") {
        m_ftype = TF_WAV;
        ret = readTagsWav(fname);
    } else if(extension == "flac" || extension == "fla" || extension == "flc") {
        m_ftype = TF_FLAC;
        ret = readTagsFLAC(fname);
    } else {
        m_ftype = TF_Other;
        
        TagLib::FileRef f(fnameba.data());
        TagLib::Tag* tag = f.tag();
        
        if (tag) {
            m_tinfo.artist = TStringToQString(tag->artist());
            m_tinfo.title = TStringToQString(tag->title());
            m_tinfo.album = TStringToQString(tag->album());
            m_tinfo.genre = TStringToQString(tag->genre());
            m_tinfo.comment = TStringToQString(tag->comment());
            m_tinfo.track = tag->track();
            m_tinfo.year = tag->year();
        }
        
        m_ttype = TT_Other;
    }
    
    
    if(!m_hasOriginfo) {
        m_originfo = m_tinfo;
        m_hasOriginfo = true;
    }
    
    if(m_ntinfo.isEmpty()) {
        m_ntinfo = m_tinfo;
    }
    
    // NOTE: disabled because if tags are being read by TagReaderThread, file list view updates everytime and it's slow
    //emitChanged();
    return ret;
}

bool TagFile::saveTags() {
    if(!isChanged()) return true;

    if (!isSupported()) {
        m_changed = false;
        return false;
    }
    
    // tags not changed (renamed only)
    if (! m_ntinfo.tagsChanged(m_tinfo) && !m_changed) return true;   
    
    bool ret = false;
    QString fname = m_finfo.absoluteFilePath();
    QString ext = m_finfo.suffix().toLower();
    qDebug() << "TagFile:saveTags: saving tags to file" << fname;
    
    if(ext == "mp3") {
        ret = saveTagsMPEG(fname);
    } else if(ext == "ogg") {
        ret = saveTagsOggVorbis(fname);
    } else if (ext == "opus") {
        ret = saveTagsOggOpus(fname);
    } else if (ext == "flac" || ext == "flc" || ext == "fla") {
        ret = saveTagsFLAC(fname);
    } else if(ext == "wav") {
        ret = saveTagsWav(fname);
    } else {
        QByteArray fnameba = fname.toUtf8();
        TagLib::FileRef f(fnameba.data());
        TagLib::Tag* tag = f.tag();
        
        if (tag) {
            tag->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
            tag->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
            tag->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
            tag->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
            tag->setYear(m_ntinfo.year);
            tag->setTrack(m_ntinfo.track);
            tag->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
            
            ret = f.save();
        }
    }
    
    if(!m_hasOriginfo) {
        m_originfo = m_tinfo;
        m_hasOriginfo = true;
    }
    
    if(ret) {
        // save tag info but keep filename (not renamed yet)
        QString absfname = m_tinfo.absoluteFilename();
        m_tinfo = m_ntinfo;
        m_tinfo.setAbsoluteFilename(absfname);
        m_changed = false;
    }
    
    emitChanged();
    return ret;
}

bool TagFile::renameFile() {
    // TODO: remove/replace characters that shoud not be in filename
    
    QFileInfo oldfile(m_tinfo.absoluteFilename());
    QFileInfo newfile(m_ntinfo.absoluteFilename());
    // do not rename when filenames are equal
    if(newfile.absoluteFilePath() == oldfile.absoluteFilePath()) return true;
    if(!oldfile.exists()) return false; // old file not exists
    
    QString nfilename = m_ntinfo.simplifiedFilename();
    if(nfilename.isEmpty()) return false;   // filename should not be empty
    
    // add or update counter if new filename already exists
    while(newfile.exists()) {
        // check number at end of filename
        QRegExp rxp("\\((\\d+)\\)$");
        if(nfilename.contains(rxp)) {
            // replace with new number
            QString strnum = rxp.cap(1);
            int num = strnum.toInt();
            strnum = QString::number(num+1);
            nfilename.replace(rxp, "(" + strnum + ")");
            
        } else {
            // add counter
            nfilename += " (1)";
        }
        
        qDebug() << "  filename exists:" << newfile.fileName() << " trying:" << nfilename;
        m_ntinfo.setSimplifiedFilename(nfilename);
        newfile.setFile(m_ntinfo.absoluteFilename());
    }
    
    QString oldname = m_tinfo.absoluteFilename();
    QString newname = m_ntinfo.absoluteFilename();
    
    qDebug() << __PRETTY_FUNCTION__ << "renaming File:" << oldname << "to" << newname;
    bool ret = QFile::rename(oldname, newname);
    
    if(ret) {
        //qDebug() << "rename OK";
        m_finfo.setFile(newname);
        m_tinfo.setAbsoluteFilename(newname);
        TagFileCacheI.emitFileRenamed(oldname, newname);
    } else {
        qDebug() << "rename failed";
    }
    
    emitChanged();
    return ret;
}

void TagFile::emitChanged() {
    TagFileCacheI.emitTagFileChanged(this);
    emit tagsUpdated();
}

QString TagFile::artist() const {
    return m_ntinfo.artist;
}

QString TagFile::title() const {
    return m_ntinfo.title;
}

QString TagFile::album() const {
    return m_ntinfo.album;
}

unsigned int TagFile::track() const {
    return m_ntinfo.track;
}

QString TagFile::genre() const {
    return m_ntinfo.genre;
}

unsigned int TagFile::year() const {
    return m_ntinfo.year;
}

QString TagFile::comment(QString description, QString lang) const {
    (void) lang;
    (void) description;
    return m_ntinfo.comment;
}

QString TagFile::BPM() const {
    return m_ntinfo.BPM;
}

void TagFile::setBPM(QString bpm) {
    m_ntinfo.BPM = bpm;
}

void TagFile::setArtist(QString text) {
    m_ntinfo.artist = text;
}

void TagFile::setTitle(QString text) {
    m_ntinfo.title = text;
}

void TagFile::setAlbum(QString text) {
    m_ntinfo.album = text;
}

void TagFile::setTrack(unsigned int num) {
    m_ntinfo.track = num;
}

void TagFile::setGenre(QString text) {
    m_ntinfo.genre = text;
}

void TagFile::setYear(unsigned int num) {
    m_ntinfo.year = num;
}

void TagFile::setComment(QString text, QString description, QString lang) {
    // TODO
    (void) description;
    (void) lang;
    m_ntinfo.comment = text;
}

bool TagFile::isChanged() const {
    //return m_changed;
    return (m_ntinfo != m_tinfo) || m_changed;
}

bool TagFile::isModified() const {
    return (m_tinfo != m_originfo);
}

bool TagFile::isSupported() const {
    QStringList extlist;
    extlist += "mp3";
    extlist += "wav";
    extlist += "ogg";
    extlist += "opus";
    extlist += "flac";
    extlist += "flc";
    extlist += "fla";
    
    QString ext = m_finfo.suffix().toLower();
    return extlist.contains(ext);
}

QString TagFile::parentDir() const {
    if (!m_finfo.isFile()) return QString();
    
    QDir d = m_finfo.dir();
    if (d.cdUp()) {
        QString dname = d.dirName();
        return dname;
    }
        
    return QString();
    
    //QString pdir = m_finfo.absolutePath();
    //pdir = pdir.section("/", -1, -1);
    //return pdir;
}

QStringList TagFile::itemSeparators() {
    return m_isepl;
}

QStringList TagFile::wordSeparators() {
    return m_wsepl;
}

void TagFile::setItemSeparators(QStringList isepl) {
    m_isepl = isepl;
}

void TagFile::setWordSeparators(QStringList wsepl) {
    m_wsepl = wsepl;
}

void TagFile::setDirAsArtist() {
    QString dname = dirName();
    if (dname.isEmpty()) return;
    
    setArtist(dname);
}

void TagFile::setDirAsAlbum() {
    if (!m_finfo.isFile()) return;
    QString dname = m_finfo.dir().dirName();
    setAlbum(dname);
}

void TagFile::setDirAsArtistAlbum() {
    if (!m_finfo.isFile()) return;
    
    StringFilter sfilter(m_finfo.dir().dirName());
    sfilter.createLists();
    QStringList items = sfilter.getItems();
    
    if (items.size() == 1) {
        setArtist(items.at(0));
        setAlbum(items.at(0));
    } else if (items.size() > 1) {
        setArtist(items.at(0));
        setAlbum(items.at(1));
    }
}

void TagFile::swapArtistTitle() {
    QString tmp = m_ntinfo.artist;
    m_ntinfo.artist = m_ntinfo.title;
    m_ntinfo.title = tmp;
}

void TagFile::swapAlbumTitle() {
    QString tmp = m_ntinfo.album;
    m_ntinfo.album = m_ntinfo.title;
    m_ntinfo.title = tmp;
}

void TagFile::swapAlbumArtist() {
    QString tmp = m_ntinfo.artist;
    m_ntinfo.artist = m_ntinfo.album;
    m_ntinfo.album = tmp;
}

QString TagFile::dirName() {
    if (!m_finfo.isFile()) return QString();
    
    QDir d = m_finfo.dir();
    QString dname = d.dirName();
    return dname;
}

QString TagFile::extractFirstFilenameItem() {
    StringFilter fname = StringFilter(simplifiedNewFilename());
    QString str = fname.extractFirstItem();
    setSimplifiedFilename(fname);
    return str;
}

void TagFile::printInfo() {
    qDebug() << "==========================";
    qDebug() << "TagFile:displayInfo:" << m_finfo.fileName();
    qDebug() << "original filename:" << m_originfo.simplifiedFilename();
    qDebug() << "current filename: " << m_tinfo.simplifiedFilename();
    qDebug() << "new filename:     " << m_ntinfo.simplifiedFilename();
    qDebug() << "--------------------------";
    QString str;
    qDebug() << "Artist:" << m_originfo.artist << m_tinfo.artist << m_ntinfo.artist;
    qDebug() << "Title: " << m_originfo.title << m_tinfo.title << m_ntinfo.title;
    qDebug() << "Album: " << m_originfo.album << m_tinfo.album << m_ntinfo.album;
    qDebug() << "==========================";
}

void TagFile::parseFilename() {
    QString fname = simplifiedNewFilename();    // use new filename
    StringFilter stringf(fname);
    
    QStringList lst_nums = stringf.getNumbers();
    QStringList lst_seps = stringf.getSeparators();
    QStringList lst_words = stringf.getWords();
    QStringList lst_bracketStr = stringf.getBracketStrings();
    
    
}

void TagFile::checkFType() {
    QString ext = m_finfo.suffix().toLower();
    if(ext == "mp3") {
        m_ftype = TF_MP3;
    } else if(ext == "ogg") {
        m_ftype = TF_Vorbis;
    } else if(ext == "wav") {
        m_ftype = TF_WAV;
    } else if(ext == "opus") {
        m_ftype = TF_Opus;
    } else if(ext == "flac" || ext == "flc" || ext == "fla") {
        m_ftype = TF_FLAC;
    } else if(m_finfo.isDir()) {
        m_ftype = TF_Dir;
    } else {
        m_ftype = TF_Other;
    }
}

bool TagFile::readTagsMPEG(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::MPEG::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::MPEG::File(fname.toLocal8Bit());  // fname.toUtf8()
    #endif
    
    if(!tfile->isValid()) {
        delete tfile;
        return false;
    }
    
    if(tfile->hasID3v1Tag()) {
        TagLib::ID3v1::Tag* tag1 = tfile->ID3v1Tag(false);
        m_tinfo.artist = TStringToQString(tag1->artist());
        m_tinfo.title = TStringToQString(tag1->title());
        m_tinfo.album = TStringToQString(tag1->album());
        m_tinfo.genre = TStringToQString(tag1->genre());
        m_tinfo.comment = TStringToQString(tag1->comment());
        m_tinfo.track = tag1->track();
        m_tinfo.year = tag1->year();
        
        m_ttype |= TT_ID3v1;
    }
    
    if(tfile->hasID3v2Tag()) {
        TagLib::ID3v2::Tag* tag2 = tfile->ID3v2Tag(false);
        if(tag2) {
            m_tinfo.artist = TStringToQString(tag2->artist());
            m_tinfo.album = TStringToQString(tag2->album());
            m_tinfo.title = TStringToQString(tag2->title());
            m_tinfo.track = tag2->track();
            m_tinfo.year = tag2->year();
            m_tinfo.genre = TStringToQString(tag2->genre());
            m_tinfo.comment = TStringToQString(tag2->comment());

            /*                
            const TagLib::ID3v2::FrameList framelist = tag2->frameList();
            
            QStringList flst;
            for(TagLib::ID3v2::FrameList::ConstIterator it = framelist.begin(); it != framelist.end(); it++) {
                const TagLib::ID3v2::Frame* frame = (*it);
                QString s = frame->frameID().data();
                flst += s;
                
            }
            //qDebug() << "  ID3v2 Frames:" << flst;
            */
            m_ttype |= TT_ID3v2;
        }
    }
    
    if(tfile->hasAPETag() && !tfile->hasID3v2Tag()) {
        TagLib::APE::Tag* tagape = tfile->APETag(false);
        if(tagape) {
            m_tinfo.artist = TStringToQString(tagape->artist());
            m_tinfo.album = TStringToQString(tagape->album());
            m_tinfo.title = TStringToQString(tagape->title());
            m_tinfo.track = tagape->track();
            m_tinfo.year = tagape->year();
            m_tinfo.genre = TStringToQString(tagape->genre());
            m_tinfo.comment = TStringToQString(tagape->comment());
        }
        
        m_ttype |= TT_APE;
    }
    
    delete tfile;
    return true;
}

bool TagFile::readTagsOggVorbis(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::Ogg::Vorbis::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::Ogg::Vorbis::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) {
        delete tfile;
        return false;
    }
    
    TagLib::Ogg::XiphComment* tag = tfile->tag();
    if (tag) {
        m_tinfo.artist = TStringToQString(tag->artist());
        m_tinfo.album = TStringToQString(tag->album());
        m_tinfo.title = TStringToQString(tag->title());
        m_tinfo.track = tag->track();
        m_tinfo.year = tag->year();
        m_tinfo.genre = TStringToQString(tag->genre());
        m_tinfo.comment = TStringToQString(tag->comment());
        m_ttype = TT_XIPH;
    }
    
    delete tfile;
    return true;
}

bool TagFile::readTagsOggOpus(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::Ogg::Opus::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::Ogg::Opus::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) {
        delete tfile;
        return false;
    }
    
    TagLib::Ogg::XiphComment* tag = tfile->tag();
    if (tag) {
        m_tinfo.artist = TStringToQString(tag->artist());
        m_tinfo.album = TStringToQString(tag->album());
        m_tinfo.title = TStringToQString(tag->title());
        m_tinfo.track = tag->track();
        m_tinfo.year = tag->year();
        m_tinfo.genre = TStringToQString(tag->genre());
        m_tinfo.comment = TStringToQString(tag->comment());
        m_ttype = TT_XIPH;
    }
    
    delete tfile;
    return true;
}

bool TagFile::readTagsFLAC(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::FLAC::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::FLAC::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) {
        delete tfile;
        return false;
    }
    
    if (tfile->hasID3v1Tag()) {
        auto tag = tfile->ID3v1Tag(false);
        if (tag) {
            m_tinfo.artist = TStringToQString(tag->artist());
            m_tinfo.album = TStringToQString(tag->album());
            m_tinfo.title = TStringToQString(tag->title());
            m_tinfo.track = tag->track();
            m_tinfo.year = tag->year();
            m_tinfo.genre = TStringToQString(tag->genre());
            m_tinfo.comment = TStringToQString(tag->comment());
        }
        
        m_ttype &= TT_ID3v1;
    }
    
    if (tfile->hasID3v2Tag()) {
        auto tag = tfile->ID3v2Tag(false);
        if (tag) {
            m_tinfo.artist = TStringToQString(tag->artist());
            m_tinfo.album = TStringToQString(tag->album());
            m_tinfo.title = TStringToQString(tag->title());
            m_tinfo.track = tag->track();
            m_tinfo.year = tag->year();
            m_tinfo.genre = TStringToQString(tag->genre());
            m_tinfo.comment = TStringToQString(tag->comment());
        }
        
        m_ttype &= TT_ID3v2;
    }
    
    if (tfile->hasXiphComment()) {
        auto tag = tfile->xiphComment(false);
        if (tag) {
            m_tinfo.artist = TStringToQString(tag->artist());
            m_tinfo.album = TStringToQString(tag->album());
            m_tinfo.title = TStringToQString(tag->title());
            m_tinfo.track = tag->track();
            m_tinfo.year = tag->year();
            m_tinfo.genre = TStringToQString(tag->genre());
            m_tinfo.comment = TStringToQString(tag->comment());
        }
        
        m_ttype &= TT_XIPH;
    }
    
    return true;
}

bool TagFile::readTagsWav(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::RIFF::WAV::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::RIFF::WAV::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) {
        delete tfile;
        return false;
    }
    
    if(tfile->hasInfoTag()) {
        TagLib::RIFF::Info::Tag* tag = tfile->InfoTag();
        m_tinfo.artist = TStringToQString(tag->artist());
        m_tinfo.title = TStringToQString(tag->title());
        m_tinfo.album = TStringToQString(tag->album());
        m_tinfo.genre = TStringToQString(tag->genre());
        m_tinfo.comment = TStringToQString(tag->comment());
        m_tinfo.track = tag->track();
        m_tinfo.year = tag->year();
        
        m_ttype |= TT_RIFF;
    }
    
    if(tfile->hasID3v2Tag()) {
        TagLib::ID3v2::Tag* tag2 = tfile->ID3v2Tag();
        if(tag2) {
            m_tinfo.artist = TStringToQString(tag2->artist());
            m_tinfo.album = TStringToQString(tag2->album());
            m_tinfo.title = TStringToQString(tag2->title());
            m_tinfo.track = tag2->track();
            m_tinfo.year = tag2->year();
            m_tinfo.genre = TStringToQString(tag2->genre());
            m_tinfo.comment = TStringToQString(tag2->comment());

            m_ttype |= TT_ID3v2;
        }
    }
    
    return false;
}

bool TagFile::saveTagsMPEG(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::MPEG::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::MPEG::File(fname.toLocal8Bit());  // fname.toUtf8() ?
    #endif
    if(!tfile->isValid()) return false;
    
    // TODO tag types: remove and create (v1, v2, ape)
    
    //if (m_ttype & TT_ID3v1)
    if(1) {
        TagLib::ID3v1::Tag* tag1 = tfile->ID3v1Tag(true);
        if(tag1) {
            tag1->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
            tag1->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
            tag1->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
            tag1->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
            tag1->setYear(m_ntinfo.year);
            tag1->setTrack(m_ntinfo.track);
            tag1->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
        }
        m_ttype |= TT_ID3v1;
    }
    
    //if (m_ttype & TT_ID3v2)
    if(1) {
        TagLib::ID3v2::Tag* tag2 = tfile->ID3v2Tag(true);
        if(tag2) {
            tag2->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
            tag2->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
            tag2->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
            tag2->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
            tag2->setYear(m_ntinfo.year);
            tag2->setTrack(m_ntinfo.track);
            tag2->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
            m_ttype |= TT_ID3v2;
        }
    }
    
    if (m_ttype & TT_APE) {
        TagLib::APE::Tag* tag = tfile->APETag(true);

        if(tag) {
            tag->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
            tag->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
            tag->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
            tag->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
            tag->setYear(m_ntinfo.year);
            tag->setTrack(m_ntinfo.track);
            tag->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
            m_ttype |= TT_APE;
        }
    }
    
    bool ret = tfile->save(TagLib::MPEG::File::ID3v1 | TagLib::MPEG::File::ID3v2);
    if(!ret) {
        qDebug() << "Tags NOT saved" << m_tinfo.absoluteFilename();
    } else {
        //qDebug() << "Tags saved";
    }
    delete tfile;
    return ret;
}

bool TagFile::saveTagsOggVorbis(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::Ogg::Vorbis::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::Ogg::Vorbis::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) return false;
    
    TagLib::Ogg::XiphComment* xiph = tfile->tag();
    xiph->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
    xiph->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
    xiph->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
    xiph->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
    xiph->setYear(m_ntinfo.year);
    xiph->setTrack(m_ntinfo.track);
    xiph->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
    m_ttype |= TT_XIPH;
    
    bool ret = tfile->save();
    delete tfile;
    return ret;
}

bool TagFile::saveTagsOggOpus(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::Ogg::Opus::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::Ogg::Opus::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) return false;
    
    TagLib::Ogg::XiphComment* xiph = tfile->tag();
    xiph->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
    xiph->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
    xiph->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
    xiph->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
    xiph->setYear(m_ntinfo.year);
    xiph->setTrack(m_ntinfo.track);
    xiph->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
    m_ttype |= TT_XIPH;
    
    bool ret = tfile->save();
    delete tfile;
    return ret;
}

bool TagFile::saveTagsFLAC(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::FLAC::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::FLAC::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) return false;
    
    TagLib::Ogg::XiphComment* xiph = tfile->xiphComment();
    xiph->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
    xiph->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
    xiph->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
    xiph->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
    xiph->setYear(m_ntinfo.year);
    xiph->setTrack(m_ntinfo.track);
    xiph->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
    m_ttype |= TT_XIPH;
    
    bool ret = tfile->save();
    delete tfile;
    return ret;
}

bool TagFile::saveTagsWav(QString fname) {
    #ifdef Q_OS_WIN
        auto tfile = new TagLib::RIFF::WAV::File((const wchar_t *) fname.utf16());
    #else
        auto tfile = new TagLib::RIFF::WAV::File(fname.toLocal8Bit());
    #endif
    if(!tfile->isValid()) return false;
    
    if (m_ttype & TT_RIFF) {
        TagLib::RIFF::Info::Tag* tag = tfile->InfoTag();
        tag->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
        tag->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
        tag->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
        tag->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
        tag->setYear(m_ntinfo.year);
        tag->setTrack(m_ntinfo.track);
        tag->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
        m_ttype |= TT_RIFF;
    }
    
    //if (m_ttype & TT_ID3v2)
    if (1) {
        TagLib::ID3v2::Tag* tag2 = tfile->ID3v2Tag(); // allways returns valid pointer
        tag2->setArtist(TagLib::String(m_ntinfo.artist.toUtf8().data(), TagLib::String::UTF8));
        tag2->setTitle(TagLib::String(m_ntinfo.title.toUtf8().data(), TagLib::String::UTF8));
        tag2->setAlbum(TagLib::String(m_ntinfo.album.toUtf8().data(), TagLib::String::UTF8));
        tag2->setGenre(TagLib::String(m_ntinfo.genre.toUtf8().data(), TagLib::String::UTF8));
        tag2->setYear(m_ntinfo.year);
        tag2->setTrack(m_ntinfo.track);
        tag2->setComment(TagLib::String(m_ntinfo.comment.toUtf8().data(), TagLib::String::UTF8));
        m_ttype |= TT_ID3v2;
    }
    
    bool ret = tfile->save();
    delete tfile;
    
    return ret;
}


TagDirInfo::TagDirInfo() {
    
}

void TagDirInfo::setPath(const QString& spath) {
    m_path = spath;
    updateInfo();
}

void TagDirInfo::updateInfo() {
    QDir d;
    d.setPath(m_path);
    // TODO: split path
}
