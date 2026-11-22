#include "Lift.h"

#include <IO/DataProcessUtl.h>
#include <Misc/Log.h>
#include "Misc/CRC.h"

static QString liftIp = "192.168.192.11";

boardDevice::Lift::Lift(QObject *parent)
    : QObject{parent}
{

}

bool boardDevice::Lift::init()
{
    protocol.init(std::bind(&boardDevice::Lift::frameParse,this,std::placeholders::_1));
    return true;
}


void boardDevice::Lift::frameParse(QByteArray& data)
{
    char flag =data.at(4);
    char content =data.at(7);
    if(flag==12 && content == 00)
    {
        qDebug()<<"空闲通过";
       // emit lift_message("12");
    }
    else if(flag==11 &&flag == 00)
    {
        qDebug()<<"呼叫电梯成功";
      //  emit lift_message("11");
    }
    else if(flag==14 && content == 02)
    {
        qDebug()<<"电梯到达呼叫楼层";
        //emit lift_arrived_call_floor(tmp_Data.mid(7,1).toHex());
    }
    else if(flag==15 && content == 00)
    {
        qDebug()<<"接收到回复AGV进入电梯15指令";
       // emit lift_message("15");

    }
    else if(flag==16 && content == 04)
    {
        qDebug()<<"电梯到达目标楼层";
       // emit lift_arrived_last_floor(tmp_Data.mid(7,1).toHex());
    }
    else if(flag==17 && content == 00)
    {
         qDebug()<<"接收到回复AGV出电梯17指令";
       //  emit lift_message("17");
    }
//    else if(flag== "ee")
//    {
//        qDebug()<<"电梯error";
//    }
}


boardDevice::lift::Protocol::Protocol()
{
    m_tcp_connection = new io::TcpSocketClient(this);
}

boardDevice::lift::Protocol::~Protocol()
{
    m_tcp_connection->deleteLater();
    m_tcp_connection=nullptr;
}
void boardDevice::lift::Protocol::init(std::function<void(QByteArray& data)> frameParse)
{

    if(!m_tcp_connection->initConnection(QHostAddress(liftIp),8080))
    {
        LOGDEBUG("connect board failed");
    }
    m_tcp_connection->waitForConnected();
    if(!m_tcp_connection->isOpen())
    {
        LOGDEBUG("connect board failed after wait");
    }

    connect(m_tcp_connection,&io::TcpSocketClient::readyRead,this,&boardDevice::lift::Protocol::receiveReadyRead);
}



void boardDevice::lift::Protocol::wirte(const QByteArray& data)
{

    quint16 data_crc;// = CRC16Data(data);                //计算校验和
   QByteArray send_data;
   QByteArray before_send_data;
   before_send_data = data;
   //加帧头
   send_data.append(0xAA);
   send_data.append(0x55);
   send_data += before_send_data;
   //加校验
   QByteArray tempbyte= QByteArray::fromHex(QByteArray::number(data_crc,16));
   if(tempbyte.length()==1)
   {
       //qDebug()<<"效验数据:"<<tempbyte.toHex();
       send_data.append(QByteArray::fromHex("00")).append(tempbyte);
   }
   else
   {
       send_data.append(tempbyte);
   }
   //agvliftnum_send = send_data.mid(5,1).toHex().toInt(0,16);
   m_tcp_connection->write(send_data);
   //qDebug()<<"agvTolift:"<<agvliftnum_send<<send_data.toHex();
}

void boardDevice::lift::Protocol::receiveReadyRead()
{

    //返回的数据大小不定,需要使用_lastMessage成员变量存放多次触发槽读取的数据.
    QByteArray _message = m_tcp_connection->readAll();
//    LOGDEBUG(_message);
    _message = m_lastmessage + _message;
    int size = _message.size();

    while(size > 0){
        char a0 = _message.at(0);
        char a1 = _message.at(1);
        if (uint8_t(a0) == 0xAA && uint8_t(a1) == 0x55){//检测第一位是否为0xAA.
            if (size >= 17) {//返回的数据最小长度为17位,如果大小小于17则数据不完整等待再次读取.
                uint32_t data_size;//返回所有数据总长值.
                data_size=_message.mid(2,2).toHex().toInt(0,16);
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
//                    qint32 lift_CRC = _message.mid(tmp_Data.count()- 2,2).toHex().toInt(0,16); //接受到的CRC
//                     QByteArray Count_CRC_Data = tmp_Data.mid(2,tmp_Data.count()- 4); //待计算crc的数据
//                     qint32 CRC_Count;
//                     CRC_Count = calculate_crc.CRC16Data(Count_CRC_Data);
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
