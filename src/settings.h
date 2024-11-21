/***************************************************************************
     Copyright          : (C) 2008 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "singleton.h"

#define settings Settings::instance()

class Settings : public QSettings, public Singleton<Settings> {
    Q_OBJECT
    friend class Singleton<Settings>;
public:
    ~Settings();
    QString getString(QString key, QString defaultValue = QString());
    int getInt(QString key, int defaultValue = 0);
    
    QStringList getArray(QString grp, QString key);
    void setArray(QString grp, QString key, const QStringList& array);

    QString recentDir(QString defaultDir = ".");
    void setRecentDir(QString dir);

protected:
    Settings();

};

#endif
