#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>

#include <IO/TcpSocket.h>

class NetManager : public QObject
{
public:
    explicit NetManager(QObject *parent = nullptr);

public:
    bool init(const QString &address,const quint16 port);
signals:

private:
        IO::TcpSocket* m_tcpConnection;
};

#endif // NETMANAGER_H
