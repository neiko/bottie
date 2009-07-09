/***************************************************************************
 *   bottie - lurker.cpp                                                   *
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

#include "lurker.h"
#include <QDateTime>

Lurker::Lurker()
{
  conf = new QSettings("config.ini", QSettings::IniFormat, this);

  if (!conf) {
    qDebug() << "Could not open/find config.ini. Exiting." << endl;
    exit(1);
  }

  server = conf->value("server/server").toString();
  port = conf->value("server/port").toInt();
  passwd = conf->value("server/password").toString(); // this passwd is for the SERVER
  chans = conf->value("server/autojoin").toString();

  ownNick = conf->value("identity/nick").toString();
  ident = conf->value("identity/ident").toString();
  realname = conf->value("identity/realname").toString();
  myNick = ownNick; // presupondré que no cambia... :p

  Irc *interface = new Irc(server,port,ownNick,chans,ident,realname,passwd);

  interface->goConnect();

  connect(interface, SIGNAL(gotConnection()), this, SLOT(gotConnection()));
  connect(interface, SIGNAL(gotDisconnection()), this, SLOT(gotDisconnection()));
  connect(interface, SIGNAL(PingPong()), this, SLOT(PingPong()));
  connect(interface, SIGNAL(ownNickChange(QString)), this, SLOT(ownNickChange(QString)));
  connect(interface, SIGNAL(join(QString,QString,QString)), this, SLOT(join(QString,QString,QString)));
  connect(interface, SIGNAL(chanmsg(QString,QString,QString,QString)), this, SLOT(chanmsg(QString,QString,QString,QString)));
  connect(interface, SIGNAL(querymsg(QString,QString,QString)), this, SLOT(querymsg(QString,QString,QString)));
  connect(interface, SIGNAL(chanctcp(QString,QString,QString,QString)), this, SLOT(chanctcp(QString,QString,QString,QString)));
  connect(interface, SIGNAL(queryctcp(QString,QString,QString)), this, SLOT(queryctcp(QString,QString,QString)));
  connect(interface, SIGNAL(chanme(QString,QString,QString,QString)), this, SLOT(chanme(QString,QString,QString,QString)));
  connect(interface, SIGNAL(queryme(QString,QString,QString)), this, SLOT(queryme(QString,QString,QString)));
  connect(interface, SIGNAL(channotice(QString,QString,QString,QString)), this, SLOT(channotice(QString,QString,QString,QString)));
  connect(interface, SIGNAL(querynotice(QString,QString,QString)), this, SLOT(querynotice(QString,QString,QString)));
  connect(interface, SIGNAL(part(QString,QString,QString,QString)), this, SLOT(part(QString,QString,QString,QString)));
  connect(interface, SIGNAL(quit(QString,QString,QString)), this, SLOT(quit(QString,QString,QString)));
  connect(interface, SIGNAL(nickChange(QString,QString,QString)), this, SLOT(nickChange(QString,QString,QString)));
  connect(interface, SIGNAL(connError(QString)), this, SLOT(connError(QString)));
  connect(interface, SIGNAL(topic(QString,QString)), this, SLOT(topic(QString,QString)));
  connect(interface, SIGNAL(topicTime(QString,QString,QString)), this, SLOT(topicTime(QString,QString,QString)));
  connect(interface, SIGNAL(umodeChange(QString,QString,QString)), this, SLOT(umodeChange(QString,QString,QString)));
  connect(interface, SIGNAL(modeChange(QString,QString,QString,QString)), this, SLOT(modeChange(QString,QString,QString,QString)));
  connect(interface, SIGNAL(kick(QString,QString,QString,QString,QString)), this, SLOT(kick(QString,QString,QString,QString,QString)));
  connect(interface, SIGNAL(usedNick(QString,QString)), this, SLOT(usedNick(QString,QString)));


  connect(this, SIGNAL(sendData(QString)), interface, SLOT(sendData(QString)));
  connect(this, SIGNAL(sendData(QString,bool)), interface, SLOT(sendData(QString,bool)));
}


void Lurker::out(QString mes) { // no se especificó tipo así que presupondré que será OUT y así ahorro teclado
  QTextStream cout(stdout, QIODevice::WriteOnly);
#ifdef Q_OS_WIN
  cout << mes;
#else
  cout << "\033[15;1m" << mes << "\033[0m";
#endif
}

void Lurker::out(QString mes, int type) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_CLEAN)
    cout << mes;
  else if (type == OUT)
#ifdef Q_OS_WIN
    cout << mes;
#else
    cout << "\033[15;1m" << mes << "\033[0m";
#endif
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Lurker::out(QString mes, int type, int colour) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
#ifdef Q_OS_WIN
    cout << mes;
#else
    cout << "\033[3" << colour << ";2m" << mes << "\033[0m";
#endif
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Lurker::out(QString mes, int type, int colour, bool notused) {
  // truco sucio. no hace falta ver qué devuelve notused, si hemos llegado aquí, es porque
  // queremos negrita. viva la sobrecarga de operadores :D
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
#ifdef Q_OS_WIN
    cout << mes;
#else
    cout << "\033[3" << colour << ";1m" << mes << "\033[0m";
#endif
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Lurker::gotConnection(){
  timestamp();
  out("Connected to " + server + "\n");
  timestamp();
  out("Signing in...\n");
}

void Lurker::gotDisconnection(){
  timestamp();
  out("WARNING: Disconnected from " + server + "\n", OUT_COLORED, COLOR_RED, true);
  //fatal
  exit(1);
}

void Lurker::PingPong(){
  timestamp();
  out("Ping? PONG!\n",OUT);
}

void Lurker::join(QString nick,QString mask,QString chan) {
  timestamp();
  out("---> ",OUT_COLORED, COLOR_GREEN);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
    out("I have",OUT_COLORED, COLOR_GREEN);
  else {
    out(nick);
    out(" [");
    out(mask,OUT_COLORED, COLOR_GREEN);
    out("]");
  }
    out(" joined ",OUT_COLORED, COLOR_GREEN);
    out(chan);
    out("\n");
}

void Lurker::chanmsg(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out("<",OUT_COLORED, COLOR_CYAN, true);
  out(nick, OUT_COLORED, COLOR_WHITE, true);
  out("@",OUT_COLORED, COLOR_CYAN, true);
  out(chan,OUT_COLORED, COLOR_WHITE, true);
  out("> ",OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out("\n",OUT);
}

void Lurker::querymsg(QString nick,QString mask,QString message) {
  timestamp();
  out("<",OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_CYAN, true);
  out("> ",OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out("\n",OUT);
}

void Lurker::chanme(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(" * ",OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out("@",OUT_COLORED, COLOR_CYAN, true);
  out(chan,OUT_COLORED, COLOR_WHITE);
  out(" ",OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out("\n",OUT);
}

void Lurker::queryme(QString nick,QString mask,QString message) {
  timestamp();
  out(" * ",OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_CYAN, true);
  out(" ",OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_CYAN, true);
  out("\n",OUT);
}

void Lurker::channotice(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out("/", OUT_COLORED, COLOR_CYAN, true);
  out(nick, OUT_COLORED, COLOR_WHITE);
  out("@", OUT_COLORED, COLOR_CYAN, true);
  out(chan, OUT_COLORED, COLOR_WHITE);
  out("/ ", OUT_COLORED, COLOR_CYAN, true);
  out(message, OUT_COLORED, COLOR_WHITE);
  out("\n", OUT);
}

void Lurker::querynotice(QString nick,QString mask,QString message) {
  timestamp();
  out("/", OUT_COLORED, COLOR_CYAN, true);
  out(nick, OUT_COLORED, COLOR_CYAN, true);
  out("/ ", OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out("\n", OUT);
}

void Lurker::part(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out("<--- ",OUT_COLORED, COLOR_CYAN);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // part propio o ajeno???
    out("I've", OUT_COLORED, COLOR_CYAN);
  else {
    out(nick);
    out(" [");
    out(mask,OUT_COLORED, COLOR_CYAN);
    out("]");
  }
  out(" parted ", OUT_COLORED, COLOR_CYAN);
  out(chan);
  if (!message.isEmpty()) {
    out(" [");
    out(message, OUT_COLORED, COLOR_CYAN);
    out("]");
  }
  out("\n");
}

void Lurker::quit(QString nick,QString mask,QString message) {
  timestamp();
  out("<--- ",OUT_COLORED, COLOR_CYAN);
  out(nick);
  out(" [");
  out(mask,OUT_COLORED, COLOR_CYAN);
  out("]");
  out(" has quit ",OUT_COLORED, COLOR_CYAN);
  if (!message.isEmpty()) {
    out("[");
    out(message, OUT_COLORED, COLOR_CYAN);
    out("]");
  }
  out("\n");
}

void Lurker::ownNickChange(QString newnick){
  myNick = newnick;
}

void Lurker::timestamp(){
  QTime time = QTime::currentTime();
  QString timeString = time.toString();
  out("[");
  out(timeString, OUT_COLORED, COLOR_WHITE);
  out("] ");
}

void Lurker::nickChange(QString nick,QString mask,QString newnick) {
  timestamp();
  out("  -> ", OUT_COLORED, COLOR_YELLOW);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
    out("I have", OUT_COLORED, COLOR_YELLOW);
  else {
    out(nick);
    out(" [");
    out(mask,OUT_COLORED, COLOR_YELLOW);
    out("]");
  }
    out(" changed nick to ",OUT_COLORED, COLOR_YELLOW);
    out(newnick);
    out("\n");
}

void Lurker::connError(QString errdesc) {
  timestamp();
  out(errdesc + "\n", OUT_COLORED, COLOR_RED, true);
  // Sad but true, a connection error is FATAL for us.
  exit(1);
}

void Lurker::topic(QString chan,QString topic_) {
  timestamp();
  out("  -> Topic for ",OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(": ", OUT_COLORED, COLOR_CYAN);
  out(topic_);
  out("\n");
}

void Lurker::topicTime(QString chan,QString nick,QString tstamp) {
  timestamp();
  QDateTime realtimestamp = realtimestamp.fromTime_t(tstamp.toInt());
  out("  -> Topic for ", OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(" was set by ", OUT_COLORED, COLOR_CYAN);
  out(nick);
  out(" on ", OUT_COLORED, COLOR_CYAN);
  out(realtimestamp.toString());
  out("\n");
}

void Lurker::umodeChange(QString setter,QString setted,QString mode) {
  timestamp();
  out("  -> ", OUT_COLORED, COLOR_CYAN);
  out(setter);
  out(" sets mode ", OUT_COLORED, COLOR_CYAN);
  out(setted);
  out(" to ", OUT_COLORED, COLOR_CYAN);
  out(mode);
  out("\n");
}

void Lurker::modeChange(QString setter,QString mask, QString chan, QString mode) {
  timestamp();
  out("  -> ", OUT_COLORED, COLOR_CYAN);
  out(setter);
  out(" has set channel mode ", OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(" to ", OUT_COLORED, COLOR_CYAN);
  out(mode);
  out("\n");
}

void Lurker::kick(QString nick, QString mask, QString chan, QString kicked, QString message) {
  timestamp();
  out("<--- ", OUT_COLORED, COLOR_RED, true);
  out(kicked);
  out(" was kicked from ", OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(" by ", OUT_COLORED, COLOR_CYAN);
  out(nick);
  out(" [");
  out(mask, OUT_COLORED, COLOR_CYAN);
  out("]");
  if ( !message.isEmpty() ) {
    out(" [");
    out(message, OUT_COLORED, COLOR_CYAN);
    out("]");
  }
  out("\n");
}

void Lurker::usedNick(QString oldNick, QString newNick) {
  timestamp();
  out("> Nick ", OUT_COLORED, 1, true);
  out(oldNick);
  out(" is being used. Trying ", OUT_COLORED, COLOR_RED, true);
  out(newNick);
  out("...\n", OUT_COLORED, COLOR_RED, true);
}

void Lurker::chanctcp(QString nick,QString mask,QString chan,QString ctcp) {
  timestamp();
  out(" > CTCP ",OUT_COLORED, COLOR_CYAN, true);
  out(ctcp, OUT_COLORED, COLOR_WHITE, true);
  out(" requested from ", OUT_COLORED, COLOR_CYAN, true);
  out(nick, OUT_COLORED, COLOR_WHITE, true);
  out(" [");
  out(mask, OUT_COLORED, COLOR_WHITE, true);
  out("]");
  out(" to ", OUT_COLORED, COLOR_CYAN, true);
  out(chan, OUT_COLORED, COLOR_WHITE, true);
  out("\n");
}

void Lurker::queryctcp(QString nick,QString mask,QString ctcp) {
  timestamp();
  out(" > CTCP ",OUT_COLORED, COLOR_CYAN, true);
  out(ctcp, OUT_COLORED, COLOR_WHITE, true);
  out(" requested from ", OUT_COLORED, COLOR_CYAN, true);
  out(nick, OUT_COLORED, COLOR_WHITE, true);
  out(" [");
  out(mask, OUT_COLORED, COLOR_WHITE, true);
  out("]\n");

/* Con esto activado, el bot responde a CTCPs. La razón por
     la que está desactivado es simple: no hay control de saturación,
     por lo que entre 2 o más personas, pueden tirar el bot.

  if (ctcp.startsWith("VERSION"))
    emit sendData("NOTICE " + nick + " :\001VERSION Bottie");

  if (ctcp.startsWith("PING"))
    emit sendData("NOTICE " + nick + " :\001PING"); */

}
