/***************************************************************************
 *   bottie - irc.h                                                        *
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

#ifndef IRC_H
#define IRC_H

#include <QtCore>
#include <QtNetwork>
#include <QTextStream>

#define STATUS_WARMING_UP 0
#define STATUS_LOGGING_IN 1
#define STATUS_AUTOJOINING 2
#define STATUS_IDLE 3
#define STATUS_QUITTING 4

class Irc : public QObject
{
Q_OBJECT

public:
  Irc(QString,int,QString,QString,QString,QString);
  void goDisconnect();
  void sendData(QString);
  void sendData(QString, bool);
  void goConnect();

private:
  void parse(QString);

signals:
  void PingPong();
  void gotConnection();
  void gotDisconnection();
  void ownNickChange(QString);
  void join(QString,QString,QString);
  void chanmsg(QString,QString,QString,QString);
  void querymsg(QString,QString,QString);
  void channotice(QString,QString,QString,QString);
  void querynotice(QString,QString,QString);
  void part(QString,QString,QString,QString);
  void quit(QString,QString,QString);
  void nickChange(QString,QString,QString);
  void connError(QString);
  void topic(QString,QString);
  void topicTime(QString,QString,QString);

public slots:

private slots:
  void readData();
  void connected();
  void disconnected();
  void displayError(QAbstractSocket::SocketError);

private:
  QTcpSocket*socket;
  QString server;
  int port;
  int status;
  QString indata;
  QString chans;
  QString ident;
  QString ownNick;
  QString realname;
};

#endif // IRC_H
