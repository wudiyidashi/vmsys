#include "RobotStatus.h"
#include "Misc/Log.h"
#include "qjsonarray.h"
#include "Misc/Music.h"

#include <QJsonDocument>
#include <QJsonObject>

robokit::RobotStatus::RobotStatus()

{
#if 0
    setDefaultAddress("192.168.3.29");
    setDefaultTcpPort(19204);

#elif 1
    setDefaultAddress("192.168.192.5");
    setDefaultTcpPort(19204);

#endif
    connect(&m_protocol,&robokit::RProtocol::errorMessage,this,&robokit::RobotStatus::getError);
    connect(&m_protocol, &robokit::RProtocol::stateChange, this, &robokit::RobotStatus::slot_connectionStateChange);
    connect(this, &RobotBase::connectionStateChanged, this, [this] {
        if (!getConnectionState()) {
            statusSound = StatusSound::None;
            Music::GetInstance().stopmusic();
            mapReportedInStatus = false;
            if (!m_diagnostics.isEmpty()) {
                m_diagnostics.clear();
                emit diagnosticsChanged();
            }
        }
    });

    set_default_all1_req_obj(false);
}

bool robokit::RobotStatus::initConnection()
{
    if(m_protocol.initTcpConnection(QHostAddress(defaultAddress),defaultTcpPort))
    {

        return true;
    }
    return false;
}
bool robokit::RobotStatus::checkConnection()
{
    return m_protocol.getProtocolState()!=-1 ? false : true ;
}


void robokit::RobotStatus::setDefaultAddress(const QString &newDefaultAddress)
{
    if (defaultAddress == newDefaultAddress)
        return;
    defaultAddress = newDefaultAddress;
    emit robotAddressChanged();
}

void robokit::RobotStatus::setDefaultTcpPort(quint16 newDefaultTcpPort)
{
    defaultTcpPort = newDefaultTcpPort;
}

void robokit::RobotStatus::_SendDataForTest()
{
    QByteArray sendData;
    m_protocol.WriteTcpData(1000,sendData,std::bind(&::robokit::RobotStatus::robot_status_info_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotStatus::getError(QString msg)
{
    LOGERROR("RobotStatus {}",msg.toStdString());
}

//查询机器人信息
void robokit::RobotStatus::robot_status_info_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1000,sendData,std::bind(&::robokit::RobotStatus::robot_status_info_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::robot_status_info_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    // print out the list of keys ("count")
    QStringList keys = rootObject.keys();
    //机器人 id
    QString id=rootObject.value("id").toString();
    //机器人名称
    QString vehicle_id=rootObject.value("vehicle_id").toString();
    //机器人备注
    QString robot_note=rootObject.value("robot_note").toString();
    //Robokit版本号
    QString version=rootObject.value("version").toString();
    //机器人模型名
    QString model=rootObject.value("model").toString();
    //固件版本号
    QString dsp_version=rootObject.value("dsp_version").toString();
    //陀螺仪版本号
    QString gyro_version=rootObject.value("gyro_version").toString();
    //地图版本号
    QString map_version=rootObject.value("map_version").toString();
    //模型版本号
    QString model_version=rootObject.value("model_version").toString();
    //网络协议版本号
    QString netprotocol_version=rootObject.value("netprotocol_version").toString();
    //ModbusTCP 协议版本号
    QString modbus_version=rootObject.value("modbus_version").toString();
    //当前地图名
    QString current_map=rootObject.value("current_map").toString();
    //当前地图 MD5 值
    QString current_map_md5=rootObject.value("current_map_md5").toString();
    //当前模型 MD5 值
    QString model_md5=rootObject.value("model_md5").toString();
    //当前连接 Wifi 的 SSID（机器人需要支持连接 Wifi 并且已经连接，否则为空）
    QString ssid=rootObject.value("ssid").toString();
    //当前连接 Wifi 的信号强度，0-100 百分比（机器人需要支持连接 Wifi 并且已经连接，否则为0）
    double rssi=rootObject.value("rssi").toDouble();
    //当前连接的AP的mac地址
    QString ap_addr=rootObject.value("ap_addr").toString();
    //当前 IP（除 192.168.192.5 外的另一个 IP 地址）
    QString current_ip=rootObject.value("current_ip").toString();
    //机器人无线客户端专用以太网接口的 MAC 地址（仅对 SRC-2000 有效），格式为 XXXXXXXXXXXX，中间没有分隔符
    QString MAC=rootObject.value("MAC").toString();
    //机器人机器码类型 (用于激活，通常为 0x1800)
    QString echoid_type=rootObject.value("echoid_type").toString();
    //机器人机器码
    QString echoid=rootObject.value("echoidechoid_type").toString();
    //机器人功能模块   name = 功能模块名称   active = 是否已激活   expiry_date = 到期日期(可缺省)

    //    TODO @秦赛飞 等待测试JSONarray内容输出
    QJsonArray features = rootObject.value("features").toArray();
    for(int i = 0; i < features.size(); i++){
        QJsonValue arrayValue = features.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人运行信息
void robokit::RobotStatus::robot_status_run_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1002,sendData,std::bind(&::robokit::RobotStatus::robot_status_run_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}

void robokit::RobotStatus::robot_status_run_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();

    //今日累计行驶里程, 单位 m
    double today_odoe=rootObject.value("today_odo").toDouble();
    //本次运行时间(开机后到当前的时间), 单位 ms
    double time=rootObject.value("time").toDouble();
    //累计运行时间, 单位 ms
    double total_time=rootObject.value("total_time").toDouble();
    //控制器温度, 单位 ℃
    double controller_temp=rootObject.value("controller_temp").toDouble();
    //控制器湿度, 单位 %
    double controller_humi=rootObject.value("controller_humi").toDouble();
    //控制器电压, 单位 V
    double controller_voltage=rootObject.value("controller_voltage").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    double create_on=rootObject.value("create_on").toDouble();
    //错误信息
    double err_msg=rootObject.value("err_msg").toDouble();

}


//查询机器人位置
void robokit::RobotStatus::robot_status_loc_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1004,sendData,std::bind(&::robokit::RobotStatus::robot_status_loc_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::robot_status_loc_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();


    //机器人的 x 坐标, 单位 m
    double x=rootObject.value("x").toDouble();
    //机器人的 y 坐标, 单位 m
    double y=rootObject.value("y").toDouble();
    //机器人的 angle 坐标, 单位 rad
    double angle=rootObject.value("angle").toDouble();
    //机器人的定位置信度, 范围 [0, 1]
    double confidence=rootObject.value("confidence").toDouble();
    //离机器人最近站点的 id
    QString current_station=rootObject.value("current_station").toString();
    //机器人上一个所在站点的 id
    QString last_station=rootObject.value("last_station").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询机器人速度
void robokit::RobotStatus::robot_status_speed_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1005,sendData,std::bind(&::robokit::RobotStatus::robot_status_speed_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::robot_status_speed_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //机器人在机器人坐标系的 x 方向实际的速度, 单位 m/s
    double vx=rootObject.value("vx").toDouble();
    //机器人在机器人坐标系的 y 方向实际的速度, 单位 m/s
    double vy=rootObject.value("vy").toDouble();
    //机器人在机器人坐标系的实际的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double w=rootObject.value("w").toDouble();
    //单舵轮机器人当前的舵轮角度, 单位 rad
    double steer=rootObject.value("steer").toDouble();
    //机器人的托盘（如果存在）角度, 单位 rad
    double spin=rootObject.value("spin").toDouble();
    //机器人在机器人坐标系的 x 方向接收到的速度, 单位 m/s
    double r_vx=rootObject.value("r_vx").toDouble();
    //机器人在机器人坐标系的 y 方向收到的速度, 单位 m/s
    double r_vy=rootObject.value("r_vy").toDouble();
    //机器人在机器人坐标系的收到的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double r_w=rootObject.value("r_w").toDouble();
    //单舵轮机器人收到的舵轮角度, 单位 rad
    double r_steer=rootObject.value("r_steer").toDouble();
    //托盘机器人的收到托盘（如果存在）转动速度, 单位 rad/s
    double r_spinr=rootObject.value("r_spin").toDouble();
    //多舵轮机器人当前的舵角角度, 双舵轮=[前舵轮角度，后舵轮角度], 单位 rad
    QJsonArray steer_angles = rootObject.value("steer_angles").toArray();
    //多舵轮机器人收到的舵角角度, 双舵轮=[前舵轮角度，后舵轮角度], 单位 rad
    QJsonArray r_steer_angles = rootObject.value("r_steer_angles").toArray();
    //机器人底盘是否静止（行走电机）
    bool is_stop=rootObject.value("is_stop").toBool();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}

//查询机器人的被阻挡状态
void robokit::RobotStatus::robot_status_block_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1006,sendData,std::bind(&::robokit::RobotStatus::robot_status_block_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::robot_status_block_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //机器人是否被阻挡
    bool blocked=rootObject.value("blocked").toBool();
    //被阻挡的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 =  Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机，9 = 距离传感器，10 = DI 超声
    double block_reason=rootObject.value("block_reason").toDouble();
    //最近障碍物位置的 x 坐标, 单位 m
    double block_x=rootObject.value("block_x").toDouble();
    //最近障碍物位置的 y 坐标, 单位 m
    double block_y=rootObject.value("block_y").toDouble();
    //发生阻挡的 id 号,  仅当 block_reason = 0、2、3、4、9、10 时有意义3.2.8.0 新增
    double block_id=rootObject.value("block_id").toDouble();
    //机器人是否减速
    bool slowed=rootObject.value("slowed").toBool();
    //减速的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 =  Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机，9 = 距离传感器，10 = DI 超声
    double slow_reason=rootObject.value("slow_reason").toDouble();
    //发生减速的 id 号,  仅当 slow_reason = 0、2、3、4、9、10 时有意义3.2.8.0 新增
    double slow_id=rootObject.value("slow_id").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}

//查询机器人电池状态

//是否只返回简单数据, true = 是，false = 否，缺省则为否
//JSObject.insert("simple",QJsonValue(simple));
void robokit::RobotStatus:: robot_status_battery_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;




    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1007,sendData,std::bind(&::robokit::RobotStatus::robot_status_battery_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_battery_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
    QJsonObject JSObject;

    //机器人电池电量, 范围 [0, 1]
    double battery_level=rootObject.value("battery_level").toDouble();
    //机器人电池温度, 单位 ℃
    double battery_temp=rootObject.value("battery_temp").toDouble();
    //电池是否正在充电
    bool charging=rootObject.value("charging").toBool();
    //电压, 单位 V
    double voltage=rootObject.value("voltage").toDouble();
    //电流, 单位 A
    double current=rootObject.value("current").toDouble();
    //允许充电的最大电压( -1 = 该电池不支持此功能), 单位 V
    double max_charge_voltage=rootObject.value("max_charge_voltage").toDouble();
    //允许充电的最大电流( -1 = 该电池不支持此功能), 单位 A
    double max_charge_current=rootObject.value("max_charge_current").toDouble();
    //仅指示机器人是否连接手动充电器，不保证电源是否接通，连接手动充电器时不能运动(仅 SRC-2000 支持)
    bool manual_charge=rootObject.value("manual_charge").toBool();
    //仅指示机器人是否连接自动充电桩，不保证电源是否接通(仅 SRC-2000 支持)
    bool auto_charge=rootObject.value("auto_charge").toBool();
    //电池循环次数(从电池 BMS 获取，不保证正确)
    double battery_cycle=rootObject.value("battery_cycle").toDouble();
    //电池用户自定义数据
    QString battery_user_data=rootObject.value("battery_user_data").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();


}

//查询电机状态信息


//根据电机名称查询指定电机状态信息,如果缺省该字段表示查询所有电机状态信息  *****
//JSObject.insert("motor_names",QJsonValue(motor_names));
void robokit::RobotStatus:: robot_status_motor_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1040,sendData,std::bind(&::robokit::RobotStatus::robot_status_motor_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_motor_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //电机名称
    QString motor_name=rootObject.value("motor_name").toString();
    //电机类型
    //0=WALK 1=SPEED 2=SPIN 3=LINEAR 4=ROTATION 5=DO
    double type=rootObject.value("type").toDouble();
    //对应模型文件的 canPort
    double can_router=rootObject.value("can_router").toDouble();
    //对应模型文件的 canId
    double can_id=rootObject.value("can_id").toDouble();
    //位置，单位: (linear 为 m; steer、spin、rotation 为 rad)
    double position=rootObject.value("position").toDouble();
    //速度，单位: (linear 为 m/s; steer、spin、rotation 为 rad/s)
    double speed=rootObject.value("speed").toDouble();
    //电流, 单位 A
    double current=rootObject.value("current").toDouble();
    //电压, 单位 V
    double voltage=rootObject.value("voltage").toDouble();
    //是否停止
    bool stop=rootObject.value("stop").toBool();
    //报错码
    double error_code=rootObject.value("error_code").toDouble();
    //是否报错
    double err=rootObject.value("err").toDouble();
    //是否急停
    bool emc=rootObject.value("emc").toBool();
    //温度
    double temperature=rootObject.value("temperature").toDouble();
    //编码器
    double encoder=rootObject.value("encoder").toDouble();
    //是否为被动电机(指不执行，仅反馈的电机)
    bool passive=rootObject.value("emc").toBool();
    //是否标零完成
    bool calib=rootObject.value("emc").toBool();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}


//查询机器人激光点云数据

//是否返回多线激光数据，true = 返回，false = 不返回，缺省则为不返回
//JSObject.insert("return_beams3D",QJsonValue(return_beams3D));
void robokit::RobotStatus:: robot_status_laser_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();



    m_protocol.WriteTcpData(1009,sendData,std::bind(&::robokit::RobotStatus::robot_status_area_res_receive, this, std::placeholders::_1, std::placeholders::_2));


}
void robokit::RobotStatus:: robot_status_area_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
    //激光点云数据
    QJsonArray lasers = rootObject.value("lasers").toArray();
    for(int i = 0; i < lasers.size(); i++){
        QJsonValue arrayValue = lasers.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}


//查询机器人当前所在区域
void robokit::RobotStatus:: robot_status_area_req_get()
{
    QByteArray sendData;

    m_protocol.WriteTcpData(1011,sendData,std::bind(&::robokit::RobotStatus::robot_status_laser_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_laser_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //机器人所在区域 id 的数组(由于地图上的区域是可以重叠的, 所以机器人可能同时在多个区域)，数组可能为空
    QJsonArray area_ids = rootObject.value("area_ids").toArray();
    for(int i = 0; i < area_ids.size(); i++){
        QJsonValue arrayValue = area_ids.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}


//查询机器人急停状态
void robokit::RobotStatus:: robot_status_emergency_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1012,sendData,std::bind(&robokit::RobotStatus:: robot_status_emergency_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_emergency_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //表示急停按钮处于激活状态(按下), false 表示急停按钮处于非激活状态(拔起)
    bool emergency=rootObject.value("emergency").toBool();
    //true表示电机驱动器发生急停, false 表示电机驱动器发生未急停
    bool driver_emc=rootObject.value("driver_emc").toBool();
    //true 表示继电器开启, false 表示继电器关闭
    bool electric=rootObject.value("electric").toBool();
    //true 表示电机驱动器发生软急停, false 表示电机驱动器未发生软急停
    bool soft_emc=rootObject.value("soft_emc").toBool();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}



//查询机器人 I/O 数据
void robokit::RobotStatus:: robot_status_io_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData(1013,sendData,std::bind(&::robokit::RobotStatus::robot_status_io_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::  robot_status_io_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //id: 对应的 id 号  source: 来源 normal = 普通 DI virtual = 虚拟 DI modbus = modbus DI
    //status: 表示高低电平
    //  true = 高电平
    //  false = 低电平
    //valid: 对应 DI 是否启用
    //  true = 启用
    //  false = 禁用
    QJsonArray DO= rootObject.value("DO").toArray();
    for(int i = 0; i < DO.size(); i++){
        QJsonValue arrayValue = DO.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}

//查询机器人 IMU 数据
void robokit::RobotStatus:: robot_status_imu_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1014,sendData,std::bind(&robokit::RobotStatus::robot_status_imu_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_imu_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //偏航角，单位：rad
    double yaw=rootObject.value("yaw").toDouble();
    //滚转角，单位：rad
    double roll=rootObject.value("roll").toDouble();
    //俯仰角，单位：rad
    double pitch=rootObject.value("pitch").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}



//查询 RFID 数据
void robokit::RobotStatus:: robot_status_rfid_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData(1015 ,sendData,std::bind(&::robokit::RobotStatus::robot_status_rfid_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_rfid_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //扫描到的所有 RFID 标签 id,  如果没扫描到 RFID 标签, 则为空数组
    QJsonArray rfids= rootObject.value("rfids").toArray();
    for(int i = 0; i < rfids.size(); i++){
        QJsonValue arrayValue = rfids.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}




//查询机器人的超声传感器数据
void robokit::RobotStatus:: robot_status_ultrasonic_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData(1016,sendData,std::bind(&robokit::RobotStatus::robot_status_ultrasonic_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_ultrasonic_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //扫超声节点数据, 数据示例见下文
    QJsonArray ultrasonic_nodes= rootObject.value("rfids").toArray();
    for(int i = 0; i < ultrasonic_nodes.size(); i++){
        QJsonValue arrayValue =ultrasonic_nodes.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}



//查询二维码数据(PGV)
void robokit::RobotStatus:: robot_status_pgv_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData( 1017,sendData,std::bind(&robokit::RobotStatus::robot_status_pgv_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_pgv_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
    //扫超声节点数据, 数据示例见下文
    QJsonArray pgvs= rootObject.value("rfids").toArray();
    for(int i = 0; i < pgvs.size(); i++){
        QJsonValue arrayValue = pgvs.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}


//查询编码器脉冲值
void robokit::RobotStatus:: robot_status_encoder_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1018,sendData,std::bind(&robokit::RobotStatus::robot_status_encoder_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_encoder_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    QJsonArray motor_encoder= rootObject.value("motor_encoder").toArray();
    for(int i = 0; i < motor_encoder.size(); i++){
        QJsonValue arrayValue = motor_encoder.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人导航状态
void robokit::RobotStatus:: robot_status_task_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData(1020,sendData,std::bind(&robokit::RobotStatus::robot_status_task_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_task_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
}



//查询机器人任务状态

//根据电机名称查询指定电机状态信息,如果缺省该字段表示查询所有电机状态信息  *****
//JSObject.insert("motor_names",QJsonValue(motor_names));
void robokit::RobotStatus:: robot_status_task_status_package_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    

    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1110,sendData,std::bind(&robokit::RobotStatus::robot_status_task_status_package_res, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_task_status_package_res(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //一定直线距离内（该距离是rbk内的一个参数），离机器人最近的站点。可能为空串，表示机器人周围没有站点
    QString source_name=rootObject.value("source_name").toString();
    //机器人当前正执行的导航任务中的 "source_id"
    QString closest_target=rootObject.value("closest_target").toString();
    //机器人当前正执行的导航任务中的 "id"
    QString target_name=rootObject.value("target_name").toString();
    //机器人当前正执行的导航任务中，对于相应的路径，机器人已经走完的部分，占整条路径的比例
    double percentage=rootObject.value("percentage").toDouble();
    //机器人 到 “当前正执行的导航任务所对应的路径” 的投影距离
    double distance=rootObject.value("distance").toDouble();
    //任务状态列表（字段说明详见下表）
    QJsonArray task_status_list= rootObject.value("task_status_list").toArray();
    for(int i = 0; i < task_status_list.size(); i++){
        QJsonValue arrayValue = task_status_list.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //机器人在导航过程中，对使用者的一些提示，可以将其输出到前台。该字段不参与实际的逻辑判断
    QString info=rootObject.value("info").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}




//查询机器人定位状态
void robokit::RobotStatus:: robot_status_reloc_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1021,sendData,std::bind(&robokit::RobotStatus::robot_status_reloc_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_reloc_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //0 = FAILED(定位失败), 1 = SUCCESS(定位正确), 2 = RELOCING(正在重定位), 3=COMPLETED(定位完成)
    double reloc_status=rootObject.value("reloc_status").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询机器人地图载入状态
void robokit::RobotStatus::robot_status_loadmap_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1022,sendData,std::bind(&robokit::RobotStatus::robot_status_loadmap_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_loadmap_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //	0 = FAILED(载入地图失败), 1 = SUCCESS(载入地图成功), 2 = LOADING(正在载入地图)
    double loadmap_status=rootObject.value("loadmap_status").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人扫图状态
void robokit::RobotStatus::robot_status_slam_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1025,sendData,std::bind(&::robokit::RobotStatus::robot_status_slam_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_slam_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //0 = 没有扫图, 1 = 正在扫图(离线), 2 = 正在实时扫图(SLAM)，3 = 正在3D扫图（离线），4 = 正在实时3D扫图
    double slam_status=rootObject.value("slam_status").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询顶升机构状态
void robokit::RobotStatus::robot_status_jack_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1027,jsonBuffer,std::bind(&robokit::RobotStatus::robot_status_jack_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_jack_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //运行模式, false = 手动, true = 自动
    bool jack_mode=rootObject.value("jack_mode").toBool();
    //运行状态,  0x00 = 上升中，0x01 = 上升到位，0x02 = 下降中，0x03 = 下降到位，0x04 = 停止，0xFF = 执行失败
    double jack_state=rootObject.value("jack_state").toDouble();
    //顶升是否启用, false = 未启用, true = 启用
    bool jack_enable=rootObject.value("jack_enable").toBool();
    //顶升错误码
    double jack_speed=rootObject.value("jack_emc").toDouble();
    //顶升机构是否急停, false = 未急停,  true = 急停
    bool jack_emc=rootObject.value("jack_mode").toBool();
    //顶升当前的高度，单位: m
    double jack_height=rootObject.value("jack_height").toDouble();
    //顶升机构上是否有料, false = 无料, true = 有料
    bool jack_isFull=rootObject.value("ret_code").toBool();
    //外设中的用户自定义数据
    QJsonArray peripheral_data= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < peripheral_data.size(); i++){
        QJsonValue arrayValue = peripheral_data.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
}


//查询货叉(叉车)状态
void robokit::RobotStatus::robot_status_fork_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1025,sendData,std::bind(&::robokit::RobotStatus::robot_status_fork_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_fork_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //	货叉高度, 单位 m
    double fork_height=rootObject.value("fork_height").toDouble();
    //货叉高度是否到位, true = 到位, false = 未到位（弃用，目前该位随机器人任务状态而变化；当机器人的任务状态为完成时为true，当机器人运动时为false）
    bool fork_height_in_place=rootObject.value("fork_height_in_place").toBool();
    //叉车的控制模式(通过叉车上的物理按钮切换), ture = 自动控制(控制器控制), false = 手动控制(方向盘驾驶)
    bool fork_auto_flag=rootObject.value("fork_auto_flag").toBool();
    //货叉前移后退距离, 单位: m
    double forward_val=rootObject.value("forward_val").toDouble();
    //货叉前移后退是否到位, true = 到位, false = 未到位
    bool forward_in_place=rootObject.value("forward_in_place").toBool();
    //货叉称重数据，单位：kg
    double fork_pressure_actual=rootObject.value("fork_pressure_actual").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询辊筒（皮带）状态
void robokit::RobotStatus::robot_status_roller_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1029,sendData,std::bind(&robokit::RobotStatus::robot_status_roller_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::  robot_status_roller_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //运行模式, false = 手动, true = 自动
    bool roller_mode=rootObject.value("roller_mode").toBool();
    //	运行状态, 0x01 = 待机, 0x02 = 执行中, 0x03 = 执行完成, 0x04 = 停止, 0xFF = 执行失败
    double roller_state=rootObject.value("roller_state").toDouble();
    //辊筒上是否有料, false = 无料, true = 有料
    bool roller_isFull=rootObject.value("roller_isFull").toBool();
    //辊筒电机速率
    double roller_speed=rootObject.value("roller_speed").toDouble();
    //辊筒是否急停, false = 未急停, true = 急停
    bool roller_emc=rootObject.value("roller_emc").toBool();
    //外设中的用户自定义数据
    QJsonArray peripheral_data= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < peripheral_data.size(); i++){
        QJsonValue arrayValue = peripheral_data.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人当前的调度状态
void robokit::RobotStatus:: robot_status_dispatch_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData( 1030 ,sendData,std::bind(&robokit::RobotStatus::robot_status_dispatch_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_dispatch_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //0 = 单机模式, 1 = 调度模式
    double dispatch_mode=rootObject.value("dispatch_mode").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询机器人报警状态
void robokit::RobotStatus::robot_status_alarm_rep_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1050,jsonBuffer,std::bind(&::robokit::RobotStatus::robot_status_alarm_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_alarm_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //报警码 Fatal的数组, 所有出现的 Fatal 报警都会出现在数据中, object 格式见下文
    QJsonArray fatals= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < fatals.size(); i++){
        QJsonValue arrayValue = fatals.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //报警码 Error的数组, 所有出现的 Error 报警都会出现在数据中, object 格式见下文
    QJsonArray errors= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < errors.size(); i++){
        QJsonValue arrayValue = errors.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //报警码 Warning的数组, 所有出现的 Warning 报警都会出现在数据中, object 格式见下文
    QJsonArray warnings= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < warnings.size(); i++){
        QJsonValue arrayValue = warnings.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //Notice的数组, 所有出现的 Notice 报警都会出现在数据中, object 格式见下文
    QJsonArray notices= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < notices.size(); i++){
        QJsonValue arrayValue = notices.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//设置默认批量数据请求体
void robokit::RobotStatus::set_default_all1_req_obj(bool return_laser)
{
    QJsonObject rootobj;
    QJsonArray keys_array={
        "odo",
        "today_odo",
        "time",
        "total_time",
        "x",
        "y",
        "angle",
        "confidence",
        "current_station",
        "last_station",
        "is_stop",
        "blocked",
        "block_reason",
        "slowed",
        "slow_reason",
        "lasers",
        "task_status",
        "task_type",
        "target_id",
        "reloc_status",
        "dispatch_mode",
    };
   
    if (requestMapMetadata) {
        keys_array.append(QStringLiteral("current_map"));
        keys_array.append(QStringLiteral("current_map_md5"));
    }
    rootobj.insert("keys",keys_array);
    rootobj.insert("return_laser", return_laser);
    QJsonDocument jsDoc(rootobj);
    robot_status_all1_default_req_obj = jsDoc.toJson();
}

//查询批量数据1 参数缺省 使用默认结构体
void robokit::RobotStatus::robot_status_all1_req_get()
{
    m_protocol.WriteTcpData(1100, robot_status_all1_default_req_obj,std::bind(&::robokit::RobotStatus::robot_status_all1_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}

//查询批量数据1
void robokit::RobotStatus::robot_status_all1_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();


    m_protocol.WriteTcpData(1100,jsonBuffer,std::bind(&::robokit::RobotStatus::robot_status_all1_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::updateStatusSound(const QJsonObject &status)
{
    const auto blocked = status.value("blocked");
    const auto taskStatus = status.value("task_status");
    if (!blocked.isBool() && !taskStatus.isDouble())
        return;

    auto nextSound = StatusSound::None;
    if (blocked.toBool())
        nextSound = StatusSound::Blocked;
    else if (taskStatus.toInt() == 2)
        nextSound = StatusSound::Navigation;
    else if (taskStatus.toInt() == 4)
        nextSound = StatusSound::Reached;

    const auto previousSound = statusSound;
    if (nextSound == previousSound)
        return;

    statusSound = nextSound;
    auto &music = Music::GetInstance();
    switch (nextSound) {
    case StatusSound::Blocked:
        music.blockPlay();
        break;
    case StatusSound::Navigation:
        music.navigationPlay();
        break;
    case StatusSound::Reached:
        music.reachedPlay();
        break;
    case StatusSound::None:
        if (previousSound == StatusSound::Blocked || previousSound == StatusSound::Navigation)
            music.stopmusic();
        break;
    }
}

void robokit::RobotStatus::updateMapTelemetry(const QJsonObject &status)
{
    const auto x = status.value("x");
    const auto y = status.value("y");
    const auto heading = status.value("angle");
    if (x.isDouble() && y.isDouble() && heading.isDouble())
        emit updatePosition(x.toDouble(), y.toDouble(), heading.toDouble());

    QVariantMap values;
    for (const auto &key : {QStringLiteral("confidence"), QStringLiteral("blocked"),
                            QStringLiteral("block_reason")}) {
        const auto value = status.value(key);
        if (!value.isUndefined())
            values.insert(key, value.toVariant());
    }
    if (m_diagnostics != values) {
        m_diagnostics = values;
        emit diagnosticsChanged();
    }
}

void robokit::RobotStatus:: robot_status_all1_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
//    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    if (rootObject.isEmpty())
        return;
    if (rootObject.value("ret_code").toInt() != 0) {
        // Older controllers may reject extra fields in the batch status request.
        if (requestMapMetadata && !mapReportedInStatus) {
            requestMapMetadata = false;
            set_default_all1_req_obj(false);
        }
        return;
    }
    const auto mapName = rootObject.value("current_map");
    if (mapName.isString()) {
        mapReportedInStatus = true;
        emit currentMapReported(mapName.toString(), rootObject.value("current_map_md5").toString());
    }
    updateMapTelemetry(rootObject);
    updateStatusSound(rootObject);
    QStringList keys = rootObject.keys();
    emit singal_sendToServerStatus(rootObject);
    //累计行驶里程, 单位 m
    double odo=rootObject.value("odo").toDouble();
    //今日累计行驶里程, 单位 m
    double today_odo=rootObject.value("today_odo").toDouble();
    //本次运行时间(开机后到当前的时间), 单位 ms
    double time=rootObject.value("time").toDouble();
    //累计运行时间, 单位 ms
    double total_time=rootObject.value("total_time").toDouble();
    //控制器温度, 单位 ℃
    double angle=rootObject.value("angle").toDouble();
    //机器人的定位置信度, 范围 [0, 1]
    double confidence=rootObject.value("confidence").toDouble();
    //机器人当前所在站点的 id（该判断比较严格，机器人必须很靠近某一个站点(<30cm， 这个距离可以通过参数配置中的CurrentPointDist修改)，否则为空字符，即不处于任何站点）
    QString current_station=rootObject.value("current_station").toString();
    //机器人上一个所在站点的 id
    QString last_station=rootObject.value("last_station").toString();
    //机器人在机器人坐标系的 x 方向实际的速度, 单位 m/
    double vx=rootObject.value("vx").toDouble();
    //机器人在机器人坐标系的 y 方向实际的速度, 单位 m/s
    double vy=rootObject.value("vy").toDouble();
    //机器人在机器人坐标系的实际的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double w=rootObject.value("w").toDouble();
    //单舵轮机器人当前的舵轮角度, 单位 rad
    double steer=rootObject.value("steer").toDouble();
    //机器人的托盘（如果存在）角度, 单位 rad
    double spin=rootObject.value("spin").toDouble();
    //机器人在机器人坐标系的 x 方向接收到的速度, 单位 m/s
    double r_vx=rootObject.value("r_vx").toDouble();
    //机器人在机器人坐标系的 y 方向收到的速度, 单位 m/s
    double r_vy=rootObject.value("r_vy").toDouble();
    //机器人在机器人坐标系的收到的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double r_w=rootObject.value("r_w").toDouble();
    //单舵轮机器人收到的舵轮角度, 单位 rad
    double r_steer=rootObject.value("r_steer").toDouble();
    //托盘机器人的收到托盘（如果存在）转动速度, 单位 rad/s
    double r_spin=rootObject.value("r_spin").toDouble();
    //多舵轮机器人当前的舵角角度, 双舵轮=[前舵轮角度，后舵轮角度], 单位 rad
    QJsonArray steer_angless= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < steer_angless.size(); i++){
        QJsonValue arrayValue = steer_angless.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //机器人底盘是否静止（行走电机）
    QJsonArray r_steer_angles= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < r_steer_angles.size(); i++){
        QJsonValue arrayValue = r_steer_angles.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }


    //机器人底盘是否静止（行走电机）
    bool is_stop=rootObject.value("is_stop").toBool();
    //机器人是否被阻挡
    bool blocked=rootObject.value("blocked").toBool();
    //被阻挡的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 = Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机
    double block_reason=rootObject.value("block_reason").toDouble();
    //	最近障碍物位置的 x 坐标, 单位 m
    double block_x=rootObject.value("block_x").toDouble();
    //最近障碍物位置的 y 坐标, 单位 m
    double block_y=rootObject.value("block_y").toDouble();
    //发生阻挡的超声 id 号, 仅当 block_reason = 0 时有意义
    double block_ultrasonic_id=rootObject.value("block_ultrasonic_id").toDouble();

    //发生阻挡的 DI 的 id 号, 仅当 block_reason = 2、3、4 时有意义
    double block_di=rootObject.value("block_di").toDouble();
    //机器人是否减速
    bool slowed=rootObject.value("slowed").toBool();
    //减速的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 = Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机
    double slow_reason=rootObject.value("slow_reason").toDouble();
    //最近的导致减速的障碍物位置的 x 坐标, 单位 m
    double slow_x=rootObject.value("slow_x").toDouble();
    //最近的导致减速的障碍物位置的 y 坐标, 单位 m
    double slow_y=rootObject.value("slow_y").toDouble();
    //发生减速的超声 id 号, 仅当 slow_reason = 0 时有意义
    double slow_ultrasonic_id=rootObject.value("slow_ultrasonic_id").toDouble();
    //发生减速的 DI 的 id 号, 仅当 slow_reason = 2、3、4 时有意义
    double slow_di=rootObject.value("slow_di").toDouble();
    //机器人电池电量, 范围 [0, 1]
    double battery_level=rootObject.value("battery_level").toDouble();
    //机器人电池温度, 单位 ℃
    double battery_temp=rootObject.value("battery_temp").toDouble();

    //电池是否正在充电
    bool charging=rootObject.value("charging").toBool();
    //电压, 单位 V
    double voltage=rootObject.value("voltage").toDouble();
    //电流, 单位 A
    double current=rootObject.value("current").toDouble();
    //允许充电的最大电压( -1 = 该电池不支持此功能), 单位 V
    double max_charge_voltage=rootObject.value("max_charge_voltage").toDouble();
    //允许充电的最大电流( -1 = 该电池不支持此功能), 单位 A
    double max_charge_current=rootObject.value("max_charge_current").toDouble();
    //仅指示机器人是否连接手动充电器，不保证电源是否接通，连接手动充电器时不能运动(仅 SRC-2000 支持)
    bool manual_charge=rootObject.value("manual_charge").toBool();
    //仅指示机器人是否连接自动充电桩，不保证电源是否接通(仅 SRC-2000 支持)
    bool auto_charge=rootObject.value("auto_charge").toBool();
    //电池循环次数(从电池 BMS 获取，不保证正确)
    double battery_cycle=rootObject.value("battery_cycle").toDouble();
    //激光点云数据
    QJsonArray lasers= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < lasers.size(); i++){
        QJsonValue arrayValue = lasers.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //按顺序的路径点数据, 依次连接就是机器人规划的路径
    QJsonArray path= rootObject.value("path").toArray();
    for(int i = 0; i < path.size(); i++){
        QJsonValue arrayValue = path.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //	机器人所在区域 id 的数组(由于地图上的区域是可以重叠的, 所以机器人可能同时在多个区域)，数组可能为空
    QJsonArray area_ids= rootObject.value("area_ids").toArray();
    for(int i = 0; i < area_ids.size(); i++){
        QJsonValue arrayValue = area_ids.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //true 表示急停按钮处于激活状态(按下), false 表示急停按钮处于非激活状态(拔起)
    bool emergency=rootObject.value("emergency").toBool();
    //true 表示电机驱动器发生急停, false 表示电机驱动器发生未急停
    bool driver_emc=rootObject.value("driver_emc").toBool();
    //id: 对应的 id 号
    //source: 来源
    //normal = 普通 DI
    //virtual = 虚拟 DI
    //modbus = modbus DI
    //status: 表示高低电平
    //true = 高电平
    //false = 低电平
    //valid: 对应 DI 是否启用
    //true = 启用
    //false = 禁用
    QJsonArray DI= rootObject.value("DI").toArray();
    for(int i = 0; i < DI.size(); i++){
        QJsonValue arrayValue = DI.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //id: 对应的 id 号
    //source: 来源
    //normal = 普通 DO
    //modbus = modbus DO
    //status: 表示高低电平
    //true = 高电平
    //false = 低电平
    QJsonArray DO= rootObject.value("DO").toArray();
    for(int i = 0; i < DO.size(); i++){
        QJsonValue arrayValue = DO.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //偏航角，单位：rad
    double yaw=rootObject.value("yaw").toDouble();
    //滚转角，单位：rad
    double roll=rootObject.value("roll").toDouble();
    //俯仰角，单位：rad
    double pitch=rootObject.value("pitch").toDouble();

    //超声节点数据
    QJsonArray ultrasonic_nodes= rootObject.value("ultrasonic_nodes").toArray();
    for(int i = 0; i < ultrasonic_nodes.size(); i++){
        QJsonValue arrayValue = ultrasonic_nodes.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //二维码识别数据
    QJsonArray pgvs= rootObject.value("pgvs").toArray();
    for(int i = 0; i < pgvs.size(); i++){
        QJsonValue arrayValue = pgvs.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //0 = NONE, 1 = WAITING(目前不可能出现该状态), 2 = RUNNING, 3 = SUSPENDED, 4 = COMPLETED, 5 = FAILED, 6 = CANCELED
    double task_status=rootObject.value("task_status").toDouble();
    //导航类型, 0 = 没有导航, 1 = 自由导航到任意点, 2 = 自由导航到站点, 3 = 路径导航到站点, 7 = 平动转动, 100 = 其他
    double task_type=rootObject.value("task_type").toDouble();
    //当前导航要去的站点, 仅当 task_type 为 2 或 3 时该字段有效, task_status 为 RUNNING 时说明正在去这个站点, task_status 为 COMPLETED 时说明已经到达这个站点, task_status 为 FAILED 时说明去这个站点失败, task_status 为 SUSPENDED 则说明去这个站点的导航暂停
    QString target_id=rootObject.value("target_id").toString();
    //当前导航要去的坐标点, 为一个包含三个元素的数组, 分别为在世界坐标系中的 x, y, r 坐标, 仅当 task_type 为 1 时该字段有效, task_status 为 RUNNING 时说明正在去这个坐标点, task_status 为 COMPLETED 时说明已经到达这个坐标点, task_status 为 FAILED 时说明去这个坐标点失败, task_status 为 SUSPENDED 则说明去这个坐标点的导航暂停
    QJsonArray target_point= rootObject.value("target_point").toArray();
    for(int i = 0; i < target_point.size(); i++){
        QJsonValue arrayValue = target_point.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //当前导航路径上已经经过的站点, 为站点的数组, 仅当 task_type 为 3 或 4 时该字段有效点。这里会列出所有已经经过的中间点
    QJsonArray finished_path= rootObject.value("target_point").toArray();
    for(int i = 0; i < finished_path.size(); i++){
        QJsonValue arrayValue = finished_path.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //当前导航路径上尚未经过的站点, 为站点的数组, 仅当 task_type 为 3 或 4 时该字段有效。这里会列出所有尚未经过的中间点
    QJsonArray unfinished_path= rootObject.value("unfinished_path").toArray();
    for(int i = 0; i < unfinished_path.size(); i++){
        QJsonValue arrayValue = unfinished_path.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //0 = FAILED(定位失败), 1 = SUCCESS(定位正确), 2 = RELOCING(正在重定位), 3=COMPLETED(定位完成)
    double reloc_status=rootObject.value("reloc_status").toDouble();
    //0 = FAILED(载入地图失败), 1 = SUCCESS(载入地图成功), 2 = LOADING(正在载入地图)
    double loadmap_status=rootObject.value("loadmap_status").toDouble();
    //0 = 没有扫图, 1 = 正在扫图(离线), 2 = 正在实时扫图(SLAM)，3 = 正在3D扫图（离线），4 = 正在实时3D扫图
    double slam_status=rootObject.value("slam_status").toDouble();
    //内容为 message_task.proto 中 Message_TaskStatus 的 JSON 形式
    QString tasklist_status=rootObject.value("tasklist_status").toString();
    //货叉高度, 单位 m
    QString fork_height=rootObject.value("fork_height").toString();
    //	货叉前移后退是否到位, true = 到位, false = 未到位
    bool forward_in_place=rootObject.value("forward_in_place").toBool();
    //运行模式, false = 手动, true = 自动
    bool roller_mode=rootObject.value("roller_mode").toBool();
    //运行状态, 0x01 = 待机, 0x02 = 执行中, 0x03 = 执行完成, 0xFF = 执行失败
    double roller_state=rootObject.value("roller_state").toDouble();
    //辊筒是否启用, false = 未启用, true = 启用
    bool roller_enable=rootObject.value("roller_enable").toBool();
    //	辊筒电机速率
    double roller_speed=rootObject.value("roller_speed").toDouble();
    //辊筒是否急停, false = 未急停, true = 急停
    bool roller_emc=rootObject.value("roller_emc").toBool();
    //运行模式, false = 手动, true = 自动
    bool jack_mode=rootObject.value("jack_mode").toBool();
    //运行状态, 0x00 = 上升中，0x01 = 上升到位，0x02 = 下降中，0x03 = 下降到位，0x04 = 停止，0xFF = 执行失败
    double jack_state=rootObject.value("jack_state").toDouble();
    //顶升是否启用, false = 未启用, true = 启用
    bool jack_enable=rootObject.value("jack_enable").toBool();
    //顶升错误码
    double jack_error_code=rootObject.value("jack_error_code").toDouble();
    //顶升机构上是否有料, false = 无料, true = 有料
    bool jack_isFull=rootObject.value("jack_isFull").toBool();
    //顶升机构电机速率, 单位: mm/s
    double jack_speed=rootObject.value("jack_speed").toDouble();
    //	顶升机构是否急停, false = 未急停, true = 急停
    bool jack_emc=rootObject.value("jack_emc").toBool();
    //顶升当前的高度，单位: m
    double jack_height=rootObject.value("jack_height").toDouble();
    //顶升顶起次数
    double jack_load_times=rootObject.value("jack_load_times").toDouble();
    //外设中的用户自定义数据
    QJsonArray peripheral_data= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < peripheral_data.size(); i++){
        QJsonValue arrayValue = peripheral_data.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //0 = 单机模式, 1 = 调度模式
    double dispatch_mode=rootObject.value("fatals").toDouble();
    //报警码 Fatal的数组, 所有出现的 Fatal 报警都会出现在数据中
    QJsonArray fatals= rootObject.value("peripheral_data").toArray();
    for(int i = 0; i < fatals.size(); i++){
        QJsonValue arrayValue = fatals.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //报警码 Error的数组, 所有出现的 Error 报警都会出现在数据中
    QJsonArray errors= rootObject.value("errors").toArray();
    for(int i = 0; i < errors.size(); i++){
        QJsonValue arrayValue = errors.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //报警码 Warning的数组, 所有出现的 Warning 报警都会出现在数据中
    QJsonArray warnings= rootObject.value("warnings").toArray();
    for(int i = 0; i < peripheral_data.size(); i++){
        QJsonValue arrayValue = peripheral_data.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //	Notice的数组, 所有出现的 Notice 报警都会出现在数据中
    QJsonArray notices= rootObject.value("notices").toArray();
    for(int i = 0; i < notices.size(); i++){
        QJsonValue arrayValue = notices.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //当前地图名
    QString current_map=rootObject.value("current_map").toString();
    //当前地图 MD5 值
    QString current_map_md5=rootObject.value("current_map_md5").toString();
    //当前连接 Wifi 的 SSID（机器人需要支持连接 Wifi 并且已经连接，否则为空）
    QString ssid=rootObject.value("ssid").toString();
    //当前连接 Wifi 的信号强度，0-100 百分比（机器人需要支持连接 Wifi 并且已经连接，否则为0）
    double rssi=rootObject.value("rssi").toDouble();
    //当前 IP（除 192.168.192.5 的另一个 IP 地址
    QString MAC=rootObject.value("MAC").toString();
    //机器人无线客户端的 MAC 地址（仅对 SRC-2000 控制器有效），格式为 XXXXXXXXXXXX，中间没有分隔符
    QString current_ip=rootObject.value("current_ip").toString();
    //扫描到的所有 RFID 标签 id, 如果没扫描到 RFID 标签, 则为空数组
    QJsonArray rfids= rootObject.value("rfids").toArray();
    for(int i = 0; i < rfids.size(); i++){
        QJsonValue arrayValue = rfids.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //电池用户自定义数据
    QString battery_user_data=rootObject.value("battery_user_data").toString();
    //机器人名称
    QString vehicle_id=rootObject.value("vehicle_id").toString();
    //机器人备注
    QString robot_note=rootObject.value("robot_note").toString();
    //导航任务附加信息
    QString move_status_info=rootObject.value("move_status_info").toString();
    //透传的数据信息
    QString transparent_data=rootObject.value("transparent_data").toString();
    //扫描到的所有 RFID 标签 id, 如果没扫描到 RFID 标签, 则为空数组
    QJsonArray containers= rootObject.value("rfids").toArray();
    for(int i = 0; i < containers.size(); i++){
        QJsonValue arrayValue =containers.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();


}

//查询批量数据2
void robokit::RobotStatus::robot_status_all2_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    m_protocol.WriteTcpData(1101,jsonBuffer,std::bind(&::robokit::RobotStatus::robot_status_all2_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::robot_status_all2_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
    emit singal_sendToServerStatus(rootObject);
    //机器人运行模式，手动模式=0，自动模式=1
    double mode=rootObject.value("mode").toDouble();
    //机器人的 x 坐标, 单位 m
    double x=rootObject.value("x").toDouble();
    //机器人的 y 坐标, 单位 m
    double y=rootObject.value("y").toDouble();
    //机器人的 angle 坐标, 单位 rad
    double angle_=rootObject.value("angle").toDouble();
    angle = angle_;
    //触发坐标返回
    updateMapTelemetry(rootObject);
    //机器人的定位置信度, 范围 [0, 1]
    double confidence_=rootObject.value("confidence").toDouble();
    confidence = confidence_;
    //	机器人当前所在站点的 id（该判断比较严格，机器人必须很靠近某一个站点(<30cm， 这个距离可以通过参数配置中的CurrentPointDist修改)，否则为空字符，即不处于任何站点）
    QString current_station_=rootObject.value("current_station").toString();
    current_station= current_station_;
    //	机器人上一个所在站点的 id
    QString last_station=rootObject.value("last_station").toString();
    //机器人在机器人坐标系的 x 方向实际的速度, 单位 m/s
    double vx=rootObject.value("vx").toDouble();
    //机器人在机器人坐标系的 y 方向实际的速度, 单位 m/s
    double vy=rootObject.value("vy").toDouble();
    //机器人在机器人坐标系的实际的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double w=rootObject.value("w").toDouble();
    //	单舵轮机器人当前的舵轮角度, 单位 rad
    double steer=rootObject.value("steer").toDouble();
    //机器人的托盘（如果存在）角度, 单位 rad
    double spin=rootObject.value("spin").toDouble();
    //	机器人在机器人坐标系的 x 方向接收到的速度, 单位 m/s
    double r_vx=rootObject.value("r_vx").toDouble();
    //机器人在机器人坐标系的 y 方向收到的速度, 单位 m/s
    double r_vy=rootObject.value("r_vy").toDouble();
    //机器人在机器人坐标系的收到的角速度(即顺时针转为负, 逆时针转为正), 单位 rad/s
    double r_w=rootObject.value("r_w").toDouble();
    //单舵轮机器人收到的舵轮角度, 单位 rad
    double r_steer=rootObject.value("r_steer").toDouble();
    //	托盘机器人的收到托盘（如果存在）转动速度, 单位 rad/s
    double r_spin=rootObject.value("r_spin").toDouble();
    /*
    //多舵轮机器人当前的舵角角度, 双舵轮=[前舵轮角度，后舵轮角度], 单位 rad
    QJsonArray steer_angles= rootObject.value("rfids").toArray();
    for(int i = 0; i < steer_angles.size(); i++){
                        QJsonValue arrayValue = steer_angles.at(i);
                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //多舵轮机器人收到的舵角角度, 双舵轮=[前舵轮角度，后舵轮角度], 单位 rad
    QJsonArray r_steer_angles= rootObject.value("rfids").toArray();
    for(int i = 0; i < r_steer_angles.size(); i++){
                        QJsonValue arrayValue = r_steer_angles.at(i);
                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
*/
    //机器人底盘是否静止（行走电机）
    bool is_stop=rootObject.value("is_stop").toBool();

    //被阻挡的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 = Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机
    double block_reason=rootObject.value("block_reason").toDouble();
    //最近障碍物位置的 x 坐标, 单位 m
    double block_x=rootObject.value("block_x").toDouble();
    //最近障碍物位置的 y 坐标, 单位 m
    double block_y=rootObject.value("block_y").toDouble();
    //发生阻挡的超声 id 号, 仅当 block_reason = 0 时有意义
    double block_ultrasonic_id=rootObject.value("block_ultrasonic_id").toDouble();
    //发生阻挡的 DI 的 id 号, 仅当 block_reason = 2、3、4 时有意义
    double block_di=rootObject.value("block_di").toDouble();
    //机器人是否减速
    bool slowed=rootObject.value("slowed").toBool();
    //减速的原因, 0 = Ultrasonic (超声), 1 = Laser (激光), 2 = Fallingdown (防跌落传感器), 3 = Collision (碰撞传感器), 4 = Infrared (红外传感器), 5 = Lock（锁车开关），6 = 动态障碍物， 7 = 虚拟激光点，8 = 3D 相机
    double slow_reason=rootObject.value("slow_reason").toDouble();
    //	最近的导致减速的障碍物位置的 x 坐标, 单位 m
    double slow_x=rootObject.value("slow_x").toDouble();
    //最近的导致减速的障碍物位置的 y 坐标, 单位 m
    double slow_y=rootObject.value("slow_y").toDouble();
    //发生减速的超声 id 号, 仅当 slow_reason = 0 时有意义
    double slow_ultrasonic_id=rootObject.value("slow_ultrasonic_id").toDouble();
    //	发生减速的 DI 的 id 号, 仅当 slow_reason = 2、3、4 时有意义
    double slow_di=rootObject.value("slow_di").toDouble();

    /*
    //激光点云数据
    QJsonArray lasers= rootObject.value("lasers").toArray();
    for(int i = 0; i < lasers.size(); i++){
                        QJsonValue arrayValue = lasers.at(i);
                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
*/
    //按顺序的路径点数据, 依次连接就是机器人规划的路径
    QJsonArray path= rootObject.value("path").toArray();
//    for(int i = 0; i < path.size(); i++){
//        QJsonValue arrayValue = path.at(i);
//                arrayValue.toArray();
//                qDebug() << "Array" << i << ": " << arrayValue.toString();

//    }
    //	机器人所在区域 id 的数组(由于地图上的区域是可以重叠的, 所以机器人可能同时在多个区域)，数组可能为空
    QJsonArray area_ids= rootObject.value("area_ids").toArray();
    for(int i = 0; i < area_ids.size(); i++){
        QJsonValue arrayValue =area_ids.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //true 表示急停按钮处于激活状态(按下), false 表示急停按钮处于非激活状态(拔起)
    bool emergency=rootObject.value("emergency").toBool();
    //true 表示电机驱动器发生急停, false 表示电机驱动器发生未急停
    bool driver_emc=rootObject.value("driver_emc").toBool();
    //	source: 来源
    //    normal = 普通 DI
    //    virtual = 虚拟 DI
    //    modbus = modbus DI
    //    status: 表示高低电平
    //    true = 高电平
    //    false = 低电平
    //    valid: 对应 DI 是否启用
    //    true = 启用
    //    false = 禁用
    QJsonArray DI= rootObject.value("DI").toArray();
    for(int i = 0; i < DI.size(); i++){
        QJsonValue arrayValue =DI.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //d: 对应的 id 号
    //source: 来源
    //normal = 普通 DO
    //modbus = modbus DO
    //status: 表示高低电平
    //true = 高电平
    //false = 低电平
    QJsonArray DO= rootObject.value("DO").toArray();
    for(int i = 0; i < DO.size(); i++){
        QJsonValue arrayValue =DO.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //偏航角，单位：rad
    double yaw=rootObject.value("yaw").toDouble();
    //滚转角，单位：rad
    double roll=rootObject.value("roll").toDouble();
    //俯仰角，单位：rad
    double pitch=rootObject.value("pitch").toDouble();
    //超声节点数据
    QJsonArray ultrasonic_nodes= rootObject.value("ultrasonic_nodes").toArray();
    for(int i = 0; i < ultrasonic_nodes.size(); i++){
        QJsonValue arrayValue =ultrasonic_nodes.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //二维码识别数据
    QJsonArray pgvs= rootObject.value("pgvs").toArray();
    for(int i = 0; i < pgvs.size(); i++){
        QJsonValue arrayValue =pgvs.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //	0 = NONE, 1 = WAITING(目前不可能出现该状态), 2 = RUNNING, 3 = SUSPENDED, 4 = COMPLETED, 5 = FAILED, 6 = CANCELED
    double task_status_=rootObject.value("task_status").toDouble();
task_status=task_status_;

    updateStatusSound(rootObject);

    //导航类型, 0 = 没有导航, 1 = 自由导航到任意点, 2 = 自由导航到站点, 3 = 路径导航到站点, 7 = 平动转动, 100 = 其他
    double task_type_=rootObject.value("task_type").toDouble();
    task_type = task_type_;
    //	当前导航要去的站点, 仅当 task_type 为 2 或 3 时该字段有效, task_status 为 RUNNING 时说明正在去这个站点, task_status 为 COMPLETED 时说明已经到达这个站点, task_status 为 FAILED 时说明去这个站点失败, task_status 为 SUSPENDED 则说明去这个站点的导航暂停
    QString target_id_=rootObject.value("target_id").toString();
    target_id = target_id_;
    //当前导航要去的坐标点, 为一个包含三个元素的数组, 分别为在世界坐标系中的 x, y, r 坐标, 仅当 task_type 为 1 时该字段有效, task_status 为 RUNNING 时说明正在去这个坐标点, task_status 为 COMPLETED 时说明已经到达这个坐标点, task_status 为 FAILED 时说明去这个坐标点失败, task_status 为 SUSPENDED 则说明去这个坐标点的导航暂停
    QJsonArray target_point= rootObject.value("target_point").toArray();
    for(int i = 0; i < target_point.size(); i++){
        QJsonValue arrayValue =target_point.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //当前导航路径上尚未经过的站点, 为站点的数组, 仅当 task_type 为 3 或 4 时该字段有效。这里会列出所有尚未经过的中间点
    QJsonArray unfinished_path= rootObject.value("unfinished_path").toArray();
    for(int i = 0; i < unfinished_path.size(); i++){
        QJsonValue arrayValue =unfinished_path.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //当前导航路径上尚未经过的站点, 为站点的数组, 仅当 task_type 为 3 或 4 时该字段有效。这里会列出所有尚未经过的中间点
    QJsonArray fatals= rootObject.value("fatals").toArray();
    for(int i = 0; i < fatals.size(); i++){
        QJsonValue arrayValue =fatals.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //报警码 Error的数组, 所有出现的 Error 报警都会出现在数据中
    QJsonArray errors= rootObject.value("errors").toArray();
    for(int i = 0; i < errors.size(); i++){
        QJsonValue arrayValue =errors.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //报警码 Warning的数组, 所有出现的 Warning 报警都会出现在数据中
    QJsonArray warnings= rootObject.value("warnings").toArray();
    for(int i = 0; i < warnings.size(); i++){
        QJsonValue arrayValue =warnings.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }
    //Notice的数组, 所有出现的 Notice 报警都会出现在数据中
    QJsonArray notices= rootObject.value("notices").toArray();
    for(int i = 0; i < notices.size(); i++){
        QJsonValue arrayValue =notices.at(i);
        //                        qDebug() << "Array" << i << ": " << arrayValue.toString();

    }

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}




//查询批量数据3
void robokit::RobotStatus:: robot_status_all3_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1102,sendData,std::bind(&robokit::RobotStatus::robot_status_all3_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_all3_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //累计行驶里程, 单位 m
    double odo=rootObject.value("odo").toDouble();
    //今日累计行驶里程, 单位 m
    double today_odo=rootObject.value("today_odo").toDouble();
    //本次运行时间(开机后到当前的时间), 单位 ms
    double time=rootObject.value("time").toDouble();
    //累计运行时间, 单位 ms
    double total_time=rootObject.value("total_time").toDouble();
    //控制器温度, 单位 ℃
    double controller_temp=rootObject.value("controller_temp").toDouble();
    //控制器湿度, 单位 %
    double controller_humi=rootObject.value("controller_humi").toDouble();
    //控制器电压, 单位 V
    double controller_voltage=rootObject.value("controller_voltage").toDouble();
    //机器人电池电量, 范围 [0, 1]
    double battery_level=rootObject.value("battery_level").toDouble();
    //机器人电池温度, 单位 ℃
    double battery_temp=rootObject.value("battery_temp").toDouble();
    //电池是否正在充电
    bool charging=rootObject.value("charging").toBool();
    //电压, 单位 V
    double voltage=rootObject.value("voltage").toDouble();
    //电流, 单位 A
    double current=rootObject.value("current").toDouble();
    //允许充电的最大电压(-1=该电池不支持此功能), 单位 V
    double max_charge_voltage=rootObject.value("max_charge_voltage").toDouble();
    //允许充电的最大电流(-1=该电池不支持此功能), 单位 A
    double max_charge_current=rootObject.value("max_charge_current").toDouble();
    //仅指示机器人是否连接手动充电器，不保证电源是否接通，连接手动充电器时不能运动(仅SRC2000支持)
    bool manual_charge=rootObject.value("manual_charge").toBool();
    //仅指示机器人是否连接自动充电桩，不保证电源是否接通(仅SRC2000支持)
    bool auto_charge=rootObject.value("auto_charge").toBool();
    //电池循环次数(由电池供应商提供的数据，不保证正确)
    double battery_cycle=rootObject.value("battery_cycle").toDouble();
    //    0 = FAILED(定位失败),
    //    1 = SUCCESS(定位正确), 2 = RELOCING(正在重定位),
    //    3=COMPLETED(定位完成)
    double reloc_status=rootObject.value("reloc_status").toDouble();
    //    0 = FAILED(载入地图失败)
    //    1 = SUCCESS(载入地图成功),
    //    2 = LOADING(正在载入地图)
    double loadmap_status=rootObject.value("loadmap_status").toDouble();
    //    0 = 没有扫图,
    //    1 = 正在扫图(离线)
    //    2 = 正在实时扫图(SLAM)
    //    3 = 正在3D扫图（离线）
    //    4 = 正在实时3D扫图
    double slam_status=rootObject.value("slam_status").toDouble();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询当前控制权所有者
void robokit::RobotStatus:: robot_status_current_lock_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1060,sendData,std::bind(&robokit::RobotStatus::robot_status_current_lock_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_current_lock_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //当前控制权是否被抢占
    bool locked=rootObject.value("locked").toBool();
    //控制权所有者 ip
    QString ip=rootObject.value("ip").toString();
    //控制权所有者昵称信息
    QString nick_name=rootObject.value("nick_name").toString();
    //抢占控制权的时间戳(s)
    double time_t=rootObject.value("time_t").toDouble();
    //控制权所有者描述
    QString desc=rootObject.value("desc").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询机器人载入的地图以及储存的地图 参数可缺省
void robokit::RobotStatus::robot_status_map_req_get()
{
    m_protocol.WriteTcpData(1300, QByteArrayLiteral("{}"),
                            std::bind(&RobotStatus::robot_status_map_res_receive, this,
                                      std::placeholders::_1, std::placeholders::_2),
                            true);
}

void robokit::RobotStatus::robot_status_map_res_receive(uint16_t, QByteArray &receivedata)
{
    const auto status = QJsonDocument::fromJson(receivedata).object();
    const auto name = status.value("current_map");
    if (!mapReportedInStatus && name.isString() && status.value("ret_code").toInt() == 0)
        emit currentMapReported(name.toString(), status.value("current_map_md5").toString());
}


//查询机器人当前载入地图中的站点信息
//需要查询的地图列表，应该保证机器人中存在地图列表中所有的地图
//JSObject.insert("map_names",QJsonValue(map_names));
void robokit::RobotStatus::robot_status_station_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1301,sendData,std::bind(&robokit::RobotStatus::robot_status_station_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_station_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //站点数组, 若地图中没有站点, 则为空数组, object 形式见下文
    QJsonArray stations= rootObject.value("stations").toArray();
    for(int i = 0; i < stations.size(); i++){
        QJsonValue arrayValue =stations.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询指定地图列表的MD5值

//需要查询的地图列表，应该保证机器人中存在地图列表中所有的地图
//JSObject.insert("map_names",QJsonValue(map_names));

void robokit::RobotStatus:: robot_status_mapmd5_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    
    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1302,sendData,std::bind(&robokit::RobotStatus::robot_status_motor_req_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_motor_req_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //地图的MD5值和名字，数据示例见下文
    QJsonArray map_info= rootObject.value("map_info").toArray();
    for(int i = 0; i < map_info.size(); i++){
        QJsonValue arrayValue = map_info.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }


    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人参数

//参数所属的插件名, 如果缺省, 表示查询所有插件的所有参数
//JSObject.insert("plugin",QJsonValue(plugin));
//参数名, 如果 plugin 存在，但 param 缺省，代表查询该插件的所有参数。否则查询该插件的指定参数
//JSObject.insert("param",QJsonValue(param));
void robokit::RobotStatus:: robot_status_params_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1400,sendData,std::bind(&robokit::RobotStatus::robot_status_params_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_params_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

}


//下载机器人模型文件
void robokit::RobotStatus:: robot_status_model_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1500,jsonBuffer,std::bind(&robokit::RobotStatus::robot_status_model_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_model_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
}


//上传机器人文件

//参数所属的插件名, 如果缺省, 表示查询所有插件的所有参数
// JSObject.insert("type",QJsonValue(type));
//机器人文件的相对路径，针对于type指定的文件类型
//    JSObject.insert("file_path",QJsonValue(file_path));

void robokit::RobotStatus:: robot_status_uploadfile_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();



    m_protocol.WriteTcpData(1799 ,jsonBuffer,std::bind(&::robokit::RobotStatus::robot_status_uploadfile_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_uploadfile_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//下载机器人文件
void robokit::RobotStatus::robot_status_downloadfile_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    //机器人文件的类型，“app”=机器人程序目录下的文件，“users”=机器人用户目录下的文件
    //JSObject.insert("type",QJsonValue(type));
    //机器人文件的相对路径，针对于type指定的文件类型
    //JSObject.insert("file_path",QJsonValue(file_path));


    m_protocol.WriteTcpData(1800 ,jsonBuffer,std::bind(&::robokit::RobotStatus::robot_status_downloadfile_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_downloadfile_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
}



//查询机器人文件列表

//机器人文件的类型，“app”=机器人程序目录下的文件，“users”=机器人用户目录下的文件
//JSObject.insert("type",QJsonValue(type));
//机器人文件夹的相对路径，针对于type指定的文件类型
//JSObject.insert("path",QJsonValue(path));
void robokit::RobotStatus:: robot_status_listfile_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1798 ,sendData,std::bind(&::robokit::RobotStatus::robot_status_info_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_listfile_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //机器人文件的类型，“app”=机器人程序目录下的文件，“users”=机器人用户目录下的文件
    QString type=rootObject.value("type").toString();
    //机器人文件夹的相对路径，针对于type指定的文件类型
    QString path=rootObject.value("path").toString();
    //为目录列表的json数组，每一个目录为一个json对象。其中包含目录名name:string和文件夹标识位is_directory:bool，详情见示例。
    QJsonArray entries= rootObject.value("entries").toArray();
    for(int i = 0; i < entries.size(); i++){
        QJsonValue arrayValue = entries.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }


    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}


//查询机器人脚本列表
void robokit::RobotStatus ::robot_status_script_info_req_get(const QJsonObject& obj)
{


    QByteArray sendData;
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    //机器人脚本的类型，“module”=机构脚本，“genetic”=通用脚本
    //JSObject.insert("type",QJsonValue(type));


    m_protocol.WriteTcpData(1506,sendData,std::bind(&robokit::RobotStatus::robot_status_script_info_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_script_info_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //为脚本列表的json数组，每一个脚本信息为一个json对象。其中包含脚本md5:string，脚本相对路径relative_path:string，脚本类型type:string详情见示例。
    QJsonArray list= rootObject.value("list").toArray();
    for(int i = 0; i < list.size(); i++){
        QJsonValue arrayValue = list.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }


    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}

//查询机器人脚本详情列表
void robokit::RobotStatus ::robot_status_script_detailslist_req_get(const QJsonObject& obj)

{
    QByteArray sendData;
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();

    //机器人脚本的类型，“module”=机构脚本，“genetic”=通用脚本
    //JSObject.insert("type",QJsonValue(type));


    m_protocol.WriteTcpData(1507,sendData,std::bind(&robokit::RobotStatus::robot_status_script_detailslist_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_script_detailslist_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //为脚本列表的json数组，每一个脚本信息为一个json对象。其中包含脚本md5:string，脚本相对路径name:string，脚本默认参数default_args:json，脚本说明description:string详情见示例。
    QJsonArray list= rootObject.value("list").toArray();
    for(int i = 0; i < list.size(); i++){
        QJsonValue arrayValue = list.at(i);
        qDebug() << "Array" << i << ": " << arrayValue.toString();
    }


    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();

}


//查询机器人脚本默认参数

//机器人脚本的类型，“module”=机构脚本，“genetic”=通用脚本
//JSObject.insert("type",QJsonValue(type));
//脚本相对目录，与脚本查询中的相同
//JSObject.insert("relative_path",QJsonValue(relative_path));
void robokit::RobotStatus:: robot_status_script_args_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1508 ,sendData,std::bind(&robokit::RobotStatus::robot_status_script_args_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_script_args_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();






    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}



//查询透传数据

void robokit::RobotStatus::robot_status_transparent_data_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1900,jsonBuffer,std::bind(&robokit::RobotStatus::robot_status_transparent_data_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_transparent_data_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
}


//查询机械臂状态

void robokit::RobotStatus::robot_status_armstatus_req_get(const QJsonObject& obj)
{
    QJsonDocument jsDoc(obj);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1669,jsonBuffer,std::bind(&robokit::RobotStatus::robot_status_armstatus_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus::  robot_status_armstatus_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();
}

//计算机械臂坐标转换

//机械臂转换类型：
//            transform=ComputTrans
//            mark2Base=ComputMarkToBase
//JSObject.insert("type",QJsonValue(type));
void robokit::RobotStatus:: robot_status_armcalculate_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1670 ,sendData,std::bind(&robokit::RobotStatus::robot_status_armmove_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_armmove_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //            机械臂转换类型：
    //            transform=ComputTrans
    //            mark2Base=ComputMarkToBase
    QString type=rootObject.value("type").toString();
    //转换结果
    QString data=rootObject.value("data").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//机械臂binTask任务

//机械臂binTask任务id
//JSObject.insert("taskId",QJsonValue(taskId));
//	机械臂binTask任务组名
//JSObject.insert("group",QJsonValue(group));
void robokit::RobotStatus:: robot_status_armtask_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1671 ,sendData,std::bind(&::robokit::RobotStatus::robot_status_armtask_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_armtask_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//机械臂运动控制

//机械臂控制类型：
//stop=停止
//pause=暂停
//resume=继续
//JSObject.insert("type",QJsonValue(type));
void robokit::RobotStatus:: robot_status_armmove_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;




    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData( 1673  ,sendData,std::bind(&robokit::RobotStatus::robot_status_armmove_rep_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_armmove_rep_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}




//机器臂示教器面板控制

//机械臂示教器面板控制类型：
//power_on=上电
//power_off=下电
//break_release=松抱闸
//shutdown=关机
//unlock_protective_stop=解除保护性停止
//JSObject.insert("type",QJsonValue(type));

void robokit::RobotStatus:: robot_status_armoperation_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;



    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData(1674 ,sendData,std::bind(&robokit::RobotStatus::robot_status_armoperation_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_armoperation_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询驱动器参数
void robokit::RobotStatus::  robot_status_canframe_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1750  ,sendData,std::bind(&robokit::RobotStatus::robot_status_canframe_res, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_canframe_res(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //发送报文的canid，如0x601
    double ID=rootObject.value("ID").toDouble();
    //报文是否为扩展型，一般为false
    bool Extended=rootObject.value("Extended").toBool();
    //发送报文的数据长度，一般为8
    double DLC=rootObject.value("DLC").toDouble();
    //	报文数据区
    QString Data=rootObject.value("Data").toString();
    //使用的端口，有1和2
    double Channel=rootObject.value("Channel").toDouble();
}

//查询GNSS连接状态

//	GNSS id
//JSObject.insert("id",QJsonValue(id));
void robokit::RobotStatus:: robot_status_gnsscheck_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;




    QJsonDocument jsDoc(JSObject);
    QByteArray jsonBuffer = jsDoc.toJson();
    m_protocol.WriteTcpData( 1760 ,sendData,std::bind(&robokit::RobotStatus::robot_status_gnsscheck_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_gnsscheck_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //GNSS 连接状态：未连接=false；已连接=true
    bool alive=rootObject.value("alive").toBool();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询GNSS设备列表
void robokit::RobotStatus:: robot_status_gnss_list_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;


    m_protocol.WriteTcpData(1761 ,sendData,std::bind(&robokit::RobotStatus::robot_status_gnss_list_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_gnss_list_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //GNSS 设备列表




    bool alive=rootObject.value("alive").toBool();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}

//查询当前播放音频名称
void robokit::RobotStatus::  robot_status_sound_req_get(const QJsonObject& obj)
{
    QByteArray sendData;
    QJsonObject JSObject;

    m_protocol.WriteTcpData(1850 ,sendData,std::bind(&robokit::RobotStatus::robot_status_sound_res_receive, this, std::placeholders::_1, std::placeholders::_2));
}
void robokit::RobotStatus:: robot_status_sound_res_receive(uint16_t revCommand, QByteArray& receivedata)
{
    qDebug()<<"call back";
    QJsonDocument itemDoc = QJsonDocument::fromJson(receivedata);
    QJsonObject rootObject = itemDoc.object();
    QStringList keys = rootObject.keys();

    //当前播放音频的名称
    QString sound_name=rootObject.value("sound_name").toString();
    //API 错误码
    double ret_code=rootObject.value("ret_code").toDouble();
    //API 上传时间戳
    QString create_on=rootObject.value("create_on").toString();
    //错误信息
    QString err_msg=rootObject.value("err_msg").toString();
}
