#include "chan.h"

#define DEFAULT_ENNUI 100

Chan::Chan(QString channame)
{
  realChan = channame;
  ennui = DEFAULT_ENNUI;
}

Chan::~Chan() {}

void Chan::change(signed int changes) {
  if ( ennui + changes <= 0 )
    emit gotDepressed(realChan);
  else
    ennui += changes;
}
