#ifndef LURKER_H
#define LURKER_H

#endif // LURKER_H

#include <QtCore>
#include <QString>
#include "irc.h"

#define OUT 0
#define OUT_CLEAN 1
#define OUT_COLORED 2

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
  void channotice(QString,QString,QString,QString);
  void querynotice(QString,QString,QString);
  void part(QString,QString,QString,QString);
  void quit(QString,QString,QString);
  void nickChange(QString,QString,QString);

};
