#include "McuBoard.h"

#include <IO/DataProcessUtl.h>
#include <Misc/Log.h>
namespace boardDevice {
McuBoard::McuBoard(QObject *parent)
    : QObject{parent}
{
    connect(&protocol, &boardDevice::mcuBoard::Protocol::stateChange, this, &boardDevice::McuBoard::slot_connectionStateChange);
    connect(&timer, &QTimer::timeout, this, &boardDevice::McuBoard::slot_queryBoard);
    timer.setInterval(300);
}
//自动门数据发送，0为关闭自动门
void boardDevice::McuBoard::DoorControl(const QByteArray& data)
{

     protocol.wirte(QByteArray::fromHex(data));
}
}


bool boardDevice::McuBoard::init()
{
    protocol.init(std::bind(&boardDevice::McuBoard::frameParse,this,std::placeholders::_1));
    return true;
}

void boardDevice::McuBoard::slot_connectionStateChange(QAbstractSocket::SocketState state)
{
    if(state ==QAbstractSocket::ConnectedState)
    {
        setConnectionState(true);
        timer.start();
    }
    else
    {
        setConnectionState(false);
        timer.stop();
    }
}
void boardDevice::McuBoard::drawerControl(bool drawer_stat)
{
    LOGDEBUG("drawer_stat{}",drawer_stat);
    if(drawer_stat)
    {
        protocol.wirte(QByteArray::fromHex("0203")); //0x03:平板通知AGV主控板推杆下降
    }
    else
    {
        protocol.wirte(QByteArray::fromHex("0202"));    //0x02:平板通知AGV主控板推杆上升
    }
}

void boardDevice::McuBoard::slot_queryBoard()
{
    //LOGDEBUG("query board");
     protocol.wirte(QByteArray::fromHex("0200")); //0x00:查询
}

void boardDevice::McuBoard::frameParse(QByteArray& data)
{
    if (data.mid(2, 2).toHex() == "0d20")
    {
        int IOData1 = data.mid(5, 1).toHex().toInt(0, 16);
        //       qDebug()<<"data1: "<<IOData1<<"data2:"<<int(IOData1&0x80);
        if ((uchar(IOData1 & 0x80) == 128) && (uchar(IOData1 & 0x40) == 0x40))               //01后退,02左转，03右转
        {
            emit red_line(QByteArray("01"));
            //            qDebug()<<"后退";
        }
        else if ((uchar(IOData1 & 0x80) == 0) && (uchar(IOData1 & 0x40) == 0x40))
        {
            emit red_line(QByteArray("03"));
            //            qDebug()<<"右转";
        }
        else if ((uchar(IOData1 & 0x80) == 128) && (uchar(IOData1 & 0x40) == 0))
        {
            emit red_line(QByteArray("02"));
            //            qDebug()<<"左转";
        }
        else
        {
            emit red_line(QByteArray("04"));
        }
    }
}



boardDevice::mcuBoard::Protocol::Protocol()
{
    m_tcp_connection = new io::TcpSocketClient(this);
    connect(m_tcp_connection,&io::TcpSocketClient::stateChanged,this,&boardDevice::mcuBoard::Protocol::stateChange);
}

boardDevice::mcuBoard::Protocol::~Protocol()
{
    m_tcp_connection->deleteLater();
    m_tcp_connection=nullptr;
}
void boardDevice::mcuBoard::Protocol::init(std::function<void(QByteArray& data)> frameParse)
{

//    192.168.192.10 8080
#if 0
    if(!m_tcp_connection->initConnection("127.0.0.1",8888))
#else
  if(!m_tcp_connection->initConnection(QHostAddress("192.168.192.10"),8080))
#endif
    {
        LOGDEBUG("connect board failed");
    }
  this->frameParse = frameParse;
    connect(m_tcp_connection,&io::TcpSocketClient::readyRead,this,&boardDevice::mcuBoard::Protocol::receiveReadyRead);
}

void boardDevice::mcuBoard::Protocol::wirte(const QByteArray& data)
{

    quint16 data_crc = IO::DataProcessUtl::CRC16Data(data);            //计算校验和
    QByteArray send_data;
    //加帧头
    send_data.append(0xAA);
    send_data.append(0x55);
    send_data.append(data.length()+1);
    send_data += data;
    //加校验
    send_data.append(QByteArray::fromHex(QByteArray::number(data_crc,16)));
    if(!m_tcp_connection->isOpen())
    {
        LOGWARN("connect board NOT OPen");
    }
    LOGDEBUG(send_data.toHex().toStdString());
    m_tcp_connection->write(send_data);
}

void boardDevice::mcuBoard::Protocol::receiveReadyRead()
{

    //返回的数据大小不定,需要使用_lastMessage成员变量存放多次触发槽读取的数据.
    QByteArray _message = m_tcp_connection->readAll();
//    LOGDEBUG(_message);
    _message = m_lastmessage + _message;
    int size = _message.size();

    while(size > 1){
        char a0 = _message.at(0);
        char a1 = _message.at(1);
        if (uint8_t(a0) == 0xAA && uint8_t(a1) == 0x55){//检测第一位是否为0xAA.
            if (size >2) {//返回的数据最小长度为17位,如果大小小于17则数据不完整等待再次读取.
                uint32_t data_size;//返回所有数据总长值.
                data_size=_message.at(2) ;
                int remaining_size = size - 4;//所有数据总长度 - 4 = 数据区长度.
                //如果返回数据长度值 大于 已读取数据长度 表示数据还未读取完整，跳出循环等待再次读取..
                if (data_size > remaining_size) {
                    m_lastmessage = _message;
                    break;
                }else{//返回数据长度值 大于等于 已读取数据，开始解析.
                    QByteArray tempMessage;
                    if(m_lastmessage.isEmpty()){
                        tempMessage = _message;
                    }else{
                        tempMessage = m_lastmessage;
                    }
                    //数据区域解析
                    frameParse(tempMessage);

                    _message = _message.right(remaining_size - data_size);
                    size = _message.size();
                    
                    m_lastmessage.clear();
                }

            } else{
                m_lastmessage = _message;
                break;
            }
        }else{
            //报头数据错误，找不到0xAA 0x55就一步步往后挪
            _message = _message.right(size - 1);
            size = _message.size();
        }
    }

}

//void Client::displayError(QAbstractSocket::SocketError socketError)
//{
//    switch (socketError) {
//    case QAbstractSocket::RemoteHostClosedError:
//        break;
//    case QAbstractSocket::HostNotFoundError:
//        QMessageBox::information(this, tr("Fortune Client"),
//                                 tr("The host was not found. Please check the "
//                                    "host name and port settings."));
//        break;
//    case QAbstractSocket::ConnectionRefusedError:
//        QMessageBox::information(this, tr("Fortune Client"),
//                                 tr("The connection was refused by the peer. "
//                                    "Make sure the fortune server is running, "
//                                    "and check that the host name and port "
//                                    "settings are correct."));
//        break;
//    default:
//        QMessageBox::information(this, tr("Fortune Client"),
//                                 tr("The following error occurred: %1.")
//                                 .arg(tcpSocket->errorString()));
//    }

//    getFortuneButton->setEnabled(true);
//}


namespace boardDevice {
bool McuBoard::getConnectionState() const
{
    return connectionState;
}

void McuBoard::setConnectionState(bool newConnectionState)
{
    if (connectionState == newConnectionState)
        return;
    connectionState = newConnectionState;
    emit connectionStateChanged();
}

}
