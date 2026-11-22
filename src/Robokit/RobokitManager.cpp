/**
 * @file RobokitManager.cpp
 * @author 岳凯 (you@domain.com)
 * @brief 机器人中间件的总成，包含各个组件的控制状态管理
 * @version 0.1
 * @date 2022-11-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "Robokit/RobokitManager.h"

#include <Misc/Log.h>
#include <QDebug>
#include <QThread>
#include <Robokit/Map/MapManager.h>

//#define DEFULTADRSS "192.168.3.29"
#define DEFULTADRSS "192.168.192.5"

RobokitManager::RobokitManager()
{
settingsfile = new QSettings(QCoreApplication::applicationDirPath() + "/app_config.ini", QSettings::IniFormat);
}
RobokitManager &RobokitManager::instance()
{
    static RobokitManager singleton;
    return singleton;
}

/**
 * @brief 所有的机器人的连接应该在这里被初始化
 * 
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool RobokitManager::RobokitConnectionInit()
{
    if (connectionsInitialized)
        return true;
    connectionsInitialized = true;

    auto &map = SeerrMap::GetInstance();
    connect(&robot_status, &robokit::RobotStatus::updatePosition,
            &map, &SeerrMap::UpdateSignlePosition);
    connect(&robot_status, &robokit::RobotStatus::currentMapReported,
            &map, &SeerrMap::setCurrentMap);
    connect(&robot_status, &robokit::RobotBase::connectionStateChanged, this, [this, &map] {
        if (!robot_status.getConnectionState()) {
            lastMapQuery.invalidate();
            map.setCurrentMap({}, {});
            map.clearPosition();
        }
    });
    connect(&robot_config, &robokit::RobotBase::connectionStateChanged, this, [this, &map] {
        if (!robot_config.getConnectionState())
            map.invalidateMap();
    });

    connect(&robot_status, &robokit::RobotBase::connectionStateChanged, this, &RobokitManager::slot_connectionStateChanged);
    connect(&robot_controls, &robokit::RobotBase::connectionStateChanged, this, &RobokitManager::slot_connectionStateChanged);
    connect(&robot_task, &robokit::RobotBase::connectionStateChanged, this, &RobokitManager::slot_connectionStateChanged);
    connect(&robot_config, &robokit::RobotBase::connectionStateChanged, this, &RobokitManager::slot_connectionStateChanged);
    robot_status.setDefaultAddress(DEFULTADRSS);
    robot_status.initConnection() ;
    robot_controls.setDefaultAddress(DEFULTADRSS);
    robot_controls.initConnection();
    robot_task.setDefaultAddress(DEFULTADRSS);
    robot_task.initConnection();
    robot_config.setDefaultAddress(DEFULTADRSS);
    robot_config.initConnection();
        
     QSettings* settingsfile =getAppSettingsfile();
     settingsfile->beginGroup("RobServer");
    QString status_server_ip_ = settingsfile->value("server_ip", -1).toString();
     int agv_num_ = settingsfile->value("agv_num", -1).toInt();
    settingsfile->endGroup();
    if(status_server_ip_!="-1")
    {
        setStatus_server_ip(status_server_ip_);
    }
    else
    {
        setStatus_server_ip("192.168.3.35");
    }

    if(agv_num_!=-1)
    {
        setAgv_num(agv_num_);
    }
    else
    {//可能需要上层给他个号码
        setAgv_num(0);
    }
    status_manager.setDefaultAddress(status_server_ip);
    
    board_manager.initBoardDevice();

    connect(&robot_status,&robokit::RobotStatus::singal_sendToServerStatus,&status_manager,&StatusManager::SendToServerStatus);
    connect(&timerEvents, &Misc::TimerEvents::timeout1Hz, this, &RobokitManager::timeout1hz);
    timerEvents.startTimers();
    return true;
}

int RobokitManager::getAgv_num() const
{
    return agv_num;
}

void RobokitManager::setAgv_num(int newAgv_num)
{
    if (agv_num == newAgv_num)
        return;
    agv_num = newAgv_num;
    QSettings* settingsfile = getAppSettingsfile();
    settingsfile->beginGroup("RobServer");
    settingsfile->setValue("agv_num", agv_num);
    settingsfile->endGroup();
    emit agv_numChanged();
}

QString RobokitManager::getStatus_server_ip() const
{
    return status_server_ip;
}

Q_INVOKABLE void RobokitManager::setStatus_server_ip(const QString &newStatus_server_ip)
{
    if (status_server_ip == newStatus_server_ip)
        return;
    status_server_ip = newStatus_server_ip;

    QSettings* settingsfile = getAppSettingsfile();
    settingsfile->beginGroup("RobServer");
    settingsfile->setValue("server_ip", status_server_ip);
    settingsfile->endGroup();
    status_manager.setDefaultAddress(status_server_ip);
    emit status_server_ipChanged();
}

bool RobokitManager::isValidScheduleAddress(const QString &address) const
{
    return !QHostAddress(address).isNull();
}

bool RobokitManager::getNet_connection_statue() const
{
    return net_connection_statue;
}

Q_INVOKABLE void RobokitManager::setNet_connection_statue(bool newNet_connection_statue)
{
    if (net_connection_statue == newNet_connection_statue)
        return;
    net_connection_statue = newNet_connection_statue;
    emit net_connection_statueChanged();
}
/**
 * @brief 有关仙工的连接变化触发此函数
*/
void RobokitManager::slot_connectionStateChanged()
{
    checkAllConnection();
}
bool RobokitManager::checkAllConnection()
{
    const bool connected = robot_status.checkConnection() && robot_config.checkConnection()
        && robot_controls.checkConnection() && robot_task.checkConnection();
    setNet_connection_statue(connected);
    return connected;
}


void RobokitManager::timeout1hz()
{
    if (!robot_status.checkConnection())
        return;

    robot_status.robot_status_all1_req_get();
    if (!robot_status.hasMapMetadata() && (!lastMapQuery.isValid() || lastMapQuery.hasExpired(5000))) {
        lastMapQuery.start();
        robot_status.robot_status_map_req_get();
    }
    SeerrMap::GetInstance().refreshMap(robot_config);
}

QSettings* RobokitManager::getAppSettingsfile()
{
    return settingsfile;
}
