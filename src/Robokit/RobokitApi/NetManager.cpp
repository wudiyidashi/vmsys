#include <Robokit/RobokitApi/NetManager.h>
NetManager::NetManager(QObject *parent)
    : QObject{parent}
{


}

bool NetManager::init(const QString &address,const quint16 port)
{
    m_tcpConnection = new IO::TcpSocket(address,port);

    if(!m_tcpConnection->open(QIODeviceBase::ReadWrite))
    {
        return false;
    }

    return true;
}


