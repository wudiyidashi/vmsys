#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QHostInfo>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include "IO/ThreadHandle.h"
namespace io
{
    class TcpSocketClient : public QTcpSocket
    {
        Q_OBJECT
    public:
        explicit TcpSocketClient(QObject* parent = nullptr);
        explicit TcpSocketClient(qintptr socketDescriptor, QObject* parent = nullptr);
        ~TcpSocketClient();

        bool initConnection(const QHostAddress& address, quint16 port);

        void resetConnection(const QHostAddress& address, quint16 port);

        void reConnect();
    signals:
        void sig_sockDisConnect(qint64, const QString&, quint16, QThread*);//NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的

    public slots:
        void tcpClientDisConnected();
        void disConTcp(qint64 socketDescriptor);
    private slots:
        void slotConnected();
        void onErr(QAbstractSocket::SocketError error);
    private:
        QHostAddress m_remoteIp;
        //    QString m_remoteIp;
        quint16 m_remotePort;
        qintptr socketID;
        QTimer* timer;
        QMetaObject::Connection dis;
        void onCheckState();
    };
}

#endif // TCPSOCKETCLIENT_H
