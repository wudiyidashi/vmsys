#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <QObject>

#include <Robokit/RobokitApi/RobProtocol.h>
#include "Robokit/RobokitApi/RobotBase.h"
namespace robokit {
class RobotConfig : public RobotBase
{
    Q_OBJECT
public:
    RobotConfig();

    void setDefaultAddress(const QString &newDefaultAddress);
    void setDefaultTcpPort(quint16 newDefaultTcpPort);

    void robot_config_downloadmap_req_get(const QJsonObject &obj);
    void robot_config_downloadmap_req_get(const QJsonObject &obj, std::function<void (uint16_t revCommand,QByteArray &)> callback);

    bool Connected();
    bool checkConnection();
public Q_SLOTS:
    bool initConnection();

    void getError(QString msg);
private:

    robokit::RProtocol m_protocol;
    QString defaultAddress;
    quint16 defaultTcpPort;
    QByteArray m_receiveData;//来自tcp返回的数据

    void robot_config_downloadmap_res_receive(uint16_t revCommand,QByteArray &receivedata);
    void robot_config_uploadmap_res_receive(uint16_t revCommand,QByteArray &receivedata);
};
}



#endif // ROBOTCONFIG_H
