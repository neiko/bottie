#include "chan.h"

#define DEFAULT_ENNUI 100

Chan::Chan(QString channame)
{
  realChan = channame;
  ennui = DEFAULT_ENNUI;
}

Chan::~Chan() {
  //qDebug() << "Destroying Chan for " << realChan << endl;
}

void Chan::down(int down) {
  ennui -= down;
  //qDebug() << "DOWN for" << realChan << ":" << ennui << endl;
}

void Chan::up(int up) {
  ennui += up;
  //qDebug() << "UP for" << realChan << ":" << ennui << endl;
}

void Chan::process() {
  if ( ennui <= 0 ) {
    emit gotDepressed(realChan) ;
    //qDebug() << "I've got tired of" << realChan << ":-(" << endl;
  }
}
