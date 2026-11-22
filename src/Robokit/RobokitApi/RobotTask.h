#ifndef ROBOTTASK_H
#define ROBOTTASK_H

#include <QObject>

#include <Robokit/RobokitApi/RobProtocol.h>
#include "Robokit/RobokitApi/RobotBase.h"

namespace robokit {
class RobotTask : public RobotBase
{
    Q_OBJECT
public:
    RobotTask();

    void setDefaultAddress(const QString &newDefaultAddress);
    void setDefaultTcpPort(quint16 newDefaultTcpPort);
    void robot_task_pause_req_get();
    void robot_task_resume_req_get();
    void robot_task_cancel_req_get();
    void robot_task_translate_req(const QJsonObject& obj);
    void robot_task_target_path_req_get(QString id);
    void robot_task_cleartargetlist_req_get();
    void robot_task_gotarget_req_get(const QJsonObject &obj);
    void robot_task_gotarget2_req_get(const QJsonObject &obj);
    void robot_task_gotargetlist_req_get(const QJsonObject &obj);
    void robot_task_target_path_req_get(const QJsonObject &obj);
    void robot_task_turn_req(const QJsonObject& obj);
    void robot_task_cancel_req();
    
    bool checkConnection();

public Q_SLOTS:
    bool initConnection();
    void getError(QString msg);

    void _SendDataForTest();
signals:


private:

    robokit::RProtocol m_protocol;
    QString defaultAddress;
    quint16 defaultTcpPort;
    QByteArray m_receiveData;//来自tcp返回的数据

    void robot_task_gotarget_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_gotarget2_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_gotargetlist_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_pause_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_resume_req_receive(uint16_t revCommand,QByteArray& receivedata);
    
    void robot_task_cancel_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_target_path_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_task_cleartargetlist_req_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_task_translate_req_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_task_turn_req_receive(uint16_t revCommand, QByteArray& receivedata);
    ;
};
}


#endif // ROBOTSTATUS_H
