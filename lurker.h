/***************************************************************************
 *   bottie - lurker.h                                                     *
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

#ifndef LURKER_H
#define LURKER_H

#endif // LURKER_H

#include <QtCore>
#include <QString>
#include "irc.h"

#define OUT 0
#define OUT_CLEAN 1
#define OUT_COLORED 2

#define COLOR_GRAY 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_PINK 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

class Lurker : public QObject
{
Q_OBJECT

public:
  Lurker();

private:
  Irc *interface;
  QString server;
  int port;
  int status;
  QString chans;
  QString ownNick;
  QString ident;
  QString realname;
  QString myNick;
  void out(QString);
  void out(QString,int);
  void out(QString,int,int);
  void out(QString,int,int,bool);
  void timestamp();

signals:

public slots:
  void gotConnection();
  void gotDisconnection();
  void PingPong();
  void ownNickChange(QString);
  void join(QString,QString,QString);
  void chanmsg(QString,QString,QString,QString);
  void querymsg(QString,QString,QString);
  void chanme(QString,QString,QString,QString);
  void queryme(QString,QString,QString);
  void channotice(QString,QString,QString,QString);
  void querynotice(QString,QString,QString);
  void part(QString,QString,QString,QString);
  void quit(QString,QString,QString);
  void nickChange(QString,QString,QString);
  void connError(QString);
  void topic(QString,QString);
  void topicTime(QString,QString,QString);
  void umodeChange(QString,QString,QString);
  void modeChange(QString,QString,QString,QString);
  void kick(QString,QString,QString,QString,QString);
  void usedNick(QString,QString);
};
