#include "lurker.h"

Lurker::Lurker()
{
  server = "irc.telecable.es";
  port = 6667;
  ownNick = "*";
  chans = "#irc-hispano,#barcelona,#madrid,#mas_de_30,#mas_de_40";
  ident = "adent";
  realname = "Nombre real :-)";
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
  connect(interface, SIGNAL(channotice(QString,QString,QString,QString)), this, SLOT(channotice(QString,QString,QString,QString)));
  connect(interface, SIGNAL(querynotice(QString,QString,QString)), this, SLOT(querynotice(QString,QString,QString)));
  connect(interface, SIGNAL(part(QString,QString,QString,QString)), this, SLOT(part(QString,QString,QString,QString)));
  connect(interface, SIGNAL(quit(QString,QString,QString)), this, SLOT(quit(QString,QString,QString)));
  connect(interface, SIGNAL(nickChange(QString,QString,QString)), this, SLOT(nickChange(QString,QString,QString)));

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

void Lurker::out(QString mes) { // no se especificó tipo así que presupondré que será OUT y así ahorro teclado
  QTextStream cout(stdout, QIODevice::WriteOnly);
  cout << "\033[15;1m" << mes << "\033[0m";
}

void Lurker::out(QString mes, int type, int colour) {
  QTextStream cout(stdout, QIODevice::WriteOnly);
  if (type == OUT_COLORED)
    cout << "\033[" << colour << ";2m" << mes << "\033[0m";
  else
    qDebug() << "out() with wrong type!" << endl;
}


void Lurker::gotConnection(){
  out(QString("Conectado a ") + server + QString("\n"));
}

void Lurker::gotDisconnection(){
  out(QString("ATENCION: Desconectado de ") + server);
}

void Lurker::PingPong(){
  out(QString("Ping? PONG!\n"),OUT);
}

void Lurker::join(QString nick,QString mask,QString chan) {
  timestamp();
  out(QString("---> "),OUT_COLORED,36);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
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
}

void Lurker::chanmsg(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("<"),OUT_COLORED,36);
  out(nick,OUT_COLORED,37);
  out(QString("@"),OUT_COLORED,36);
  out(chan,OUT_COLORED,37);
  out(QString("> "),OUT_COLORED,36);
  out(message,OUT_COLORED,37);
  out(QString("\n"),OUT);
}
void Lurker::querymsg(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("<"),OUT_COLORED,36);
  out(nick,OUT_COLORED,37);
  out(QString("> "),OUT_COLORED,36);
  out(message,OUT_COLORED,37);
  out(QString("\n"),OUT);
}
void Lurker::channotice(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("/"),OUT_COLORED,36);
  out(nick,OUT_COLORED,37);
  out(QString("@"),OUT_COLORED,36);
  out(chan,OUT_COLORED,37);
  out(QString("/ "),OUT_COLORED,36);
  out(message,OUT_COLORED,37);
  out(QString("\n"),OUT);
}
void Lurker::querynotice(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("/"),OUT_COLORED,36);
  out(nick,OUT_COLORED,37);
  out(QString("/ "),OUT_COLORED,36);
  out(message,OUT_COLORED,37);
  out(QString("\n"),OUT);
}

void Lurker::part(QString nick,QString mask,QString chan,QString message) {
  timestamp();
  out(QString("<--- "),OUT_COLORED,36);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // part propio o ajeno???
    out(QString("I've"),OUT_COLORED,36);
  else {
    out(nick, OUT_COLORED, 36);
    out(QString(" ["));
    out(mask,OUT_COLORED,36);
    out(QString("]"));
  }
  out(QString(" parted "),OUT_COLORED,36);
  out(chan, OUT_COLORED, 36);
  if (QString::compare(message, "")) {
    out(QString(" ["));
    out(message, OUT_COLORED, 36);
    out(QString("]"));
  }
  out(QString("\n"));
}

void Lurker::quit(QString nick,QString mask,QString message) {
  timestamp();
  out(QString("<--- "),OUT_COLORED,36);
  out(nick, OUT_COLORED, 36);
  out(QString(" ["));
  out(mask,OUT_COLORED,36);
  out(QString("]"));
  out(QString(" has quit "),OUT_COLORED,36);
  if (QString::compare(message, "")) {
    out(QString("["));
    out(message, OUT_COLORED, 36);
    out(QString("]\n"));
  }
}

void Lurker::ownNickChange(QString newnick){
  myNick = newnick;
}

void Lurker::timestamp(){
  QTime time = QTime::currentTime();
  QString timeString = time.toString();
  out(QString("[") + timeString + QString("] "));
}

void Lurker::nickChange(QString nick,QString mask,QString newnick) {
  timestamp();
  out(QString("  -> "),OUT_COLORED,36);
  if (!QString::compare(nick, myNick, Qt::CaseInsensitive)) // JOIN propio o ajeno???
    out(QString("I have"),OUT_COLORED,36);
  else {
    out(nick, OUT_COLORED, 36);
    out(QString(" ["));
    out(mask,OUT_COLORED,36);
    out(QString("]"));
  }
    out(QString(" changed nick to "),OUT_COLORED,36);
    out(newnick, OUT_COLORED, 36);
    out(QString("\n"));
}
