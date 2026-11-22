#include "MissionBase.h"
//
#include <Misc/Log.h>

using namespace subTask;

MissionBase::MissionBase(QObject *parent)
    : QObject{parent}
{

}

MissionBase::~MissionBase()
{
//LOGDEBUG("dsadasd");
}

void MissionBase::changeTaskStatus(subTask::TaskStatus status)
{
//对进程上锁
    this->status=status;
}

void MissionBase::runTask()
{
    //qDebug()<<status;
    switch (status) {
    case onInit:
        init();
        break;
    case onRun:
        run();
        break;
    case onQuit:
        quit();
        break;
    default:
        break;
    }
}

bool MissionBase::isTaskOver()
{
    if(status==onNext)
    {

        return true;
    }
     return false;
}

