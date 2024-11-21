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

#ifndef TAGWRITERTHREAD_H
#define TAGWRITERTHREAD_H

#include "singleton.h"
#include "tagfile.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#define TagWriterThreadInst TagWriterThread::instance()

class TagWriterThread : public QThread, public Singleton<TagWriterThread> {
    Q_OBJECT
    friend class Singleton<TagWriterThread>;
public:
    ~TagWriterThread();
    
    int queueSize() const;
    void enqueue(TagFile* ptrack);
    void remove(TagFile* ptrack);
    void clear();
    
protected:
    TagWriterThread();
    
    void run();
    
signals:
    void writingFile(QString fname, int num);
    
private:
    QMutex m_mutex;
    QWaitCondition m_wait;
    QQueue<TagFile*> m_trackQueue;
};

#endif // TAGWRITERTHREAD_H
