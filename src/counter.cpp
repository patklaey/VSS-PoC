#include "counter.h"

counter::counter(QLCDNumber *counter)
{
    this->counter_text = counter;
}

void counter::run()
{
    int i = 0;

    while (i < 36000)
    {
        this->counter_text->display(i++);
        msleep(100);
    }
}
