#ifndef COUNTER_H
#define COUNTER_H

#include <QThread>
#include <QLCDNumber>

class counter : public QThread
{

public:
    counter(QLCDNumber *counter);

private:
    QLCDNumber* counter_text;
    void run();

};

#endif // COUNTER_H
