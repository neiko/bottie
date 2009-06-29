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

Lurker::Lurker()
{
  server = "irc.telecable.es";
  port = 6667;
  ownNick = "*";
  chans = "#irc-hispano,#barcelona,#madrid,#mas_de_30,#mas_de_40,#mazmorra";
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

}


void Lurker::out(QString mes) { // no se especificó tipo así que presupondré que será OUT y así ahorro teclado
  QTextStream cout(stdout, QIODevice::WriteOnly);
  cout << "\033[15;1m" << mes << "\033[0m";
}

void Lurker::out(QString mes, int type) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_CLEAN)
    cout << mes;
  else if (type == OUT)
    cout << "\033[15;1m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Lurker::out(QString mes, int type, int colour) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
    cout << "\033[3" << colour << ";2m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Lurker::out(QString mes, int type, int colour, bool notused) {
  // truco sucio. no hace falta ver qué devuelve notused, si hemos llegado aquí, es porque
  // queremos negrita. viva la sobrecarga de operadores :D
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
    cout << "\033[" << colour << ";1m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}


void Lurker::gotConnection(){
  out(QString("Conectado a ") + server + QString("\n"));
}

void Lurker::gotDisconnection(){
  out(QString("ATENCION: Desconectado de ") + server + "\n");
  //fatal
  exit(1);
}

void Lurker::PingPong(){
  out(QString("Ping? PONG!\n"),OUT);
}

void Lurker::join(QString nick,QString mask,QString chan) {
  timestamp();
  out(QString("---> "),OUT_COLORED, COLOR_GREEN, true);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
    out(QString("I have"),OUT_COLORED, COLOR_GREEN);
  else {
    out(nick, OUT_COLORED, COLOR_GREEN);
    out(QString(" ["));
    out(mask,OUT_COLORED, COLOR_GREEN);
    out(QString("]"));
  }
    out(QString(" joined "),OUT_COLORED, COLOR_GREEN);
    out(chan, OUT_COLORED, COLOR_GREEN);
    out(QString("\n"));
}

void Lurker::chanmsg(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("<"),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString("@"),OUT_COLORED, COLOR_CYAN, true);
  out(chan,OUT_COLORED, COLOR_WHITE);
  out(QString("> "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}
void Lurker::querymsg(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("<"),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString("> "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}
void Lurker::chanme(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString(" * "),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString("@"),OUT_COLORED, COLOR_CYAN, true);
  out(chan,OUT_COLORED, COLOR_WHITE);
  out(QString(" "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}
void Lurker::queryme(QString nick,QString mask,QString message) {
  timestamp();
  out(QString(" * "),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString(" "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}
void Lurker::channotice(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("/"),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString("@"),OUT_COLORED, COLOR_CYAN, true);
  out(chan,OUT_COLORED, COLOR_WHITE);
  out(QString("/ "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}
void Lurker::querynotice(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("/"),OUT_COLORED, COLOR_CYAN, true);
  out(nick,OUT_COLORED, COLOR_WHITE);
  out(QString("/ "),OUT_COLORED, COLOR_CYAN, true);
  out(message,OUT_COLORED, COLOR_WHITE);
  out(QString("\n"),OUT);
}

void Lurker::part(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("<--- "),OUT_COLORED, COLOR_RED, true);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // part propio o ajeno???
    out(QString("I've"),OUT_COLORED, COLOR_CYAN);
  else {
    out(nick, OUT_COLORED, COLOR_CYAN);
    out(QString(" ["));
    out(mask,OUT_COLORED, COLOR_CYAN);
    out(QString("]"));
  }
  out(QString(" parted "),OUT_COLORED, COLOR_CYAN);
  out(chan, OUT_COLORED, COLOR_CYAN);
  if (QString::compare(message, "")) {
    out(QString(" ["));
    out(message, OUT_COLORED, COLOR_CYAN);
    out(QString("]"));
  }
  out(QString("\n"));
}

void Lurker::quit(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("<--- "),OUT_COLORED, COLOR_RED, true);
  out(nick, OUT_COLORED, COLOR_CYAN);
  out(QString(" ["));
  out(mask,OUT_COLORED, COLOR_CYAN);
  out(QString("]"));
  out(QString(" has quit "),OUT_COLORED, COLOR_CYAN);
  if (QString::compare(message, "")) {
    out(QString("["));
    out(message, OUT_COLORED, COLOR_CYAN);
    out(QString("]\n"));
  }
}

void Lurker::ownNickChange(QString newnick){
  myNick = newnick;
}

void Lurker::timestamp(){
  QTime time = QTime::currentTime();
  QString timeString = time.toString();
  out(QString("["));
  out(timeString, OUT_COLORED, COLOR_WHITE);
  out(QString("] "));
}

void Lurker::nickChange(QString nick,QString mask,QString newnick) {
  timestamp();
  out(QString("  -> "),OUT_COLORED, COLOR_YELLOW);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
    out(QString("I have"),OUT_COLORED, COLOR_YELLOW);
  else {
    out(nick, OUT_COLORED, COLOR_YELLOW);
    out(QString(" ["));
    out(mask,OUT_COLORED, COLOR_YELLOW);
    out(QString("]"));
  }
    out(QString(" changed nick to "),OUT_COLORED, COLOR_YELLOW);
    out(newnick, OUT_COLORED, COLOR_YELLOW);
    out(QString("\n"));
}

void Lurker::connError(QString errdesc) {
  timestamp();
  out(errdesc + "\n", OUT_COLORED, 31, true);
  // Sad but true, a connection error is FATAL for us.
  exit(1);
}

void Lurker::topic(QString chan,QString topic_) {
  timestamp();
  out(QString("  -> Topic for "),OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(QString(": "),OUT_COLORED, COLOR_CYAN);
  out(topic_);
  out(QString("\n"));
}

void Lurker::topicTime(QString chan,QString nick,QString tstamp) {
  timestamp();
  out(QString("  -> Topic for "),OUT_COLORED, COLOR_CYAN);
  out(chan);
  out(QString(" was set by "),OUT_COLORED, COLOR_CYAN);
  out(nick);
  out(QString(" on "),OUT_COLORED, COLOR_CYAN);
  out(tstamp);
  out(QString("\n"));
}
