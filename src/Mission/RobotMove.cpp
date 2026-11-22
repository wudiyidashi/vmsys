#include "RobotMove.h"
#include <Misc/Log.h>

#include <Robokit/RobokitManager.h>
#include <Mission/MissionManager.h>
#include <QJsonObject>
namespace subTask
{
RobotMove::RobotMove(const move_element_s& _station,QObject *parent)
{
    name=eRobotMove;
    move_info=_station;
    goal_station = _station.id;
}
void RobotMove::init()
{
    //task_ID = MissionManager::instance().getNewTaskID();

    LOGDEBUG("{}",move_info.id.toStdString());
    QJsonObject obj;
    obj.insert("id",QJsonValue(move_info.id));
    obj.insert("source_id",QJsonValue(move_info.source_id));
    //obj.insert("task_id",QJsonValue(task_ID));

   RobokitManager::instance().robot_task.robot_task_gotarget_req_get(obj);
   changeTaskStatus(onRun);
}

void RobotMove::run()
{
if( RobokitManager::instance().robot_status.current_station==goal_station)
{
 changeTaskStatus(onQuit);
}
}

void RobotMove::quit()
{
 changeTaskStatus(onNext);
}
}
