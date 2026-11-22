#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "IO/TcpSocketClient.h"
namespace io {
    //多线程管理
    //抽象出类似于Socket的单一输出接口
    //class TcpSocket : public QTcpSocket
    //{
    //    Q_OBJECT
    //public:
    //    explicit TcpSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    //    virtual ~TcpSocket();

    //signals:
    //    void sig_sockDisConnect(qint64, const QString &, quint16, QThread *);//NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的

    //public slots:
    ////    void send(QByteArray& data, const QByteArray& dataType);
    //    void disConTcp(qint64 socketDescriptor);
    //private slots:
    ////    void OnReadyRead();

    //private:


    //private:
    //    qintptr socketID;

    //    QMetaObject::Connection dis;
    //};

    class TcpSocketServer : public QTcpServer
    {
        Q_OBJECT
    public:
        explicit TcpSocketServer(int numConnections = 10, QObject* parent = nullptr);
        ~TcpSocketServer();

        void setMaxPendingConnections(int numConnections);//重写设置最大连接数函数
        void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程

        QHash<qint64, TcpSocketClient*> tcpClient;//管理连接的map
    signals:
        void sig_connectClient(qint64, const QString&, quint16);//发送新用户连接信息
        void sig_sockDisConnect(qint64, const QString&, quint16);//断开连接的用户信息
        void sentDisConnect(qint64); //断开特定连接，并释放资源，-1为断开所有。

    public slots:
        void clear(); //断开所有连接，线程计数器请0
    private slots:
        void sockDisConnectSlot(qint64 socketDescriptor, const QString& ip, quint16 port, QThread* th);//断开连接的用户信息

    private:

        int maxConnections;
        int nConnectCount;
    };
}
#endif // TCPSOCKETSERVER_H
