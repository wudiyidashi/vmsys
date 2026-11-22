#include "RobotMoveList.h"
#include <Misc/Log.h>
#include <Robokit/RobokitManager.h>
#include <Mission/MissionManager.h>
#include <QJsonArray>
#include <QJsonObject>

namespace subTask {
RobotMoveList::RobotMoveList(QObject *parent)
    : MissionBase{parent}
{
    name = eRobotMoveList;
}

void RobotMoveList::init()
{


    QJsonObject rootObject;
    // 定义 [ ] 对象
    QJsonArray move_task_list_obj;
    qDebug() << "下发导航任务";
    for (int i = 0; i < station_list.size(); i++)
    {
        //task_ID = MissionManager::instance().getNewTaskID();
        QString source_id = station_list[i].source_id;
        QString id = station_list[i].id;
        QString task_id = station_list[i].task_id;
        qDebug() << i << ":" << source_id << "->" << id;
        // 定义 { } 对象
        QJsonObject move_task_obj;
//        if(i==0)
//        {
//            move_task_obj.insert("id", source_id);
//            move_task_obj.insert("source_id", "SELF_POSITION");
//            move_task_obj.insert("task_id", MissionManager::instance().getNewTaskID());
//            move_task_list_obj.append(move_task_obj);
//        }
        move_task_obj.insert("id", id);
        move_task_obj.insert("source_id", source_id);
        move_task_obj.insert("task_id", task_id);
        move_task_list_obj.append(move_task_obj);
    }

    rootObject.insert("move_task_list", move_task_list_obj);
   // qDebug()<<rootObject;
   RobokitManager::instance().robot_task.robot_task_gotargetlist_req_get(rootObject);
   changeTaskStatus(onRun);
}

void RobotMoveList::run()
{
    if((RobokitManager::instance().robot_status.task_type ==3||
            RobokitManager::instance().robot_status.task_type ==4)&&
            RobokitManager::instance().robot_status.task_status==4)
    {
    changeTaskStatus(onQuit);
    }

}

void RobotMoveList::quit()
{
    LOGDEBUG("结束任务");
 changeTaskStatus(onNext);
}
}

