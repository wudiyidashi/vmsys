
#include "Misc/Log.h"
#include <QtEndian>
#include <utility>

#include <Robokit/RobokitApi/RobProtocol.h>
#include <Robokit/RobokitApi/SCHeadData.h>

//trace log 跟踪性能robokit::RProtocol::
 robokit::RProtocol::RProtocol(QObject *parent)
     : QObject{parent}
{
     m_tcpConnection = new io::TcpSocketClient(this);
     connect(m_tcpConnection, &io::TcpSocketClient::readyRead, this, &RProtocol::receiveTcpReadyRead);
     connect(m_tcpConnection,&io::TcpSocketClient::stateChanged,this,&RProtocol::stateChange);
     connect(m_tcpConnection, &QAbstractSocket::disconnected, this, [this] {
         m_lastmessage.clear();
         matchedReplies.clear();
     });
}

 robokit::RProtocol::RProtocol(io::TcpSocketClient* tcp_ptr, QObject* parent)
     : QObject{ parent }
 {
     m_tcpConnection = tcp_ptr;
     connect(m_tcpConnection, &io::TcpSocketClient::readyRead, this, &RProtocol::receiveTcpReadyRead);
 }

bool robokit::RProtocol::initTcpConnection(const QHostAddress &address,const quint16 port)
{
   if(!m_tcpConnection->initConnection(address,port))
   {
        return false;
   }
  return true;
}



void robokit::RProtocol::getError(QString msg)
{
    emit errorMessage(msg);
}


qint64 robokit::RProtocol::getProtocolState()
{
    int _return = -1;
    if(m_tcpConnection->state()!=QAbstractSocket::ConnectedState)
        _return = 0;

    return _return;
}

//设置在接收中通过基础校验的数据段返回函数
void robokit::RProtocol::SetNnalyzeCallBackFunc(protocol_cb_t callback)
{
    callBackFunNoModify = false;
    m_receiveFinishCallback = callback;
}

bool robokit::RProtocol::WriteTcpData(uint16_t sendCommand,
    const QByteArray& sendData,
    protocol_cb_t callback, bool matchResponse)
{
    //qDebug() << &m_tcpConnection;
    if(m_tcpConnection->state()!=QAbstractSocket::ConnectedState)
    {
        //m_tcpConnection->reConnect();
        //LOGDEBUG("No connection!{}",m_tcpConnection->state());
        emit errorMessage("No connection");
        return false;
    }
    const quint16 requestNumber = nextRequestNumber++;
    //数据区长度.
    int size = 0;
    //报文头部数据.
    uint8_t* headBuf = Q_NULLPTR;
    int headSize = 0;
    //发送的全部数据.
    SeerData* seerData = Q_NULLPTR;
    //开始计时.
    m_elapsedtime.start();
    //根据数据区数据进行数据转换.
    if(sendData.isEmpty()){
        headSize = sizeof(SeerHeader);
        headBuf = new uint8_t[headSize];
        seerData = (SeerData*)headBuf;
        size = seerData->setData(sendCommand,Q_NULLPTR,0,requestNumber);
    }else{
        std::string json_str = sendData.toStdString();
        headSize = sizeof(SeerHeader) + json_str.length();
        headBuf = new uint8_t[headSize];
        seerData = (SeerData*)headBuf;
        size = seerData->setData(sendCommand,
                                 (uint8_t*)json_str.data(),
                                 json_str.length(),
                                 requestNumber);
    }
    //锁定接收函数
    if (matchResponse)
    {
        for (auto reply = matchedReplies.begin(); reply != matchedReplies.end();) {
            if (reply->deadline.hasExpired())
                reply = matchedReplies.erase(reply);
            else
                ++reply;
        }
        matchedReplies.insert(requestNumber, {sendCommand, QDeadlineTimer(60000), std::move(callback)});
        matchedResponseTypes.insert(sendCommand);
        matchedResponseTypes.insert(sendCommand + 10000);
    }
    else if (callBackFunNoModify)
    {
        m_receiveFinishCallback = std::move(callback);
    }


    //发送的所有数据.
    m_tcpConnection->write((char*)seerData, size);

//    qDebug()<<QByteArray::fromRawData((char*)seerData, size).toHex(' ');
    delete[] headBuf;

    return true;
}

 void robokit::RProtocol::receiveTcpReadyRead()
 {
     //qDebug()<<&m_tcpConnection;
     //返回的数据大小不定,需要使用_lastMessage成员变量存放多次触发槽读取的数据.
     QByteArray _message = m_tcpConnection->readAll();
     _message = m_lastmessage + _message;
     int size = _message.size();
   // qDebug()<<_message;
     while(size > 0){
         char a0 = _message.at(0);
         if (uint8_t(a0) == 0x5A){//检测第一位是否为0x5A.
             if (size >= 16) {//返回的数据最小长度为16位,如果大小小于16则数据不完整等待再次读取.
                 SeerHeader* header = new SeerHeader;
                 memcpy(header, _message.data(), 16);

                 uint32_t data_size;//返回所有数据总长值.
                 uint16_t revCommand;//返回报文数据类型.
                 uint16_t number;//返回序号.
                 qToBigEndian(header->m_length,(uint8_t*)&(data_size));
                 qToBigEndian(header->m_type, (uint8_t*)&(revCommand));
                 qToBigEndian(header->m_number, (uint8_t*)&(number));
                 delete header;

                 int remaining_size = size - 16;//所有数据总长度 - 头部总长度16 = 数据区长度.
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
                     QByteArray headB = _message.left(16);
                     //截取报头16位后面的数据区数据.
                     QByteArray json_data = _message.mid(16,data_size);
                    if (matchedResponseTypes.contains(revCommand)) {
                        const auto reply = matchedReplies.find(number);
                        if (reply != matchedReplies.end()
                            && (revCommand == reply->command || revCommand == reply->command + 10000)) {
                            const auto match = std::move(reply.value());
                            matchedReplies.erase(reply);
                            if (!match.deadline.hasExpired() && match.callback)
                                match.callback(revCommand, json_data);
                        }
                    } else if (m_receiveFinishCallback) {
                        auto callback = m_receiveFinishCallback;
                        callback(revCommand, json_data);
                    }
                     _message = _message.right(remaining_size - data_size);
                     size = _message.size();
                     m_lastmessage.clear();
                 }

             } else{
                 m_lastmessage = _message;
                 break;
             }
         }else{
             //报头数据错误，找不到0x5A就一步步往后挪
             _message = _message.right(size - 1);
             size = _message.size();
         }
     }
     int msTime = m_elapsedtime.elapsed();
 }

