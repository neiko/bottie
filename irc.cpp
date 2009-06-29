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

Irc::Irc(QString iserver, int iport, QString iownNick, QString ichans, QString iident, QString irealname)
{
  // TODO: handle this with QConfig

  server = iserver;
  port = iport;
  ownNick = iownNick;
  chans = ichans;
  ident = iident;
  realname = irealname;

  status = STATUS_WARMING_UP;

  socket = new QTcpSocket( this );
  connect( socket, SIGNAL( readyRead() ), this, SLOT( readData() ) );
  connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
  connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
  connect( socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

  status = STATUS_LOGGING_IN;
  socket->connectToHost( server, port );
}

void Irc::goConnect() {
}

void Irc::goDisconnect() {
  socket->disconnect();
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
    emit PingPong();

  } else if ( matrix[1] == "NOTICE" && matrix[2] == "IP_LOOKUP" && status == STATUS_LOGGING_IN) {
    //Inicio de sesión
    //out(QString("Iniciando sesion...\n"),OUT);

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

  } else if ( matrix[1] == "PRIVMSG" || matrix[1] == "NOTICE" ) {
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PRIVMSG") - nick.length()));

    if ( matrix[1] == "PRIVMSG" ) {
      if ( matrix[2].startsWith("#") ) { // mensaje de canal
        emit chanmsg ( nick, mask, matrix[2], message );
      } else { // mensaje en privado
        emit querymsg ( nick, mask, message );
      }
    } else if ( matrix[1] == "NOTICE" ) {
      if ( matrix[2].startsWith("#") ) { // notice en canal
        emit channotice ( nick, mask, matrix[2], message );
      } else { // notice en privado
        emit querynotice ( nick, mask, message );
      }
    }
  } else if ( matrix[1] == "JOIN" ) { //join a un canal
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" JOIN") - nick.length()));
    QString chan = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    emit join(nick,mask,chan);

  } else if ( matrix[1] == "PART" || matrix[1] == "QUIT" ) { //handled together
    QString message = "", chan = "";
    if (raw.indexOf(" :") != -1)
      message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PART") - nick.length()));

    if ( matrix[1] == "PART" ) {
      QString chan = raw.right((raw.length() - 1) - raw.indexOf(" #"));
      chan = chan.left(chan.indexOf(" :"));
      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PART") - nick.length()));
      emit part(nick,mask,chan,message);
    } else if ( matrix[1] == "QUIT" ) {
      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" QUIT") - nick.length()));
      emit quit(nick,mask,message);
    }
  } else if ( matrix[1] == "NICK" ) {
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" JOIN") - nick.length()));
    QString newnick = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    if (newnick == ownNick)
      emit ownNickChange(newnick);
    emit nickChange(nick,mask,newnick);
  }
  bool isInt;
  int code_msg = matrix[1].toInt( &isInt );
  if( isInt ) {
    switch ( code_msg ) {
      case 001: // me es útil, así sé con qué nick entro xD
        emit ownNickChange(matrix[2]);
        ownNick = matrix[2];
        break;
      case 266: // fin de conexion, autojoin
        status = STATUS_AUTOJOINING;
        //qDebug() << "Entrando a canales de autojoin: " << chans << endl;
        sendData("JOIN ",true);
        sendData(chans);
        status = STATUS_IDLE;
        break;
      default:
        //qDebug() << "Numeric NO MANEJADO!" << matrix[1] << endl;
        break;
    }
  }
}

void Irc::connected() {
  emit gotConnection();
}

void Irc::disconnected() {
  emit gotDisconnection();
}

void Irc::displayError(QAbstractSocket::SocketError e) { /* TODO */ }

void Irc::sendData(QString outdata) {
  socket->write( outdata.toUtf8() + "\r\n" );
}

void Irc::sendData(QString outdata, bool noTrail) {
  if (noTrail == true)
    socket->write( outdata.toUtf8() );
}
