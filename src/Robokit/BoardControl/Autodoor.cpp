#include "Autodoor.h"
namespace boardDevice {
Autodoor::Autodoor(QObject *parent)
    : QObject{parent}
{

}
void Autodoor::init(McuBoard* mcuboard)
{
    this->mcuboard = mcuboard;
}
//0为关闭自动门
void Autodoor::closeDoor(QString door_index)
{
    QString autoDoor_id = QString("%02").arg(door_index.toInt(),2,16,QLatin1Char('0'));
    mcuboard->DoorControl(QByteArray::fromHex("05020A"+autoDoor_id.toUtf8()+"02"));
}

//0为关闭自动门
void Autodoor::openDoor(QString door_index)
{
    QString autoDoor_id = QString("%02").arg(door_index.toInt(),2,16,QLatin1Char('0'));
    mcuboard->DoorControl(QByteArray::fromHex("05020A"+autoDoor_id.toUtf8()+"01"));
}

}

