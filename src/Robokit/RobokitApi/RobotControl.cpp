#include "RobotControl.h"

#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>

#include <Misc/Log.h>
robokit::RobotControl::RobotControl()
{
#if 0
    setDefaultAddress("192.168.3.29");
    setDefaultTcpPort(19205);

#elif 1
    setDefaultAddress("192.168.192.5");
    setDefaultTcpPort(19205);


#endif
     connect(&m_protocol,&robokit::RProtocol::errorMessage,this,&robokit::RobotControl::getError);
     connect(&m_protocol, &robokit::RProtocol::stateChange, this, &robokit::RobotControl::slot_connectionStateChange);
}

bool robokit::RobotControl::initConnection()
{
    if(m_protocol.initTcpConnection(QHostAddress(defaultAddress),defaultTcpPort))
    {
        return true;
    }
    return false;
}


void robokit::RobotControl::setDefaultAddress(const QString &newDefaultAddress)
{
    defaultAddress = newDefaultAddress;
}

void robokit::RobotControl::setDefaultTcpPort(quint16 newDefaultTcpPort)
{
    defaultTcpPort = newDefaultTcpPort;
}
bool robokit::RobotControl::checkConnection()
{
  return m_protocol.getProtocolState()!=-1 ? false : true ;
}

void robokit::RobotControl::_SendDataForTest()
{
    LOGDEBUG(("callback robot_control_stop_req_receive"));
    QByteArray sendData;
    m_protocol.WriteTcpData(2000,sendData,std::bind(&robokit::RobotControl::robot_control_stop_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotControl::robot_control_stop_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(2000,sendData,std::bind(&robokit::RobotControl::robot_control_stop_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotControl::getError(QString msg)
{
    LOGERROR("RobotControl {}",msg.toStdString());

}

void robokit::RobotControl::robot_control_stop_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    LOGDEBUG("callback robot_control_stop_req_receive");
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}

/*
    QJsonObject JSObject;
    JSObject.insert("x",QJsonValue(x));
    JSObject.insert("y",QJsonValue(y));
    JSObject.insert("angle",QJsonValue(angle));
    JSObject.insert("length",QJsonValue(length));
    JSObject.insert("home",QJsonValue(home));
 */
void robokit::RobotControl::robot_control_reloc_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
     m_protocol.WriteTcpData(2002,jsonBuffer,std::bind(&robokit::RobotControl::robot_control_reloc_req_receive, this, std::placeholders::_1, std::placeholders::_2));

}

void robokit::RobotControl::robot_control_reloc_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}
void robokit::RobotControl::robot_control_comfirmloc_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(2003,sendData,std::bind(&robokit::RobotControl::robot_control_comfirmloc_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotControl::robot_control_comfirmloc_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}

void robokit::RobotControl::robot_control_cancelreloc_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(2004,sendData,std::bind(&robokit::RobotControl::robot_control_cancelreloc_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotControl::robot_control_cancelreloc_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}


 //JSObject.insert("vx",QJsonValue(vx));
    //JSObject.insert("vy",QJsonValue(vy));
    //JSObject.insert("w",QJsonValue(w));
    //JSObject.insert("steer",QJsonValue(steer));
    //JSObject.insert("real_steer",QJsonValue(real_steer));
    //JSObject.insert("duration",QJsonValue(duration));
void robokit::RobotControl::robot_control_motion_req_get(const QJsonObject& obj)
{
   
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(2010,jsonBuffer,std::bind(&robokit::RobotControl::robot_control_motion_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotControl::robot_control_motion_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}

 //JSObject.insert("map_name",QJsonValue(map_name));
 
void robokit::RobotControl::robot_control_loadmap_req_get(const QJsonObject)
{
    QByteArray sendData;
    QJsonObject JSObject;
   
    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(2022,jsonBuffer,std::bind(&robokit::RobotControl::robot_control_loadmap_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotControl::robot_control_loadmap_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}

//JSObject.insert("name",QJsonValue(name));
void robokit::RobotControl::robot_control_clearmotorencoder_req_get(const QJsonObject)
{
    QByteArray sendData;
    QJsonObject JSObject;
    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
     m_protocol.WriteTcpData(2024,jsonBuffer,std::bind(&robokit::RobotControl::robot_control_clearmotorencoder_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotControl::robot_control_clearmotorencoder_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}
void robokit::RobotControl::robot_control_upload_and_loadmap_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(2025,sendData,std::bind(&robokit::RobotControl::robot_control_upload_and_loadmap_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotControl::robot_control_upload_and_loadmap_req_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}
