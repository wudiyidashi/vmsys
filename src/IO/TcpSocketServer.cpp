#include "TcpSocketServer.h"

#include "ThreadHandle.h"


namespace io {
    TcpSocketServer::TcpSocketServer(int numConnections, QObject* parent)
        : QTcpServer(parent)
    {
        setMaxPendingConnections(numConnections);
        nConnectCount = 0;
    }

    TcpSocketServer::~TcpSocketServer()
    {
        emit this->sentDisConnect(-1);
        this->close();
    }

    void TcpSocketServer::setMaxPendingConnections(int numConnections)
    {
        qDebug() << "new connection" << "socketDescriptor=" << numConnections;
        this->QTcpServer::setMaxPendingConnections(numConnections);//调用Qtcpsocket函数，设置最大连接数，主要是使maxPendingConnections()依然有效
        this->maxConnections = numConnections;
    }

    void TcpSocketServer::incomingConnection(qintptr socketDescriptor) //多线程必须在此函数里捕获新连接
    {
        qDebug() << "new connection" << "socketDescriptor=" << socketDescriptor;
        if (tcpClient.size() > maxConnections)//继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
        {
            QTcpSocket tcp;
            tcp.setSocketDescriptor(socketDescriptor);
            tcp.disconnectFromHost();
            return;
        }
        //QThread* th = ThreadHandle::getClass().getThread();
        TcpSocketClient* tcpTemp = new TcpSocketClient(socketDescriptor);
        QString ip = tcpTemp->peerAddress().toString();
        quint16 port = tcpTemp->peerPort();

        //NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
        connect(tcpTemp, &TcpSocketClient::sig_sockDisConnect, this, &TcpSocketServer::sockDisConnectSlot);
        connect(this, &TcpSocketServer::sentDisConnect, tcpTemp, &TcpSocketClient::disConTcp);//断开信号

        //tcpTemp->moveToThread(th);//把tcp类移动到新的线程，从线程管理类中获取
        tcpClient.insert(socketDescriptor, tcpTemp);//插入到连接信息中
        emit sig_connectClient(socketDescriptor, ip, port);
        nConnectCount++;
        qDebug() << "new connection" << "socketDescriptor=" << socketDescriptor;
        qDebug() << "当前连接个数为：" << nConnectCount;
    }

    void TcpSocketServer::sockDisConnectSlot(qint64 socketDescriptor, const QString& ip, quint16 port, QThread* th)
    {
        nConnectCount--;
        qDebug() << "disconnect" << "socketDescriptor=" << socketDescriptor;
        qDebug() << "当前连接个数为：" << nConnectCount;
        tcpClient.remove(socketDescriptor);//连接管理中移除断开连接的socket
        ThreadHandle::getClass().removeThread(th); //告诉线程管理类那个线程里的连接断开了
        emit sig_sockDisConnect(socketDescriptor, ip, port);
    }

    void TcpSocketServer::clear()
    {
        emit this->sentDisConnect(-1);
        ThreadHandle::getClass().clear();
        tcpClient.clear();
    }


}
