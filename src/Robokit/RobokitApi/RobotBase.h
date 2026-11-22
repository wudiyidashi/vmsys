#ifndef ROBOTBASE_H
#define ROBOTBASE_H

#include <QObject>
#include <Robokit/RobokitApi/RobProtocol.h>
namespace robokit {
class RobotBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connectionState READ getConnectionState WRITE setConnectionState NOTIFY connectionStateChanged)
public:
    explicit RobotBase(QObject *parent = nullptr);
    bool connectionState=false;

    bool getConnectionState() const;
    void setConnectionState(bool newConnectionState);
signals:
    void connectionStateChanged();

public slots:
    void slot_connectionStateChange(QAbstractSocket::SocketState state);
};
}
#endif // ROBOTBASE_H
