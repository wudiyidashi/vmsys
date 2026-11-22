#ifndef JACK_H
#define JACK_H

#include "MissionBase.h"

#include <QTime>

namespace subTask {
class Jack : public MissionBase
{
public:
    explicit Jack(bool _jacking,QObject *parent = nullptr);
    void run();
    void quit();
    void init();
    bool jacking;
    QTime startTime;
};
}



#endif // JACK_H
