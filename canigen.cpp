/***************************************************************************
 *   bottie - canigen.cpp                                                  *
 *   (C) 2009 egns                                                         *
 ***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "canigen.h"
#include <QSettings>

Canigen::Canigen()
{
  conf = new QSettings("canigen.ini", QSettings::IniFormat, this);

  if (!conf) {
    qDebug() << "Could not open/find canigen.ini. Exiting." << endl;
    exit(1);
  }
}

// false male, true female
QString Canigen::genNew(bool genre) {
  srand(time(NULL));
  if ( genre == false ) {
    One = conf->value("male/one").toString().split(",");
    Two = conf->value("male/two").toString().split(",");
    Three = conf->value("male/three").toString().split(",");
  } else {
    One = conf->value("female/one").toString().split(",");
    Two = conf->value("female/two").toString().split(",");
    Three = conf->value("female/three").toString().split(",");
  }
  return One[rand() % (One.length())] + "_"
      + Two[rand() % (Two.length())] + "_"
      + Three[rand() % (Three.length())];

}
