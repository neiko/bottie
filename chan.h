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
    void down(int);
    void up(int);
    int getEnnui() { return ennui; }
    void process();

signals:
    void gotDepressed(QString);

private:
    QString realChan;
    int ennui;
};

#endif // CHAN_H
