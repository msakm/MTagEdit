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

#ifndef TAGREADERTHREAD_H
#define TAGREADERTHREAD_H

#include "singleton.h"
#include "tagfile.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#define TagReaderThreadInst TagReaderThread::instance()

class TagReaderThread : public QThread, public Singleton<TagReaderThread> {
    Q_OBJECT
    friend class Singleton<TagReaderThread>;
public:
    ~TagReaderThread();
    
    int queueSize() const;
    void enqueue(TagFile* ptrack);
    void remove(TagFile* ptrack);
    void clear();
    
protected:
    TagReaderThread();
    
    void run();

signals:
    void readingFile(QString fnmae, int num);
    
private:
    QMutex m_mutex;
    QQueue<TagFile*> m_trackQueue;
};

#endif // TAGREADERTHREAD_H
