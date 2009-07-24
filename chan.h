#ifndef CHAN_H
#define CHAN_H

#include <QtCore>
#include <QString>

class Chan : public QObject
{
Q_OBJECT

public:
    Chan(QString);
    ~Chan();
    void change(int);
    unsigned int getEnnui() { return ennui; }

signals:
    void gotDepressed(QString);

private:
    QString realChan;
    int ennui;
};

#endif // CHAN_H
