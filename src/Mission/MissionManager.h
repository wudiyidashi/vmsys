#ifndef MISSIONMANAGER_H
#define MISSIONMANAGER_H

#include <QObject>
#include <QTimer>

#include <Robokit/RobokitApi/RobProtocol.h>

#include <Mission/MissionBase.h>
#include "IO/TcpSocketServer.h"
#define ADDMISSIONPTR(base_ptr) MissionManager::instance().addSbuTask(base_ptr)


//
class MissionManager : public QObject
{
    Q_OBJECT
public:
    static MissionManager &instance();
    void InitConnection();


    explicit MissionManager(QObject *parent = nullptr);
   
    void runTask();

    quint8 task_status;//stop 0// runing 1
    QString group_text1_title="起始站点";
    QString group_text1_content="LM15";
    QString mid_content="正在前往LM10";
    QString group_text2_title="目标站点";
    QString group_text2_content="LM10";

    QString getNewTaskID();
    quint8 getTask_status() const;
    void setTask_status(quint8 newTask_status);

    QString getGroup_text1_title() const;
    void setGroup_text1_title(const QString &newGroup_text1_title);

    QString getGroup_text1_content() const;
    void setGroup_text1_content(const QString &newGroup_text1_content);

    QString getMid_content() const;
    void setMid_content(const QString &newMid_content);

    QString getGroup_text2_title() const;
    void setGroup_text2_title(const QString &newGroup_text2_title);

    QString getGroup_text2_content() const;
    void setGroup_text2_content(const QString &newGroup_text2_content);

    subTask::MissionBase *GetNextTask();
    int GetTaskCount();
public slots:
    void addSubTaskFromTcp(robokit::RProtocol* robo_protocol,const QByteArray &json_data);
    void addSbuTask(subTask::MissionBase* baseptr);
    void addJackFromTcp(robokit::RProtocol *robo_protocol, const QByteArray &json_data);
    void publishMoveTask(QString station);
    void publishChargeTask();
private:
    quint32 task_ID=0;
    QList < subTask::MissionBase*> task_list;
    io::TcpSocketServer* tcp_socket_server;
    
    subTask::MissionBase* getNextSubTask();
    subTask::MissionBase* _base_ptr=nullptr;
    QTimer timer;
    void rerutnCurMapToServer(robokit::RProtocol* robo_protocol,const QByteArray &json_data);

    Q_PROPERTY(quint8 task_status READ getTask_status WRITE setTask_status NOTIFY task_statusChanged)

    Q_PROPERTY(QString group_text1_title READ getGroup_text1_title WRITE setGroup_text1_title NOTIFY group_text1_titleChanged)

    Q_PROPERTY(QString group_text1_content READ getGroup_text1_content WRITE setGroup_text1_content NOTIFY group_text1_contentChanged)

    Q_PROPERTY(QString mid_content READ getMid_content WRITE setMid_content NOTIFY mid_contentChanged)

    Q_PROPERTY(QString group_text2_title READ getGroup_text2_title WRITE setGroup_text2_title NOTIFY group_text2_titleChanged)

    Q_PROPERTY(QString group_text2_content READ getGroup_text2_content WRITE setGroup_text2_content NOTIFY group_text2_contentChanged)


signals:

    void task_statusChanged();

    void group_text1_titleChanged();

    void group_text1_contentChanged();

    void mid_contentChanged();

    void group_text2_titleChanged();

    void group_text2_contentChanged();

private slots:
    void slot_InCommingNewConnection(qint64 socketDescriptor, const QString& ip, quint16 port);
    void slot_Disconnected();
//    const subTask::MissionBase &getNextSubTask();

    };

#endif // MISSIONMANAGER_H
