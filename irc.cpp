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

#define COLOR_WIPE

Irc::Irc(QString iserver, int iport, QString iownNick, QString ichans, QString iident, QString irealname, QString ipasswd)
{

  server = iserver;
  port = iport;
  ownNick = iownNick;
  chans = ichans;
  ident = iident;
  realname = irealname;
  passwd = ipasswd;

  status = STATUS_WARMING_UP;

  socket = new QTcpSocket( this );
  connect( socket, SIGNAL( readyRead() ), this, SLOT( readData() ) );
  connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
  connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
  connect( socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

  status = STATUS_LOGGING_IN;

  srand (time(NULL));
}

void Irc::goConnect() {
  socket->connectToHost( server, port );
}

void Irc::goDisconnect() {
  socket->disconnect();
}

void Irc::readData() {
  indata += socket->readAll();
  qDebug() << indata << endl; // with this enabled bottie will output to console every raw it reads

  bool taking = false;
  if(indata.right (2)!= "\r\n")
    taking = true;

  QStringList list = indata.split( "\r\n", QString::SkipEmptyParts );

  if( taking ) {
    indata = list.back();
    list.removeLast();
  }

  foreach(QString pendingString,list)
    parse(QString::fromUtf8(qPrintable(pendingString)));

  if(indata.endsWith("\r\n")) indata.clear();
}

void Irc::parse(QString raw) {
  if (raw.startsWith(':'))
    raw = raw.right(raw.length() - 1);

#ifdef COLOR_WIPE
  QString xraw; int len = 0; int olen = raw.length();
  // dirty but useful color removal code
  do {
    do {
      if ( raw [len] == '\003' ) { // color, veo muuucho color
        len += 2;
        if ( raw [len].isNumber() ) len++;
        if ( raw [len] == ',' && raw [len+1].isDigit() ) { // color, veo aúuuun más color
          len += 2;
          if ( raw [len].isNumber() ) len++;
        }
      } else if ( raw [len] == '\002' || raw [len] == '\026' || raw [len] == '\035' ) // blablalba
        len++;
    } while ( raw [len] == '\003' || raw [len] == '\002'|| raw [len] == '\026'||
              raw [len] == '\035' ); // esto es una guarrada pero evita varios control codes consecutivos
      xraw.append(raw[len]);
      len++;
  } while( len < olen );
  raw = xraw;
#endif

  QStringList matrix = raw.split(' ');
  if( matrix[0] == "PING" ) { // Recibido ping, pongoneamos.
    sendData("PONG " + matrix[1]);
    emit PingPong();

  } else if ( matrix[0] == "ERROR" ) { // error de conexion, whichever
    emit connError(raw.right(raw.length() - 7)); //quita el "ERROR :" del principio y deja el msj limpio

  } else if ( matrix[1] == "PRIVMSG" || matrix[1] == "NOTICE" ) {
    QString nick = matrix[0].left(matrix[0].indexOf('!'));
    QString message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" PRIVMSG") - nick.length()));

    if ( matrix[1] == "PRIVMSG" ) {
      if ( matrix[2].startsWith("#") ) { // mensaje de canal
        if ( message.startsWith("\001") ) // /ctcp
          if ( message.startsWith("\001ACTION ")) // me
            emit chanme ( nick, mask, matrix[2], message.right((message.length() - 8)) );
          else
            emit chanctcp ( nick, mask, matrix[2], message.right((message.length() - 1)) );
        else
          emit chanmsg ( nick, mask, matrix[2], message );
      } else { // mensaje en privado
        if ( message.startsWith("\001") ) // /me
          if ( message.startsWith("\001ACTION ")) // me
            emit queryme ( nick, mask, message.right((message.length() - 8)) );
          else
            emit queryctcp ( nick, mask, message.right((message.length() - 1)) );
        else
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
    QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" NICK") - nick.length()));
    QString newnick = raw.right((raw.length() - 2) - raw.indexOf(" :"));
    if (newnick == ownNick)
      emit ownNickChange(newnick);
    emit nickChange(nick,mask,newnick);
  } else if ( matrix[1] == "MODE" ) { // cambio de modo, pero no sé si es de usuario o canal.
    if ( matrix[2].startsWith('#') ) { // c mode
      QString nick = matrix[0].left(matrix[0].indexOf('!'));
      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" MODE") - nick.length()));
      QString chan = matrix[2];
      QString mode = raw.right((raw.length() - raw.indexOf(" #")) - chan.length() - 2);
      emit modeChange(nick,mask,chan,mode);
    }
    else // u mode
      emit umodeChange(matrix[0],matrix[2],raw.right((raw.length() - 2) - raw.indexOf(" :")));
  } else if ( matrix[1] == "KICK" ) { // expulsión del canal
    if ( matrix[2].startsWith('#') ) { // c mode
      QString nick = matrix[0].left(matrix[0].indexOf('!'));
      QString mask = matrix[0].mid(matrix[0].indexOf('!') + 1,(matrix[0].indexOf(" KICK") - nick.length()));
      QString chan = matrix[2];
      QString kicked = matrix[3];
      QString message;
      if (raw.indexOf(" :") != -1)
        message = raw.right((raw.length() - 2) - raw.indexOf(" :"));
      emit kick(nick,mask,chan,kicked,message);
    }
    else // u mode
      emit umodeChange(matrix[0],matrix[2],raw.right((raw.length() - 2) - raw.indexOf(" :")));
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
        if (!chans.isEmpty()) {
          sendData("JOIN ",true);
          sendData(chans);
        }
        status = STATUS_IDLE;
        emit signedIn();
        break;
      case 321: // chanlist begin
        handleChanlist(false);
        break;
      case 322: // chanlist
        handleChanlist(matrix[3],matrix[4],raw.right((raw.length() - 2) - raw.indexOf(" :")));
        break;
      case 323: // chanlist end
        handleChanlist(true);
        break;
      case 332: //topic
        emit topic(matrix[3],raw.right((raw.length() - 2) - raw.indexOf(" :")));
        break;
      case 333: //topic timestamp
        emit topicTime(matrix[3],matrix[4],matrix[5]);
        break;
      case 372: // texto de motd
        emit motd( raw.right((raw.length() - 2) - raw.indexOf(" :")));
        break;
      case 375: // inicio de motd
        emit motdStart( raw.right((raw.length() - 2) - raw.indexOf(" :")));
        break;
      case 376: // fin de motd
        emit motdEnd( raw.right((raw.length() - 2) - raw.indexOf(" :")));
        break;
      case 433: // nick en uso!
        getNewRandomNick();
        break;
      default:
        //qDebug() << "Numeric NO MANEJADO!" << matrix[1] << endl;
        break;
    }
  }
}

void Irc::connected() {
  emit gotConnection();
  if(!passwd.isEmpty())
    sendData("PASS :" + passwd);
  sendData("NICK " + ownNick + "\nUSER " + ident + " " + ownNick + " " + \
    server + " :" + realname);
  status = STATUS_AUTOJOINING;
}

void Irc::disconnected() {
  emit gotDisconnection();
}

void Irc::displayError(QAbstractSocket::SocketError e) { /* TODO */ }

void Irc::sendData(QString outdata) {
  // qDebug() << outdata << endl; // with this enabled, debugging of output raws.
  socket->write( outdata.toUtf8() + "\r\n" );
}

void Irc::sendData(QString outdata, bool noTrail) {
  if (noTrail == true)
    socket->write( outdata.toUtf8() );
}

void Irc::getNewRandomNick() {
  QString ran, newNick = ownNick;
  int i = rand() % (999 - 99 + 1) + 99;
  ran.setNum(i);
  newNick.append(ran);
  emit usedNick ( ownNick, newNick );
  sendData("NICK ",true);
  sendData(newNick);
  emit ownNickChange ( newNick );
}

void Irc::handleChanlist(QString channame, QString users, QString topic) {
    channames.append( channame );
    userscount.append( users );
    chantopics.append( topic );
}

void Irc::handleChanlist(bool end) {
  if ( end == false ) {
    channames.clear();
    userscount.clear();
    chantopics.clear();
  } else if ( end == true ) {
    emit listResults( channames, userscount, chantopics );
  }
}
