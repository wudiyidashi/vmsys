#ifndef MISSIONBASE_H
#define MISSIONBASE_H

#include <QMutex>
#include <QObject>

namespace subTask {
enum TaskStatus{
    onInit,
    onRun,
    onQuit,
    onNext
};

enum TaskName{
    eBase,
    eRobotMove,
    eRobotMoveList,
};
//作为任务类的基类
class MissionBase : public QObject
{
    Q_OBJECT
public:
    explicit MissionBase(QObject *parent = nullptr);

    MissionBase(const subTask::MissionBase &base);
    MissionBase &operator=(const subTask::MissionBase &);
    virtual ~MissionBase();

    TaskName name=eBase;
    QString task_ID;


    //切换任务状态
    void changeTaskStatus(TaskStatus status);

    virtual void init()=0;
    virtual void run()=0;
    virtual void quit()=0;
    void runTask();
    bool isTaskOver();
signals:

private:
    TaskStatus status=onInit;

};
}


#endif // MISSIONBASE_H
