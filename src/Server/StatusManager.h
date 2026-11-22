#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <QObject>
#include <Misc/TimerEvents.h>
#include "Robokit/RobokitApi/RobProtocol.h"
#include "Robokit/RobokitApi/RobotBase.h"
class StatusManager : public robokit::RobotBase
{
    Q_OBJECT
public:
    explicit StatusManager(QObject *parent = nullptr);
    static StatusManager &instance();
    Misc::TimerEvents* timerEvents;
    void init();

    void setDefaultAddress(const QString &newDefaultAddress);
    void setDefaultTcpPort(quint16 newDefaultTcpPort);
    bool checkConnection();

public Q_SLOTS:
    bool initConnection();
    void getError(QString msg);
    void SendToServerStatus(const QJsonObject &obj);

private:
    robokit::RProtocol m_protocol;
    QString defaultAddress;
    quint16 defaultTcpPort;
    QByteArray m_receiveData;//来自tcp返回的数据
    void ReceiveFromServer(uint16_t cmd_type, QByteArray& receivedata);

};

#endif // STATUSMANAGER_H
