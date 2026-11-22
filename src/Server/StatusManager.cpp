#include "StatusManager.h"
#include "Misc/Log.h"
#include <Misc/TimerEvents.h>

#include "QJsonDocument"
#include <QJsonObject>
#include <Robokit/Map/MapManager.h>
#include <Robokit/RobokitManager.h>
//提供面向上层的状态上传接口
//对遇到的不同状态进行响应

//一般对某个状态量的要求是一直检测它直到变化为某个量再进行下一步
//状态分别有运行态由操作触发的改变，比如顶升杆操作。也有激光雷达，超声波这样的一直改变的状态。
//本类，使用合适的间隔，发送以及处理更新的状态。
//使用方法是在missionTask中连接onLiftAchive2楼这样的信号
//于是车辆抵达二楼后就可以继续操作了。

StatusManager &StatusManager::instance()
{
    static StatusManager singleton;
    return singleton;
}


StatusManager::StatusManager(QObject *parent)
    : RobotBase{parent}
{

//开启自动上传服务
//然后是一些timer更新状态的开启
    //timerEvents = new Misc::TimerEvents(this);
//获取各传感器数据，并处理，触发信号

        connect(&m_protocol,&robokit::RProtocol::errorMessage,this,&StatusManager::getError);
        connect(&m_protocol,&robokit::RProtocol::stateChange,this,&StatusManager::slot_connectionStateChange);

#if 0
        setDefaultAddress("192.168.3.29");
        setDefaultTcpPort(19206);

#elif 1
        setDefaultAddress("192.168.192.5");
        setDefaultTcpPort(29204);
#endif
}

void StatusManager::init()
{

    //timerEvents->startTimers();

     
}

bool StatusManager::initConnection()
{

    if(m_protocol.initTcpConnection(QHostAddress(defaultAddress),defaultTcpPort))
    {
        return true;
    }
    return false;
}

void StatusManager::setDefaultAddress(const QString &newDefaultAddress)
{
    defaultAddress = newDefaultAddress;
    //initConnection();
}

void StatusManager::setDefaultTcpPort(quint16 newDefaultTcpPort)
{
    defaultTcpPort = newDefaultTcpPort;
}

bool StatusManager::checkConnection()
{
    return m_protocol.getProtocolState()!=-1 ? false : true ;
}


void StatusManager::getError(QString msg)
{
    LOGERROR("StatusManager {}",msg.toStdString());
}

void StatusManager::ReceiveFromServer(uint16_t cmd_type, QByteArray& receivedata)
{


}
void StatusManager::SendToServerStatus(const QJsonObject& obj)
{
    QJsonObject JSObject(obj);
    //AGV编号
    JSObject.insert("agv_num",QJsonValue(RobokitManager::instance().getAgv_num()));
     //任务列表 数量
    JSObject.insert("task_list_cnt",QJsonValue(MissionManager::instance().GetTaskCount()));

    JSObject.insert("power",QJsonValue( RobokitManager::instance().board_manager.bat.getPower()));
    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(2002,jsonBuffer,[this](uint16_t revCommand,QByteArray& receivedata){
        qDebug()<<"receivedata";
    });

}
