#ifndef ROBOKITMANAGER_H
#define ROBOKITMANAGER_H

#include <QElapsedTimer>
#include <QObject>
#include <QSettings>

#include "Robokit/RobokitApi/RobotStatus.h"
#include "Robokit/RobokitApi/RobotTask.h"
#include "Robokit/RobokitApi/RobotControl.h"
#include "Robokit/RobokitApi/RobotConfig.h"
#include <Mission/MissionManager.h>
#include <Robokit/BoardControl/BoardManager.h>
#include <Server/StatusManager.h>
class RobokitManager : public QObject
{
     Q_OBJECT
public:
    RobokitManager();
    static RobokitManager &instance();

    bool RobokitConnectionInit();

    bool net_connection_statue = false;
    robokit::RobotStatus robot_status;
    robokit::RobotControl robot_controls;
    robokit::RobotTask robot_task;
    robokit::RobotConfig robot_config;

    boardDevice::BoardManager board_manager;

    StatusManager status_manager;

    Misc::TimerEvents timerEvents;

    QString status_server_ip;
    int agv_num = 0;
    QSettings* settingsfile;

    bool checkAllConnection();

    bool getNet_connection_statue() const;
    void setNet_connection_statue(bool newNet_connection_statue);
    QSettings* getAppSettingsfile();
    QString getStatus_server_ip() const;
    Q_INVOKABLE void setStatus_server_ip(const QString &newStatus_server_ip);
    Q_INVOKABLE bool isValidScheduleAddress(const QString &address) const;

    int getAgv_num() const;
    Q_INVOKABLE void setAgv_num(int newAgv_num);

public slots:
    void timeout1hz();
    void slot_connectionStateChanged();
signals:
    void net_connection_statueChanged();

    void status_server_ipChanged();

    void agv_numChanged();

private:

//    RobokitManager(RobokitManager &&) = delete;
//    RobokitManager(const RobokitManager &) = delete;
//    RobokitManager &operator=(RobokitManager &&) = delete;
//    RobokitManager &operator=(const RobokitManager &) = delete;
    

private:
    bool connectionsInitialized = false;
    QElapsedTimer lastMapQuery;

    Q_PROPERTY(bool net_connection_statue READ getNet_connection_statue WRITE setNet_connection_statue NOTIFY net_connection_statueChanged)
    Q_PROPERTY(QString status_server_ip READ getStatus_server_ip WRITE setStatus_server_ip NOTIFY status_server_ipChanged)
    Q_PROPERTY(int agv_num READ getAgv_num WRITE setAgv_num NOTIFY agv_numChanged)
};

#endif // ROBOKITMANAGER_H
