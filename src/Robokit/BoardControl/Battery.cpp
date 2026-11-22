#include "Battery.h"

#include <IO/DataProcessUtl.h>
#include <Misc/Log.h>
#include <Misc/Utilities.h>
/*Bms 串口协议 V82_1.6*/

boardDevice::Bat::Bat(QObject *parent)
    : QObject{parent}
{
    connect(&timer, &QTimer::timeout, this, &boardDevice::Bat::realTimeReq);
    connect(&protocol, &bat::Protocol::connectionStateChanged, this, &Bat::connectionStateChanged);
}

bool boardDevice::Bat::isConnected() const
{
    return protocol.isConnected();
}

bool boardDevice::Bat::init()
{
    //为协议提供解析函数入口
    bool ret = protocol.init(std::bind(&boardDevice::Bat::frameParse,this,std::placeholders::_1));
    return ret;
//    return true;
}


void boardDevice::Bat::frameParse(QByteArray& data)
{

            qint8 data_length_1H = Assic_To_Hex(data.mid(7,1).toHex().toInt(0,10));
            qint8 data_length_1L = Assic_To_Hex(data.mid(8,1).toHex().toInt(0,10));
            qint8 data_length_2H = Assic_To_Hex(data.mid(9,1).toHex().toInt(0,10));
            qint8 data_length_2L = Assic_To_Hex(data.mid(10,1).toHex().toInt(0,10));
            int length = data_length_1H*4096 + data_length_1L*256 + data_length_2H*16 + data_length_2L;
//            qDebug()<<"长度:"<<length;
            if(data.length() >= length)
            {
                QByteArray tmp_data = data.mid(0,length);
//                qDebug()<<tmp_data.toHex();
                data.remove(0,length);
                if(tmp_data.mid(length-1,1).toHex() == "7e")
                {
                    QByteArray All_Data = tmp_data.mid(5,20);
            //        qDebug()<<"bat_data:"<<bat_data.mid(129,2);

                    static qint8 m = 0;
                    static qint8 n = 0;
                    static qint8 charge3 = 0;
                    static qint8 charge4 = 0;
                    static qint8 alarm3 = 0;
                    static qint8 alarm4 = 0;
                    QByteArray bat_msg;
                    QByteArray bat_num;
                    QByteArray mos_stat;
                    if(tmp_data.count() == 142)
                    {
                       bat_msg = tmp_data.mid(129,2);
                       bat_num = tmp_data.mid(89,2);
                       mos_stat = tmp_data.mid(99,2);
                    }
                    else if(tmp_data.count() == 170)
                    {
                        bat_msg = tmp_data.mid(157,2);
                        bat_num = tmp_data.mid(117,2);
                        mos_stat = tmp_data.mid(127,2);
                    }
                    else if(tmp_data.count() == 136)
                    {
                        bat_msg = tmp_data.mid(123,2);
                        bat_num = tmp_data.mid(83,2);
                        mos_stat = tmp_data.mid(93,2);
                    }

                    if(tmp_data.count() == 142 || tmp_data.count() == 170 || tmp_data.count() == 136)
                    {
                        m = Assic_To_Hex(bat_msg.mid(0,1).toHex().toInt(0,10));
                        n = Assic_To_Hex(bat_msg.mid(1,1).toHex().toInt(0,10));
                        charge3 = Assic_To_Hex(bat_num.mid(0,1).toHex().toInt(0,10));
                        charge4 = Assic_To_Hex(bat_num.mid(1,1).toHex().toInt(0,10));
                        alarm3 = Assic_To_Hex(mos_stat.mid(0,1).toHex().toInt(0,10));
                        alarm4 = Assic_To_Hex(mos_stat.mid(1,1).toHex().toInt());
                        qint8 bat_power = m*16 + n;
                        //qDebug()<<"电量:"<<bat_power;
                        QByteArray charge_stat = QByteArray::number(charge3*16 + charge4);
                        QByteArray mosb_stat = QByteArray::number(alarm3*16 + alarm4);

                        setPower(bat_power);    //电池电量信息
//                          qDebug()<<"dianci"<<bat_power;
//                        qDebug()<<"电池充电状态:"<<charge_stat;
                        emit bat_charge_stat(charge_stat);    //电池充电状态
//                        emit bat_mos(mosb_stat);
//                        emit bat_error(All_Data.mid(14,1).toHex());  //电池mos的警告信息
                    }
                }
                tmp_data.clear();
            }


}

void boardDevice::Bat::startTimerReq()
{
    timer.start(500);
}
//请求实时信息
void boardDevice::Bat::realTimeReq()
{
    protocol.wirte(QByteArray::fromHex("3A3030303230303030306565387E"));
}

boardDevice::bat::Protocol::Protocol()
{
    m_tcp_connection = new io::TcpSocketClient(this);
    connect(m_tcp_connection, &QAbstractSocket::stateChanged, this, &Protocol::connectionStateChanged);
}

bool boardDevice::bat::Protocol::isConnected() const
{
    return m_tcp_connection->state() == QAbstractSocket::ConnectedState;
}

boardDevice::bat::Protocol::~Protocol()
{
    m_tcp_connection->deleteLater();
    m_tcp_connection=nullptr;
}

bool boardDevice::bat::Protocol::init(std::function<void(QByteArray& data)> _frameParse)
{
//192.168.192.12
#if 0
    if(!m_tcp_connection->initConnection("127.0.0.1",8082))
#else
  if(!m_tcp_connection->initConnection(QHostAddress("192.168.192.12"),8082))
#endif
  {
        LOGDEBUG("connect Battery failed");
    }

    if(!m_tcp_connection->isOpen())
    {
        LOGDEBUG("connect Battery failed after wait");
        return false;
    }
    frameParse=_frameParse;
    connect(m_tcp_connection,&io::TcpSocketClient::readyRead,this,&boardDevice::bat::Protocol::receiveReadyRead);
    return true;
}

void boardDevice::bat::Protocol::wirte(const QByteArray& data)
{
    QByteArray send_data;
    send_data += data;
    if(!m_tcp_connection->isOpen())
    {
        LOGWARN("connect board NOT OPen");
        return;
    }
   // LOGDEBUG(send_data.toHex().toStdString());
    m_tcp_connection->write(send_data);
}

void boardDevice::bat::Protocol::receiveReadyRead()
{

    //返回的数据大小不定,需要使用_lastMessage成员变量存放多次触发槽读取的数据.
    QByteArray _message = m_tcp_connection->readAll();
//    LOGDEBUG(_message);
    _message = m_lastmessage + _message;
    int size = _message.size();

    //qDebug()<<"bat_data:"<<bat_data.toHex();
    if(_message.mid(0,1).toHex()!="3a")
    {
        for(int i=0;i<_message.count();i++)
        {
            if(_message.mid(i,1).toHex()=="3a")
            {
                _message.remove(0,i);
                break;
            }
        }
    }
    else
    {
        if(_message.length() > 10)
        {
            QByteArray tempMessage;
            if(m_lastmessage.isEmpty()){
                tempMessage = _message;
            }else{
                tempMessage = m_lastmessage;
            }
            //数据区域解析
            frameParse(tempMessage);
            size = _message.size();
            m_lastmessage.clear();

        }
    }



}


namespace boardDevice {
quint8 Bat::getPower() const
{
    return power;
}

void Bat::setPower(quint8 newPower)
{
    if (power == newPower)
        return;
    power = newPower;
    emit powerChanged();
}

}
