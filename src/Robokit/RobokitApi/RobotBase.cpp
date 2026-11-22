#include "Robokit/RobokitApi/RobotBase.h"

namespace robokit {
RobotBase::RobotBase(QObject *parent)
    : QObject{parent}
{

}

void RobotBase::slot_connectionStateChange(QAbstractSocket::SocketState state)
{
    if(state ==QAbstractSocket::ConnectedState)
    {
        setConnectionState(true);
    }
    else
    {
        setConnectionState(false);
    }
}



bool RobotBase::getConnectionState() const
{
    return connectionState;
}

void RobotBase::setConnectionState(bool newConnectionState)
{
    if (connectionState == newConnectionState)
        return;
    connectionState = newConnectionState;
    emit connectionStateChanged();
}

}
