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

#include "tagreaderthread.h"

void TagReaderThread::enqueue(TagFile *ptrack) {
    if(!ptrack) return;
    m_mutex.lock();
    m_trackQueue.enqueue(ptrack);
    m_mutex.unlock();
    if(!isRunning()) start(QThread::IdlePriority);
}

void TagReaderThread::remove(TagFile *ptrack) {
    QMutexLocker locker(&m_mutex);
    m_trackQueue.removeAll(ptrack);
}

void TagReaderThread::clear() {
    QMutexLocker locker(&m_mutex);
    m_trackQueue.clear();
}

TagReaderThread::TagReaderThread() {

}

TagReaderThread::~TagReaderThread() {
    terminate();
}

int TagReaderThread::queueSize() const {
    return m_trackQueue.size();
}

void TagReaderThread::run() {
    while (1) {
        m_mutex.lock();
        TagFile* ptrack = nullptr;
        if(!m_trackQueue.isEmpty()) {
            ptrack = m_trackQueue.dequeue();
        }
        m_mutex.unlock();
        if(ptrack != nullptr) {
            emit readingFile(ptrack->originalFilename(), m_trackQueue.size());
            ptrack->readTags();
        } else {
            emit readingFile("", 0);
            break;
        }
    }
}
