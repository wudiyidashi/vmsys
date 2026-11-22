#include "RobotTask.h"
#include "Misc/Log.h"

#include <QJsonDocument>
#include <QJsonObject>

robokit::RobotTask::RobotTask()
{
#if 0
    setDefaultAddress("192.168.3.29");
    setDefaultTcpPort(19206);

#elif 1
  setDefaultAddress("192.168.192.5");
    setDefaultTcpPort(19206);

#endif
        connect(&m_protocol,&robokit::RProtocol::errorMessage,this,&robokit::RobotTask::getError);
        connect(&m_protocol,&robokit::RProtocol::stateChange,this,&robokit::RobotTask::slot_connectionStateChange);
}

bool robokit::RobotTask::initConnection()
{
    if(m_protocol.initTcpConnection(QHostAddress(defaultAddress),defaultTcpPort))
    {
        return true;
    }
    return false;
}

void robokit::RobotTask::setDefaultAddress(const QString &newDefaultAddress)
{
    defaultAddress = newDefaultAddress;
}

void robokit::RobotTask::setDefaultTcpPort(quint16 newDefaultTcpPort)
{
    defaultTcpPort = newDefaultTcpPort;
}



bool robokit::RobotTask::checkConnection()
{
     return m_protocol.getProtocolState()!=-1 ? false : true ;
}
void robokit::RobotTask::_SendDataForTest()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(3051,sendData,std::bind(&robokit::RobotTask::robot_task_gotarget_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
/*
    QJsonObject JSObject;
    JSObject.insert("id",QJsonValue(id));
    JSObject.insert("source_id",QJsonValue(source_id));
    JSObject.insert("task_id",QJsonValue(task_id));
    JSObject.insert("angle",QJsonValue(angle));
    JSObject.insert("method",QJsonValue(method));
    JSObject.insert("max_speed",QJsonValue(max_speed));
    JSObject.insert("max_wspeed",QJsonValue(max_wspeed));
    JSObject.insert("max_acc",QJsonValue(max_acc));
    JSObject.insert("max_wacc",QJsonValue(max_wacc));
    JSObject.insert("duration",QJsonValue(duration));
    JSObject.insert("orientation",QJsonValue(orientation));
    JSObject.insert("spin",QJsonValue(spin));
    JSObject.insert("delay",QJsonValue(delay));
    JSObject.insert("rot_dir",QJsonValue(rot_dir));
    JSObject.insert("reach_dist",QJsonValue(reach_dist));
    JSObject.insert("reach_angle",QJsonValue(reach_angle));
    JSObject.insert("skill_name",QJsonValue(skill_name));
    */
void robokit::RobotTask::robot_task_gotarget_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(3051,jsonBuffer,std::bind(&robokit::RobotTask::robot_task_gotarget_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotTask::getError(QString msg)
{
    LOGERROR("RobotTask {}",msg.toStdString());
}

void robokit::RobotTask::robot_task_gotarget_req_receive(uint16_t revCommand, QByteArray& receivedata)
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


void robokit::RobotTask::robot_task_translate_req(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson(); 
    m_protocol.WriteTcpData(3055, jsonBuffer, std::bind(&robokit::RobotTask::robot_task_translate_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotTask::robot_task_translate_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug() << "robot_task_translate_req call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}

void robokit::RobotTask::robot_task_turn_req(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(3056, jsonBuffer, std::bind(&robokit::RobotTask::robot_task_turn_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotTask::robot_task_turn_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug() << "robot_task_turn_req call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
}
//    JSObject.insert("id",QJsonValue(id));
//    JSObject.insert("angle",QJsonValue(angle));
//    JSObject.insert("method",QJsonValue(method));
//    JSObject.insert("operation",QJsonValue(operation));
//    JSObject.insert("direction",QJsonValue(direction));
//    JSObject.insert("recognize",QJsonValue(recognize));
//    JSObject.insert("layer",QJsonValue(layer));
//    JSObject.insert("jack_height",QJsonValue(jack_height));
//    JSObject.insert("start_heught",QJsonValue(start_height));
//    JSObject.insert("end_height",QJsonValue(end_height));
//    JSObject.insert("fork_disk",QJsonValue(fork_dist));
//    JSObject.insert("use_pgv",QJsonValue(use_pgv));
//    JSObject.insert("max_speed",QJsonValue(max_speed));
//    JSObject.insert("max_wspeed",QJsonValue(max_wspeed));
//    JSObject.insert("max_acc",QJsonValue(max_acc));
//    JSObject.insert("max_wacc",QJsonValue(max_wacc));
//    JSObject.insert("duration",QJsonValue(duration));
//    JSObject.insert("orientation",QJsonValue(orientation));
//    JSObject.insert("spin",QJsonValue(spin));
//    JSObject.insert("pgv_x_adjust",QJsonValue(pgv_x_adjust));
//    JSObject.insert("pgv_adjust_dist",QJsonValue(pgv_adjust_dist));
//    JSObject.insert("delay",QJsonValue(delay));
//    JSObject.insert("audio_name",QJsonValue(audio_name));
//    JSObject.insert("audio_loop",QJsonValue(audio_loop));
    //TOOD @CaoLi insert peripheral_data not knowenable
    //peripheral_data = JSObject.value("peripheral_data").toArray();
    //JSObject.insert("peripheral",QJsonValue(peripheral_data));
void robokit::RobotTask::robot_task_gotarget2_req_get(const QJsonObject&obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(3052,jsonBuffer,std::bind(&robokit::RobotTask::robot_task_gotarget_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotTask::robot_task_gotarget2_req_receive(uint16_t revCommand, QByteArray& receivedata)
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



//    JSObject.insert("id",QJsonValue(id));
//    JSObject.insert("source_id",QJsonValue(source_id));
//    JSObject.insert("task_id",QJsonValue(task_id));
//    JSObject.insert("angle",QJsonValue(angle));
//    JSObject.insert("method",QJsonValue(method));
//    JSObject.insert("max_speed",QJsonValue(max_speed));
//    JSObject.insert("max_wspeed",QJsonValue(max_wspeed));
//    JSObject.insert("max_acc",QJsonValue(max_acc));
//    JSObject.insert("max_wacc",QJsonValue(max_wacc));
//    JSObject.insert("duration",QJsonValue(duration));
//    JSObject.insert("orientation",QJsonValue(orientation));
//    JSObject.insert("spin",QJsonValue(spin));
//    JSObject.insert("delay",QJsonValue(delay));
//    JSObject.insert("rot_dir",QJsonValue(rot_dir));
//    JSObject.insert("reach_dist",QJsonValue(reach_dist));
//    JSObject.insert("reach_angle",QJsonValue(reach_angle));
//    JSObject.insert("skill_name",QJsonValue(skill_name));
void robokit::RobotTask::robot_task_gotargetlist_req_get(const QJsonObject &obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(3066,jsonBuffer,std::bind(&robokit::RobotTask::robot_task_gotargetlist_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotTask::robot_task_gotargetlist_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    double ret_code = rootObject.value("ret_code").toDouble();
    QString create_on = rootObject.value("create_on").toString();
    QString err_msg = rootObject.value("err_msg").toString();
    LOGDEBUG("{},{},{}",ret_code,create_on.toStdString(),err_msg.toStdString());
}
void robokit::RobotTask::robot_task_pause_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(3001,sendData,std::bind(&robokit::RobotTask::robot_task_pause_req_receive,this,std::placeholders::_1, std::placeholders::_2));

}
void robokit::RobotTask::robot_task_pause_req_receive(uint16_t revCommand, QByteArray& receivedata)
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
void robokit::RobotTask::robot_task_resume_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(3002,sendData,std::bind(&robokit::RobotTask::robot_task_resume_req_receive,this,std::placeholders::_1, std::placeholders::_2));

}
void robokit::RobotTask::robot_task_resume_req_receive(uint16_t revCommand, QByteArray& receivedata)
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


void robokit::RobotTask::robot_task_cancel_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(3003,sendData,std::bind(&robokit::RobotTask::robot_task_cancel_req_receive,this,std::placeholders::_1, std::placeholders::_2));

}
void robokit::RobotTask::robot_task_cancel_req_receive(uint16_t revCommand, QByteArray& receivedata)
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

//JSObject.insert("id",QJsonValue(id));
void robokit::RobotTask::robot_task_target_path_req_get(const QJsonObject&obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(3053,jsonBuffer,std::bind(&robokit::RobotTask::robot_task_target_path_req_receive, this, std::placeholders::_1, std::placeholders::_2));

}
void robokit::RobotTask::robot_task_target_path_req_receive(uint16_t revCommand, QByteArray& receivedata)
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
void robokit::RobotTask::robot_task_cleartargetlist_req_get()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(3067,sendData,std::bind(&robokit::RobotTask::robot_task_cleartargetlist_req_receive,this,std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotTask::robot_task_cleartargetlist_req_receive(uint16_t revCommand, QByteArray& receivedata)
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

