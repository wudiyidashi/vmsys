#ifndef AUTODOOR_H
#define AUTODOOR_H

#include <QObject>
#include "McuBoard.h"

namespace boardDevice {
class Autodoor : public QObject
{
    Q_OBJECT
public:
    explicit Autodoor(QObject *parent = nullptr);
   void init(McuBoard* mcuboard);
   void closeDoor(QString door_index);
   void openDoor(QString door_index);
signals:

private:
    McuBoard* mcuboard;
};
}


#endif // AUTODOOR_H
