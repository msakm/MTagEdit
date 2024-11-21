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

#include "tagwriterthread.h"

void TagWriterThread::enqueue(TagFile* ptrack) {
    if(!ptrack) return;
    m_mutex.lock();
    m_trackQueue.enqueue(ptrack);
    m_mutex.unlock();
    m_wait.wakeAll();
    if(!isRunning()) start(QThread::IdlePriority);
}

void TagWriterThread::remove(TagFile* ptrack) {
    m_trackQueue.removeAll(ptrack);
}

void TagWriterThread::clear() {
    m_trackQueue.clear();
}

TagWriterThread::TagWriterThread() {

}

TagWriterThread::~TagWriterThread() {
    terminate();  
}

int TagWriterThread::queueSize() const {
    return m_trackQueue.size();
}

void TagWriterThread::run() {
    while (1) {
        m_mutex.lock();
        TagFile* ptrack = nullptr;
        if(!m_trackQueue.isEmpty()) {
            ptrack = m_trackQueue.dequeue();
        }
        m_mutex.unlock();
        if(ptrack != nullptr) {
            emit writingFile(ptrack->filename(), m_trackQueue.size());
            ptrack->saveTags();
            ptrack->renameFile();
        } else {
            emit writingFile("", 0);
            break;
        }
    }
}
