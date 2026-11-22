#include "RobotConfig.h"
#include "Robokit/RobokitApi/RobProtocol.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <utility>

#include <Misc/Log.h>
robokit::RobotConfig::RobotConfig()
{
#if 0
    setDefaultAddress("192.168.3.29");
    setDefaultTcpPort(19207);


#elif 1
    setDefaultAddress("192.168.192.5");
    setDefaultTcpPort(19207);

#endif

    connect(&m_protocol,&robokit::RProtocol::errorMessage,this,&robokit::RobotConfig::getError);
    connect(&m_protocol, &robokit::RProtocol::stateChange, this, &robokit::RobotConfig::slot_connectionStateChange);

}


void robokit::RobotConfig::setDefaultAddress(const QString &newDefaultAddress)
{
    defaultAddress = newDefaultAddress;
    ///initConnection();
}

void robokit::RobotConfig::setDefaultTcpPort(quint16 newDefaultTcpPort)
{
    defaultTcpPort = newDefaultTcpPort;
}

bool robokit::RobotConfig::Connected()
{
    //
    return true;
}

bool robokit::RobotConfig::checkConnection()
{
    return m_protocol.getProtocolState()!=-1 ? false : true ;
}

bool robokit::RobotConfig::initConnection()
{
    if(m_protocol.initTcpConnection(QHostAddress(defaultAddress),defaultTcpPort))
    {
        return true;
    }
    return false;
}

void robokit::RobotConfig::getError(QString msg)
{
    LOGERROR("RobotConfig {}",msg.toStdString());
}
//上传地图到机器人

void robokit::RobotConfig::robot_config_uploadmap_res_receive(uint16_t revCommand,QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}



//地图名(不能包含中文等非法字符, 只能使用 0-9, a-z, A-Z, -, _)
//JSObject.insert("map_name",QJsonValue(map_name));


void robokit::RobotConfig::robot_config_downloadmap_req_get(const QJsonObject& obj,std::function<void(uint16_t revCommand,QByteArray&)> callback)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    m_protocol.WriteTcpData(4011, jsonBuffer, std::move(callback), true);

}
//从机器人下载地图
void robokit::RobotConfig::robot_config_downloadmap_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    m_protocol.WriteTcpData(4011, jsonBuffer,
                            std::bind(&RobotConfig::robot_config_uploadmap_res_receive, this,
                                      std::placeholders::_1, std::placeholders::_2),
                            true);
}

void robokit::RobotConfig::robot_config_downloadmap_res_receive(uint16_t revCommand,QByteArray& receivedata)
{
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    QStringList keys = rootObject.keys();
    //检查该操作是否出错
    if(rootObject.value("ret_code").toDouble()!=QJsonValue::Undefined )
    {

    }
    else
    {
        //API 错误码
        double ret_code=rootObject.value("ret_code").toDouble();
        //API 上传时间戳
        QString create_on=rootObject.value("create_on").toString();
        //错误信息
        QString err_msg=rootObject.value("err_msg").toString();
    }
}
