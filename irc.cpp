/***************************************************************************
 *   bottie - irc.cpp                                                      *
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

#include "irc.h"
#include <stdio.h>

Irc::Irc()
{
  // TODO: handle this with QConfig
  server = "irc.telecable.es";
  port = 6667;
  ownNick = "*";
  chans = "#irc-hispano,#barcelona,#madrid,#mas_de_30,#mas_de_40";
  ident = "ident";
  realname = "Nombre real :-)";

  status = STATUS_WARMING_UP;

  socket = new QTcpSocket( this );
  connect( socket, SIGNAL( readyRead() ), this, SLOT( readData() ) );
  connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
  connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
  connect( socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

  socket->connectToHost( server, port );
  status = STATUS_LOGGING_IN;
}

void Irc::out(QString mes, int type) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_CLEAN)
    cout << mes;
  else if (type == OUT)
    cout << "\033[15;1m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Irc::out(QString mes) { // no se especificó tipo así que presupondré que será OUT y así ahorro teclado
  QTextStream cout(stdout, QIODevice::WriteOnly);
  cout << "\033[15;1m" << mes << "\033[0m";
}

void Irc::out(QString mes, int type, int colour) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
    cout << "\033[" << colour << ";2m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}

void Irc::readData() {
  indata += socket->readAll();
  //out(indata,OUT_CLEAN); // with this enabled, bottie will output EVERY raw it gets to console

  bool taking = false;
  if(indata.right (2)!= "\r\n")
    taking = true;
  QStringList list = indata.split( "\r\n", QString::SkipEmptyParts );
  if( taking )
  {indata = list.back();
   list.removeLast();}

  foreach(QString pendingString,list)
    parse(QString::fromUtf8(qPrintable(pendingString)));

  indata.clear();
}

void Irc::parse(QString raw) {
  if (raw.startsWith(':'))
    raw = raw.right(raw.length() - 1);
  QStringList matrix = raw.split(' ');

  if( matrix[0] == "PING" ) { // Recibido ping, pongoneamos.
    indata[1] = 'O'; // xD
    sendData(indata);
    out(QString("Ping? PONG!\n"),OUT);

  } else if ( matrix[1] == "NOTICE" && matrix[2] == "IP_LOOKUP" && status == STATUS_LOGGING_IN) {
    //Inicio de sesión
    out(QString("Iniciando sesion...\n"),OUT);

    // Menuda cerdada esto de aquí abajo, ¿no?
    QString tempraw = QString("NICK ");
    tempraw.append(ownNick);
    tempraw.append("\nUSER ");
    tempraw.append(ident);
    tempraw.append(" ");
    tempraw.append(ownNick);
    tempraw.append(" ");
    tempraw.append(server);
    tempraw.append(" :");
    tempraw.append(realname);
    sendData(tempraw);

  } else if ( matrix[1] == "PRIVMSG" || matrix[1] == "NOTICE" ) { // query o chanmsg O NOTICE que a fin de
                                                                  // cuentas es lo mismo, ya veré cuando lo
    QString nick = matrix[0].left(matrix[0].indexOf('!'));        // separo, aun no tengo ganas
    QString message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    if ( matrix[2].startsWith("#") ) { // chanmsg

      out(QString("<"),OUT_COLORED,36);
      out(nick,OUT_COLORED,37);
      out(QString("@"),OUT_COLORED,36);
      out(matrix[2],OUT_COLORED,37);
      out(QString("> "),OUT_COLORED,36);
      out(message,OUT_COLORED,37);
      out(QString("\n"),OUT);

    } else { //privmsg

      out(QString("<"),OUT_COLORED,36);
      out(nick,OUT_COLORED,37);
      out(QString("> "),OUT_COLORED,36);
      out(message,OUT_COLORED,37);
      out(QString("\n"),OUT);

    }

  } else if ( matrix[1] == "JOIN" ) { //join a un canal
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" JOIN") - nick.length()));
    QString chan = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    out(QString("---> "),OUT_COLORED,36);
    if (!QString::compare(nick, ownNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
      out(QString("I have"),OUT_COLORED,36);
    else {
      out(nick, OUT_COLORED, 36);
      out(QString(" ["));
      out(mask,OUT_COLORED,36);
      out(QString("]"));
    }
    out(QString(" joined "),OUT_COLORED,36);
    out(chan, OUT_COLORED, 36);
    out(QString("\n"));

  } else if ( matrix[1] == "PART" || matrix[1] == "QUIT" ) { // part o quit
    // los pongo juntos pq se parecen un poco
    QString message = "", mask = "", chan = "";
    if (raw.indexOf(" :") != -1)
      message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PART") - nick.length()));
    out(QString("<--- "),OUT_COLORED,36);
    if (!QString::compare(nick, ownNick, Qt::CaseInsensitive)) // part propio o ajeno???
      out(QString("I've"),OUT_COLORED,36);
    else {
      out(nick, OUT_COLORED, 36);
      out(QString(" ["));
      out(mask,OUT_COLORED,36);
      out(QString("]"));
    }
    if ( matrix[1] == "PART" ) {

      // NICK Y MASK YA DICHOS, COJONES YA!
      QString chan = raw.right((raw.length() - 1) - raw.indexOf(" #"));
      chan = chan.left(chan.indexOf(" :"));
      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PART") - nick.length()));
      out(QString(" parted "),OUT_COLORED,36);
      out(chan, OUT_COLORED, 36);

    } else if ( matrix[1] == "QUIT" ) {

      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" QUIT") - nick.length()));
      out(QString(" quitted"),OUT_COLORED,36);

    }
    out(QString(" ["));
    out(message, OUT_COLORED, 36);
    out(QString("]\n"));
  }
  bool isInt;
  int code_msg = matrix[1].toInt( &isInt );
  if( isInt ) {
    switch ( code_msg ) {
      case 001: // me es útil, así sé con qué nick entro xD
        ownNick = matrix[2];
        break;
      case 266: // fin de conexion, autojoin
        status = STATUS_AUTOJOINING;
        out(QString("Entrando a canales de autojoin: "));
        out(chans);
        out(QString("\n"));
        sendData("JOIN ",true);
        sendData(chans);
        status = STATUS_IDLE;
        break;
      default:
        out(QString("Numeric NO MANEJADO!"));
        //out(matrix[1]); // COMMENTED OUT: error de aserción, list fuera de rango(?)
        out("\n");
        break;
    }
  }
}

void Irc::connected() {
  out(QString("Conectado a "));
  out(server);
  out(QString("\n"));
}

void Irc::disconnected() {
  out(QString("ATENCION: Desconectado de "));
  out(server);
  out(QString("!\n"));
}

void Irc::displayError(QAbstractSocket::SocketError e) { /* TODO */ }

void Irc::sendData(QString outdata) {
  socket->write( outdata.toUtf8() + "\r\n" );
}

void Irc::sendData(QString outdata, bool noTrail) {
  if (noTrail == true)
    socket->write( outdata.toUtf8() );
}
