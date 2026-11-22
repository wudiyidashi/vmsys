#ifndef CHARGE_H
#define CHARGE_H

#include "MissionBase.h"
#include "qdatetime.h"

namespace subTask {
enum ChargeStage
{
    ChargeInit,
    ChargeOrientation,
    ChargeTurnAround,
    ChargeBackUp,
    ChargeCharging
};

class Charge : public MissionBase
{
public:
    explicit Charge(QObject *parent = nullptr);
    
    void run();
    void quit();
    void init();


public slots:
    void slot_laser_receive(const QJsonObject &rootObject);
private:
    QTime star_time;
    double robot_star_angle;
    double turn_dir = 0.05;
    const double max_turn_angle = double((15.0 / 180.0 )* 3.1415926);
    bool charge_success = false;
    void robot_translate(double dist, double vx);
    void robot_turn(double angle, double vw);
       void robot_turn_task(double angle, double vw);
       ChargeStage stage=ChargeInit;
       void robot_translate_task(double dist, double vx);
private slots:
    void slot_IR_handle(QString data);
    void slot_charge_state(QByteArray byte);
};
}
#endif // CHARGE_H
