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

#include "settings.h"

#include <QStringList>
#include <QApplication>
#include <QDebug>

Settings::Settings() : 
#ifdef _WIN32
    QSettings(QApplication::applicationDirPath() + "/" + "settings.ini", QSettings::IniFormat)
#else
    QSettings(QSettings::IniFormat, QSettings::UserScope, "TagEditor", "settings")
#endif
{
    
}

Settings::~Settings() {}

QString Settings::getString(QString key, QString defaultValue) {
    return value(key, QVariant(defaultValue)).toString();
}

int Settings::getInt(QString key, int defaultValue) {
    return value(key, defaultValue).toInt();
}

QString Settings::recentDir(QString defaultDir) {
    return value("LastUsedDir", QVariant(defaultDir)).toString();
}

void Settings::setRecentDir(QString dir) {
    settings.setValue("LastUsedDir", dir);
}

QStringList Settings::getArray(QString grp, QString key) {
    QStringList lst;
    int size = beginReadArray(grp);
    for(int i = 0; i < size; ++i) {
        setArrayIndex(i);
        lst += value(key).toString();
    }
    endArray();
    return lst;
}

void Settings::setArray(QString grp, QString key, const QStringList& array) {
    beginWriteArray(grp);
    for(int i = 0; i < array.size(); ++i) {
        setArrayIndex(i);
        setValue(key, array.at(i));
    }
    endArray();
}
