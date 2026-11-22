#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include "McuBoard.h"
#include "Battery.h"
#include "Lift.h"
#include "Autodoor.h"

#include <QObject>
namespace boardDevice {
    class BoardManager : public QObject
    {
        Q_OBJECT
    public:
        boardDevice::McuBoard mcu_board;
        boardDevice::Bat bat;
        boardDevice::Lift lift;
        boardDevice::Autodoor auto_door;
    public:
        explicit BoardManager(QObject *parent = nullptr);

        bool initBoardDevice();
    signals:

    private:
        BoardManager(BoardManager &&) = delete;
        BoardManager(const BoardManager &) = delete;
        BoardManager &operator=(BoardManager &&) = delete;
        BoardManager &operator=(const BoardManager &) = delete;

    };
}


#endif // BOARDMANAGER_H
