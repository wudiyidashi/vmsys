#include "Charge.h"
#include "Robokit/BoardControl/BoardManager.h"
#include "Robokit/RobokitManager.h"
#include <QJsonArray>
#include <qjsonobject.h>
#include <QtMath>
namespace subTask
{
Charge::Charge(QObject *parent)
    : MissionBase{parent}
{

}
void Charge::slot_IR_handle(QString data)
{
    qDebug() << data;
    //后退
    if (data == "01")
    {
        robot_translate(0.05,-0.02);
    }
    //右转
    else if (data == "03")
    {
        robot_turn(1, -0.05);
    }
    //左转
    else if (data == "02")
    {
        robot_turn(1, 0.05);
    }
    else if (data == "04")
    {
//        if (turn_dir < 0)
//        {
//             robot_turn(1, turn_dir);
//        }
//        else if(turn_dir >0)
//        {
//            if (RobokitManager::instance().robot_status.angle - robot_star_angle > max_turn_angle)
//            {
//                turn_dir = turn_dir * -1;
//                RobokitManager::instance().robot_controls.robot_control_stop_req_get();
//            }
//            robot_turn(1, turn_dir);
//        }
        //qDebug() << RobokitManager::instance().robot_status.angle;
       
    }
}

void Charge::slot_charge_state(QByteArray byte)
{
    if (byte.toInt() == 1)
    {
        RobokitManager& robot =RobokitManager::instance();
        //RobokitManager::instance().robot_controls.robot_control_stop_req_get();
        robot.robot_task.robot_task_cancel_req_get();
        charge_success = true;
        robot.robot_controls.robot_control_stop_req_get();
        boardDevice::McuBoard& McuBoard = robot.board_manager.mcu_board;
        disconnect(&McuBoard, &boardDevice::McuBoard::red_line, this, &Charge::slot_IR_handle);
        boardDevice::Bat& Bat = robot.board_manager.bat;
        disconnect(&Bat, &boardDevice::Bat::bat_charge_stat, this, &Charge::slot_charge_state);

        changeTaskStatus(onQuit);
    }
}
//直线运动
/**
dist	number	直线运动距离, 绝对值, 单位: m	否
vx	   number	机器人坐标系下 X 方向运动的速度, 正为向前, 负为向后, 单位: m/s
*/
void Charge::robot_translate(double dist,double vx)
{
    QJsonObject JSObject;
   /* JSObject.insert("dist", QJsonValue(dist));
    JSObject.insert("vx", QJsonValue(vx));

    RobokitManager::instance().robot_task.robot_task_translate_req(JSObject);*/

 
    JSObject.insert("vx", QJsonValue(vx));
    RobokitManager::instance().robot_controls.robot_control_motion_req_get(JSObject);
}

void Charge::robot_translate_task(double dist,double vx)
{
    QJsonObject JSObject;
    JSObject.insert("dist", QJsonValue(dist));
    JSObject.insert("vx", QJsonValue(vx));

    RobokitManager::instance().robot_task.robot_task_translate_req(JSObject);

}
//旋转
/**
 angle	number	转动的角度(机器人坐标系), 绝对值, 单位 rad, 可以大于 2π	否
  vw	number	转动的角速度(机器人坐标系), 正为逆时针转, 负为顺时针转 单位 rad/s	否
*/
void Charge::robot_turn(double angle, double vw)
{
    QJsonObject JSObject;

    //JSObject.insert("angle", QJsonValue(angle));
    //JSObject.insert("vw", QJsonValue(vw));
    //RobokitManager::instance().robot_task.robot_task_turn_req(JSObject);

    JSObject.insert("w", QJsonValue(vw));
    
    RobokitManager::instance().robot_controls.robot_control_motion_req_get(JSObject);
}

void Charge::robot_turn_task(double angle, double vw)
{
    QJsonObject JSObject;

    JSObject.insert("angle", QJsonValue(angle));
    JSObject.insert("vw", QJsonValue(vw));
    RobokitManager::instance().robot_task.robot_task_turn_req(JSObject);
}

void Charge::slot_laser_receive(const QJsonObject& rootObject)
{

    //激光点云数据
    QJsonArray lasers= rootObject.value("lasers").toArray();
    QJsonArray arrayValue = lasers.at(0).toObject().value("beams").toArray();
    double angle1=0,angle2=0,dist1=0,dist2=0;
    int angle1_index=0,angle2_index=0;
    int last_rssi;
    QList<double> dist_list;
    for(int i = 0; i < arrayValue.size(); i++){
        QJsonValue beeams_arrayValue = arrayValue.at(i);
        QJsonObject beams_obj =  beeams_arrayValue.toObject();
        double angle =beams_obj.value("angle").toDouble();
        double dist = beams_obj.value("dist").toDouble();
        dist_list.append(dist);
        double rssi = beams_obj.value("rssi").toDouble();
        if(last_rssi>150&&rssi<150&&angle1==0)
        {
            angle1=angle;dist1=dist;
            angle1_index =i;
        }
        if(angle1!=0)
        {
            if(rssi>150&&last_rssi<150)
            {
                 angle2=angle;dist2=dist;
                 angle2_index =i;
            }
        }
        last_rssi=rssi;
        //qDebug() << "Array" << i << ": " << angle<<": " << dist<<": " << rssi;
    }
    if((90-angle1)-(angle2-90)>1.2)
    {
        //left
        robot_turn(1, -0.02);
    }
    else if((90-angle1)-(angle2-90)<-1.2)
    {
        robot_turn(1, 0.02);
    }
    else if(dist_list[(angle2_index+angle1_index)/2]<0.48)
    {
        auto robot_status = &RobokitManager::instance().robot_status;
        disconnect(robot_status,&robokit::RobotStatus::singal_sendToServerStatus,this,&Charge::slot_laser_receive);
        robot_turn_task(qDegreesToRadians(180),0.6);
        stage=ChargeTurnAround;
    }
    else
    {
        robot_translate(0.05,0.02);
    }
}
void Charge::init()
{

    auto robot_status = &RobokitManager::instance().robot_status;

    connect(robot_status,&robokit::RobotStatus::singal_sendToServerStatus,this,&Charge::slot_laser_receive);

    RobokitManager& RobokitManagerd = RobokitManager::instance();
    robot_star_angle = RobokitManager::instance().robot_status.angle;
    star_time = QTime::currentTime();
    boardDevice::McuBoard& McuBoard = RobokitManagerd.board_manager.mcu_board;
    boardDevice::Bat& Bat = RobokitManagerd.board_manager.bat;
    //connect(&McuBoard, &boardDevice::McuBoard::red_line, this, &Charge::slot_IR_handle);
    connect(&Bat, &boardDevice::Bat::bat_charge_stat, this, &Charge::slot_charge_state);
    stage = ChargeOrientation;
    changeTaskStatus(onRun);
}

void Charge::run()
{
    auto robot_status = &RobokitManager::instance().robot_status;
    switch (stage) {
    case ChargeTurnAround:
        if(robot_status->task_type==7&&robot_status->task_status==4)
        {
            robot_translate_task(0.3,-0.05);
            stage=ChargeBackUp;
        }

        break;
    case ChargeBackUp:

        break;
    default:
        break;
    }
if(star_time.secsTo(QTime::currentTime())>40 || charge_success == true)
{
    auto robot_status = &RobokitManager::instance().robot_status;
    disconnect(robot_status,&robokit::RobotStatus::singal_sendToServerStatus,this,&Charge::slot_laser_receive);

    RobokitManager::instance().robot_controls.robot_control_stop_req_get();
    RobokitManager& RobokitManagerd = RobokitManager::instance();
    boardDevice::McuBoard& McuBoard = RobokitManagerd.board_manager.mcu_board;
    disconnect(&McuBoard, &boardDevice::McuBoard::red_line, this, &Charge::slot_IR_handle);
    boardDevice::Bat& Bat = RobokitManagerd.board_manager.bat;
    disconnect(&Bat, &boardDevice::Bat::bat_charge_stat, this, &Charge::slot_charge_state);
    changeTaskStatus(onQuit);
}
}

void Charge::quit()
{
 changeTaskStatus(onNext);
}
}
