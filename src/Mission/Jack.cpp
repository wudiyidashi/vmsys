#include "Jack.h"
#include "Robokit/BoardControl/BoardManager.h"
#include "Robokit/RobokitManager.h"
namespace subTask
{
Jack::Jack(bool _jacking,QObject *parent)
    : jacking(_jacking)
{

}
void Jack::init()
{
    qDebug()<<"exctue jack task";
    startTime = QTime::currentTime();
    RobokitManager& RobokitManagerd = RobokitManager::instance();
    boardDevice::McuBoard& McuBoard = RobokitManagerd.board_manager.mcu_board;
    McuBoard.drawerControl(jacking);
   changeTaskStatus(onRun);
}

void Jack::run()
{
if( startTime.secsTo(QTime::currentTime())>6)
{
 changeTaskStatus(onQuit);
}
}

void Jack::quit()
{
 changeTaskStatus(onNext);
}
}
