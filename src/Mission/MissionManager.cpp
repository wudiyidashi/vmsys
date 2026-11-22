#include "Misc/Log.h"
#include "qjsonarray.h"
#include <Mission/RobotMoveList.h>
#include <Mission/MissionManager.h>
#include "Mission/Jack.h"
#include "Mission/Charge.h"
#include "Robokit/RobokitManager.h"
#include "RobotMove.h"
#include <QUuid>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <IO/TcpSocketClient.h>
#include <Robokit/Map/MapManager.h>


MissionManager &MissionManager::instance()
{
    static MissionManager singleton;
    return singleton;
}
void MissionManager::InitConnection()
{
    tcp_socket_server = new io::TcpSocketServer(10, this);

    if (!tcp_socket_server->listen(QHostAddress::Any, 29204))
    {
        qDebug() << "错误" << tcp_socket_server->errorString();
    }

    connect(tcp_socket_server, &io::TcpSocketServer::sig_connectClient, this, &MissionManager::slot_InCommingNewConnection);
}


void MissionManager::slot_InCommingNewConnection(qint64 socketDescriptor, const QString& ip, quint16 port)//接受新用户连接信息
{
    io::TcpSocketClient* tmpTcp = tcp_socket_server->tcpClient[socketDescriptor];
    robokit::RProtocol* rob_protocol = new robokit::RProtocol(tmpTcp, this);
    //qDebug()<<tmpTcp->peerAddress();
    connect(tmpTcp, &io::TcpSocketClient::disconnected, this, &MissionManager::slot_Disconnected);

    rob_protocol->SetNnalyzeCallBackFunc([this,rob_protocol](uint16_t cmd_type, QByteArray& receivedata) {
        switch (cmd_type)
        {
        case 2003:
            this->addSubTaskFromTcp(rob_protocol,receivedata);
        case 2004:
            this->rerutnCurMapToServer(rob_protocol,receivedata);
        case 2005:
            this->addJackFromTcp(rob_protocol,receivedata);
        default:
            break;
        }
        


        });

}

void MissionManager::rerutnCurMapToServer(robokit::RProtocol* robo_protocol,const QByteArray& json_data)
{
    QJsonObject JSObject;
    QString map_name = SeerrMap::GetInstance().GetCurMapName();
    JSObject.insert("map_name", QJsonValue(map_name));

    RobokitManager::instance().robot_config.robot_config_downloadmap_req_get(JSObject,
        [this,robo_protocol](uint16_t revCommand, QByteArray& receivedata)
        {
            //qDebug() << receivedata;
            robo_protocol->WriteTcpData(2004, receivedata, [](uint16_t revCommand, QByteArray& receivedata) {
                //qDebug() << "receivedata";
                });
        });
}

void MissionManager::slot_Disconnected()
{
//    qDebug() << sender();
}

MissionManager::MissionManager(QObject *parent)
    : QObject{parent}
{
    LOGDEBUG("MissionManager Start");
  
    //定时器触发任务执行
    connect(&timer,&QTimer::timeout,this,&MissionManager::runTask);
    timer.start(1000);
}

//来自Protocol的JSON数据
void MissionManager::addSubTaskFromTcp(robokit::RProtocol* robo_protocol,const QByteArray& json_data)
{
//    qDebug()<<json_data;
    QJsonDocument itemDoc = QJsonDocument::fromJson(json_data);
    QJsonObject rootObject = itemDoc.object();

    subTask::move_list_element_s move_list_element;
    subTask::RobotMoveList* movelist= new subTask::RobotMoveList(nullptr);
    QUuid id = QUuid::createUuid();
    QString task_id=id.toString().remove("{").remove("}").remove("-");
//    qDebug()<<task_id;
    //发布移动任务列表
    QJsonArray move_task_list_obj = rootObject.value("move_task_list").toArray();
    for(int i = 0; i < move_task_list_obj.size(); i++){
        QJsonObject move_task_obj = move_task_list_obj.at(i).toObject();
        QString id = move_task_obj.value("id").toString();
        QString source_id = move_task_obj.value("source_id").toString();
        //QString task_id = move_task_obj.value("task_id").toString();

        move_list_element.id = id;
        move_list_element.source_id=source_id;
        move_list_element.task_id=task_id;

        movelist->station_list.append(move_list_element);

    }
      task_list.emplaceBack(movelist);
    this->runTask();
}

//来自Protocol的JSON数据
void MissionManager::addJackFromTcp(robokit::RProtocol* robo_protocol,const QByteArray& json_data)
{
//    qDebug()<<json_data;
    QJsonDocument itemDoc = QJsonDocument::fromJson(json_data);
    QJsonObject rootObject = itemDoc.object();
    QString operation = rootObject.value("operation").toString();
    bool _jack=false;
    if(operation=="JackHeight")
    {
_jack=false;
    }
    else if(operation=="JackDown")
    {
_jack=true;
    }

    task_list.emplaceBack( new subTask::Jack(_jack));
    this->runTask();
}

int MissionManager::GetTaskCount()
{
    return task_list.count();
}
void MissionManager::addSbuTask( subTask::MissionBase* baseptr)
{
    LOGDEBUG("Internal task");
    task_list.append(baseptr);
}

//手动点击界面发布任务
void MissionManager::publishMoveTask(QString station)
{

    subTask::move_element_s move={
        station,
        "SELF_POSITION",
    };
    task_list.emplaceBack(new subTask::RobotMove(move));

    this->runTask();
}

//手动点击界面充电
void MissionManager::publishChargeTask()
{
    task_list.emplaceBack(new subTask::Charge());
    this->runTask();
}

//引用转发需要定义拷贝
subTask::MissionBase* MissionManager::getNextSubTask()
{
    return task_list.last();
}

void MissionManager::runTask()
{

    //是否可以在执行过程中改变list
    //考虑迭代器迭代
    //或者在这里决定是不是要进入下一步任务

    if(_base_ptr!=nullptr)
    {
        //任务处于结束态，获取下一个任务
        if(_base_ptr->isTaskOver())
        {
            _base_ptr = GetNextTask();
            return;
        }
        _base_ptr->runTask();
        //更新界面信息
        if (_base_ptr)
        {
            subTask::TaskName task = _base_ptr->name;
            switch (task) {
            case subTask::eBase:

                break;
            case subTask::eRobotMove:
            {
                auto task_ptr = dynamic_cast<subTask::RobotMove*>(_base_ptr);
                setGroup_text2_content(task_ptr->goal_station);
            }
            break;
            case subTask::eRobotMoveList:
            {
                auto task_ptr = dynamic_cast<subTask::RobotMoveList*>(_base_ptr);
                //qDebug() << task_ptr->station_list.first().source_id;
                setGroup_text1_content(task_ptr->station_list.first().source_id);
                setMid_content("当前站点"+RobokitManager::instance().robot_status.current_station);
                setGroup_text2_content(task_ptr->station_list.last().id);
            }
            break;
            default:
                break;
            }
        }
       

    }
    else
    {
         _base_ptr = GetNextTask();
    }
}

subTask::MissionBase* MissionManager::GetNextTask()
{
    if (!task_list.isEmpty()) {
        subTask::MissionBase* base_ptr = task_list.takeFirst();
        return base_ptr;
    }
    else
    {
        return nullptr;
    }
}

QString MissionManager::getGroup_text2_content() const
{
    return group_text2_content;
}

void MissionManager::setGroup_text2_content(const QString &newGroup_text2_content)
{
    if (group_text2_content == newGroup_text2_content)
        return;
    group_text2_content = newGroup_text2_content;
    emit group_text2_contentChanged();
}

QString MissionManager::getGroup_text2_title() const
{
    return group_text2_title;
}

void MissionManager::setGroup_text2_title(const QString &newGroup_text2_title)
{
    if (group_text2_title == newGroup_text2_title)
        return;
    group_text2_title = newGroup_text2_title;
    emit group_text2_titleChanged();
}

QString MissionManager::getMid_content() const
{
    return mid_content;
}

void MissionManager::setMid_content(const QString &newMid_content)
{
    if (mid_content == newMid_content)
        return;
    mid_content = newMid_content;
    emit mid_contentChanged();
}

QString MissionManager::getGroup_text1_content() const
{
    return group_text1_content;
}

void MissionManager::setGroup_text1_content(const QString &newGroup_text1_content)
{
    if (group_text1_content == newGroup_text1_content)
        return;
    group_text1_content = newGroup_text1_content;
    emit group_text1_contentChanged();
}

QString MissionManager::getGroup_text1_title() const
{
    return group_text1_title;
}

void MissionManager::setGroup_text1_title(const QString &newGroup_text1_title)
{
    if (group_text1_title == newGroup_text1_title)
        return;
    group_text1_title = newGroup_text1_title;
    emit group_text1_titleChanged();
}

quint8 MissionManager::getTask_status() const
{
    return task_status;
}

void MissionManager::setTask_status(quint8 newTask_status)
{
    if (task_status == newTask_status)
        return;
    task_status = newTask_status;
    emit task_statusChanged();
}

QString  MissionManager::getNewTaskID()
{
    task_ID+=1;
    return QString::number(task_ID,10);
}
