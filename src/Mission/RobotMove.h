#ifndef ROBOTMOVE_H
#define ROBOTMOVE_H

#include "MissionBase.h"
namespace subTask {
struct move_element_s
{
    QString id ;
    QString source_id ;
};

class RobotMove : public MissionBase
{
    public:
        explicit RobotMove(const move_element_s& _station,QObject *parent = nullptr);
        void run();
        void quit();
        void init();
        move_element_s move_info;
        QString goal_station;
};
}

#endif // ROBOTMOVE_H
