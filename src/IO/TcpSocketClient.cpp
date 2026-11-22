/**
 * @file TcpSocketClient.cpp
 * @author 岳凯 (yue_kai@aliyun.com)
 * @brief 继承于Qtcpsocket 实现连接管理如自动重连、掉线检测
 * @version 0.1
 * @date 2022-11-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "TcpSocketClient.h"
#include <Misc/Log.h>
 

#if 0
  //keepalive 引用自https://zhuanlan.zhihu.com/p/553068992
  //该机制能在无协议对接（心跳帧）的情况下探测连接状态 比如
const int keepalive = 1;    // 开启keepalive属性
const int keepidle = 5;     // 如果连接在5秒内没有任何数据来往则进行探测
const int keepinterval = 3; // 探测时发包的时间间隔为3秒
const int keepcount = 3;    // 尝试探测的次数， 如果第一次探测包就收到响应，则不在继续探测

#if defined (Q_OS_LINUX) || defined (Q_OS_MACOS)
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

// 开启TCP心跳检测机制
int enableKeepalive(int fd) {
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) return -1;
    if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)) < 0) return -1;
    if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(keepinterval)) < 0) return -1;
    if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &keepcount, sizeof(keepcount)) < 0) return -1;
    return 0;
}
#elif defined (Q_OS_WIN)
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)

struct tcp_keepalive {
    unsigned long onoff;
    unsigned long keepalivetime;
    unsigned long keepaliveinterval;
};

int enableKeepalive(int fd) {
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepalive, sizeof(keepalive)) < 0) return -1;

    struct tcp_keepalive in_keep_alive;
    memset(&in_keep_alive, 0, sizeof(in_keep_alive));
    unsigned long ul_in_len = sizeof(struct tcp_keepalive);
    struct tcp_keepalive out_keep_alive;
    memset(&out_keep_alive, 0, sizeof(out_keep_alive));
    unsigned long ul_out_len = sizeof(struct tcp_keepalive);
    unsigned long ul_bytes_return = 0;

    in_keep_alive.onoff = 1;                                // 打开keepalive
    in_keep_alive.keepaliveinterval = keepinterval * 1000;  // 发送keepalive心跳时间间隔-单位为毫秒
    in_keep_alive.keepalivetime = keepidle * 1000;          // 多长时间没有报文开始发送keepalive心跳包-单位为毫秒

    if (WSAIoctl(fd, SIO_KEEPALIVE_VALS, (LPVOID)&in_keep_alive, ul_in_len,
        (LPVOID)&out_keep_alive, ul_out_len, &ul_bytes_return, NULL, NULL) < 0) return -1;

    return 0;
}
#else
int enableKeepalive(int fd) {
    Q_UNUSED(fd);
    return -1;
}
#endif

#endif
namespace io
{
    TcpSocketClient::TcpSocketClient(QObject* parent) : QTcpSocket(parent)
    {
        timer = new QTimer(this);
    }

    TcpSocketClient::TcpSocketClient(qintptr socketDescriptor, QObject* parent)
        : QTcpSocket(parent), socketID(socketDescriptor)
    {
        this->setSocketDescriptor(socketDescriptor);
        //socket readyRead 信号给server
    //    connect(this, &QTcpSocket::readyRead, this, &TcpSocket::OnReadyRead);
        dis = connect(this, &QTcpSocket::disconnected,
            [&]() {
                emit sig_sockDisConnect(socketID, this->peerAddress().toString(), this->peerPort(), QThread::currentThread());//发送断开连接的用户信息
                this->deleteLater();
            });
        timer = new QTimer(this);
    }


    TcpSocketClient::~TcpSocketClient()
    {
        this->disconnectFromHost();
        if (this->state() != QAbstractSocket::UnconnectedState) {
            this->waitForDisconnected();
        }
    }

    // 状态检查定时器槽函数
    void TcpSocketClient::onCheckState()
    {
        switch (this->state()) {
        case QAbstractSocket::UnconnectedState:

            reConnect();
            break;

        case QAbstractSocket::HostLookupState:
            break;
        case QAbstractSocket::ConnectingState:
            break;
        case QAbstractSocket::ConnectedState:
            break;
        case QAbstractSocket::BoundState:
            break;
        case QAbstractSocket::ListeningState:
            break;
        case QAbstractSocket::ClosingState:
            break;
        }
    }

    void TcpSocketClient::onErr(QAbstractSocket::SocketError error)
    {
        switch (error)
        {
        case QAbstractSocket::AddressInUseError:
            LOGINFO("SOCKET ERROR: Address is already in use");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            LOGINFO("SOCKET ERROR: Connection refused");
            break;
        case QAbstractSocket::HostNotFoundError:
            LOGINFO("SOCKET ERROR: Host not found");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            LOGINFO("SOCKET ERROR: Remote host closed");
            break;
        }
        QTimer::singleShot(3000, this, &TcpSocketClient::reConnect); // Added here
    }

    bool TcpSocketClient::initConnection(const QHostAddress& address, quint16 port)
    {
        m_remoteIp = address;
        m_remotePort = port;

        //QObject::connect(this, &TcpSocketClient::connected, this, &TcpSocketClient::slotConnected);
        QObject::connect(this, &QAbstractSocket::errorOccurred, this, &TcpSocketClient::onErr);
        QObject::connect(this->timer, &QTimer::timeout, this, &TcpSocketClient::slotConnected);
        //timer->start(2000);
        this->connectToHost(m_remoteIp, m_remotePort);
        
        return true;
    }

    void  TcpSocketClient::resetConnection(const QHostAddress& address, quint16 port)
    {
        m_remoteIp = address;
        m_remotePort = port;

        this->disconnectFromHost();
        this->close();
        this->abort();
        this->connectToHost(m_remoteIp, m_remotePort);
    }
    void TcpSocketClient::slotConnected()
    {

        // 未连接时，每隔两秒自动向指定服务器发送连接请求
        if (this->state() == QAbstractSocket::UnconnectedState)
        {
            //qDebug() << "connect state" << TSocket->state();
            this->abort();
            this->connectToHost(m_remoteIp, m_remotePort);
        }
        // 连接成功后关闭初始化连接定时器，后面的断线重连与此无关
        else if (this->state() == QAbstractSocket::ConnectedState)
        {
            timer->stop();
        }

       
        //    if(enableKeepalive(socketDescriptor())!=0)
        //    {
        //        LOGDEBUG("fail to enable SO_KEEPALIVE");
        //    }
    }
    void TcpSocketClient::disConTcp(qint64 socketDescriptor)
    {
        if (socketDescriptor == socketID)
        {
            this->disconnectFromHost();
        }
        else if (socketDescriptor == -1) //-1为全部断开
        {
            disconnect(dis); //先断开连接的信号槽，防止二次析构
            this->disconnectFromHost();
            this->deleteLater();
        }
    }
    void  TcpSocketClient::reConnect()
    {

        //this->disconnectFromHost();
        //this->close();
        this->abort();
        this->connectToHost(m_remoteIp, m_remotePort);
    }

    void  TcpSocketClient::tcpClientDisConnected()
    {
        LOGWARN("tcp Client DisConnected");
    }
}

