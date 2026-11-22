#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QObject>
#include <Robokit/RobokitApi/RobProtocol.h>
#include "Robokit/RobokitApi/RobotBase.h"
namespace robokit {
class RobotControl : public RobotBase
{

    Q_OBJECT
public:
    RobotControl();

    void setDefaultAddress(const QString &newDefaultAddress);
    void setDefaultTcpPort(quint16 newDefaultTcpPort);


    void robot_control_stop_req_get();
    void robot_control_reloc_req_get(const QJsonObject& obj);
    void robot_control_comfirmloc_req_get();
    void robot_control_cancelreloc_req_get();
    void robot_control_motion_req_get(double vx=0,double vy =0,double w=0,double steer=-2,double real_steer=0,double duration=0);
    void robot_control_loadmap_req_get(QString map_name="");
    void robot_control_clearmotorencoder_req_get(QString name="");
    void robot_control_upload_and_loadmap_req_get();
 	void robot_control_motion_req_get(const QJsonObject& obj);
    void robot_control_loadmap_req_get(const QJsonObject);
    void robot_control_clearmotorencoder_req_get(const QJsonObject);

    bool checkConnection();
public Q_SLOTS:
    bool initConnection();

    void _SendDataForTest();
    void getError(QString msg);
private:

    robokit::RProtocol m_protocol;
    QString defaultAddress;
    quint16 defaultTcpPort;
    QByteArray m_receiveData;//来自tcp返回的数据

    void robot_control_stop_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_reloc_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_comfirmloc_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_cancelreloc_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_motion_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_loadmap_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_clearmotorencoder_req_receive(uint16_t revCommand,QByteArray& receivedata);
    void robot_control_upload_and_loadmap_req_receive(uint16_t revCommand,QByteArray& receivedata);
};

}

#endif // ROBOTCONTROL_H
