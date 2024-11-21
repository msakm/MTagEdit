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

#ifndef PATTERNS_H
#define PATTERNS_H

template<typename T> class Singleton {
public:
    static T& instance() {
        static T theSingleInstance;  // assumes T has a protected default constructor
        return theSingleInstance;
    }
};

#endif // PATTERNS_H
