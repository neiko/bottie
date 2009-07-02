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
  server = "irc.telecable.es";
  port = 6667;
  ownNick = "chan";
  chans = "#irc-hispano,#barcelona,#madrid,#mas_de_30,#mas_de_40,#mazmorra,#ajejas";
  ident = "nosoyunbot";
  realname = "Alejese, creo que esta claro que no soy un bot.";
  myNick = ownNick; // presupondré que no cambia... :p

  Irc *interface = new Irc(server,port,ownNick,chans,ident,realname);

  interface->goConnect();

  connect(interface, SIGNAL(gotConnection()), this, SLOT(gotConnection()));
  connect(interface, SIGNAL(gotDisconnection()), this, SLOT(gotDisconnection()));
  connect(interface, SIGNAL(PingPong()), this, SLOT(PingPong()));
  connect(interface, SIGNAL(ownNickChange(QString)), this, SLOT(ownNickChange(QString)));
  connect(interface, SIGNAL(join(QString,QString,QString)), this, SLOT(join(QString,QString,QString)));
  connect(interface, SIGNAL(chanmsg(QString,QString,QString,QString)), this, SLOT(chanmsg(QString,QString,QString,QString)));
  connect(interface, SIGNAL(querymsg(QString,QString,QString)), this, SLOT(querymsg(QString,QString,QString)));
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
  if (type == OUT_COLORED) {
#ifdef Q_OS_WIN
    cout << mes;
#else
    cout << "\033[3" << colour << ";2m" << mes << "\033[0m";
#endif
  } else
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
  out("Conectado a " + server + "\n");
  timestamp();
  out("Iniciando sesion...\n");
}

void Lurker::gotDisconnection(){
  timestamp();
  out("ATENCION: Desconectado de " + server + "\n", OUT_COLORED, COLOR_RED, true);
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
  out(nick);
  out(" [");
  out(mask, OUT_COLORED, COLOR_CYAN);
  out("]");
  out(" has kicked ", OUT_COLORED, COLOR_CYAN);
  out(kicked);
  out(" from ", OUT_COLORED, COLOR_CYAN);
  out(chan);
  if ( !message.isEmpty() ) {
    out(" [", OUT_COLORED, COLOR_CYAN);
    out(message);
    out("]", OUT_COLORED, COLOR_CYAN);
  }
  out("\n");
}

void Lurker::usedNick(QString oldNick, QString newNick) {
  timestamp();
  out("> El nick ", OUT_COLORED, 1, true);
  out(oldNick);
  out(" ya esta en uso. Intentando con ", OUT_COLORED, COLOR_RED, true);
  out(newNick);
  out("...\n", OUT_COLORED, COLOR_RED, true);
}
