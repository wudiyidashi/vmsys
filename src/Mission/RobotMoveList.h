#ifndef ROBOTMOVELIST_H
#define ROBOTMOVELIST_H

#include "MissionBase.h"
namespace subTask {

struct move_list_element_s
{
    QString id ;
    QString source_id ;
    QString task_id ;
};

class RobotMoveList : public MissionBase
{
public:
    explicit RobotMoveList(QObject *parent = nullptr);
    void run();
    void quit();
    void init();

    QList<move_list_element_s> station_list;
};

}

#endif // ROBOTMOVELIST_H
