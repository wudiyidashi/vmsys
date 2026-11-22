/**
 * @file BoardManager.cpp
 * @author 岳凯 (you@domain.com)
 * @brief 所有板级设备的连接、控制接口管理
 * @version 0.1
 * @date 2022-11-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "BoardManager.h"

boardDevice::BoardManager::BoardManager(QObject *parent)
    : QObject{parent}
{}

bool boardDevice::BoardManager::initBoardDevice()
{
    //mcu_board.init();
    //bat.init();
    //bat.startTimerReq();
    //auto_door.init(&mcu_board);
    //lift.init();
    return true;
}
