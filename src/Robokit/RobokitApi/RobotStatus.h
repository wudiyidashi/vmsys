#ifndef ROBOTSTATUS_H
#define ROBOTSTATUS_H

#include <QObject>
#include <QVariantMap>

#include <Robokit/RobokitApi/RobProtocol.h>
#include "Robokit/RobokitApi/RobotBase.h"

namespace robokit {
class RobotStatus : public RobotBase
{
    Q_OBJECT
    Q_PROPERTY(QString robotAddress READ robotAddress NOTIFY robotAddressChanged)
    Q_PROPERTY(QVariantMap diagnostics READ diagnostics NOTIFY diagnosticsChanged)
public:
    double angle;
    double confidence;
    QString current_station;
    //导航类型, 0 = 没有导航, 1 = 自由导航到任意点, 2 = 自由导航到站点, 3 = 路径导航到站点, 7 = 平动转动, 100 = 其他
    double task_type;
    double task_status;
    QString target_id;
public:
    RobotStatus();

    void setDefaultAddress(const QString &newDefaultAddress);
    void setDefaultTcpPort(quint16 newDefaultTcpPort);
    QString robotAddress() const { return defaultAddress; }
    QVariantMap diagnostics() const { return m_diagnostics; }
    bool hasMapMetadata() const { return mapReportedInStatus; }

    void set_default_all1_req_obj(bool return_laser);
    void robot_status_all1_req_get();
    void robot_status_info_req_get();
    void robot_status_run_req_get();
    void robot_status_loc_req_get();
    void robot_status_speed_req_get();
    void robot_status_block_req_get();
    void robot_status_battery_req_get();
    void robot_status_motor_req_get();
    void robot_status_laser_req_get();
    void robot_status_area_req_get();

    void robot_status_area_res_get();

    void robot_status_slam_req_get(const QJsonObject &obj);
    void robot_status_jack_req_get(const QJsonObject& obj);

    void robot_status_roller_req_get(const QJsonObject &obj);

    void robot_status_map_req_get();
    void robot_status_station_req_get();
    //void robot_status_downloadfile_req_get(const QJsonObject& obj);
    void robot_status_model_req_get(const QJsonObject& obj);
    void robot_status_params_req_get(const QJsonObject &obj);
    void robot_status_transparent_data_req_get(const QJsonObject &obj);
    void robot_status_armstatus_req_get(const QJsonObject &obj);
    void robot_status_alarm_req(const QJsonObject &obj);
    void robot_status_alarm_rep_get(const QJsonObject &obj);
    void robot_status_all1_req_get(const QJsonObject &obj);
    void robot_status_all2_req_get(const QJsonObject &obj);
    void robot_status_downloadfile_req_get(const QJsonObject &obj);
    void robot_status_script_info_req_get(const QJsonObject &obj);
    void robot_status_script_detailslist_req_get(const QJsonObject &obj);
    void robot_status_map_req_get(const QJsonObject &obj);
    void robot_status_battery_req_get(const QJsonObject &obj);
    void robot_status_sound_req_get(const QJsonObject &obj);
    void robot_status_motor_req_get(const QJsonObject &obj);
    void robot_status_laser_req_get(const QJsonObject &obj);
    void robot_status_emergency_req_get(const QJsonObject &obj);
    void robot_status_io_req_get(const QJsonObject &obj);
    void robot_status_imu_req_get(const QJsonObject &obj);
    void robot_status_rfid_req_get(const QJsonObject &obj);
    void robot_status_ultrasonic_req_get(const QJsonObject &obj);
    void robot_status_pgv_req_get(const QJsonObject &obj);
    void robot_status_encoder_req_get(const QJsonObject &obj);
    void robot_status_task_req_get(const QJsonObject &obj);
    void robot_status_task_status_package_req_get(const QJsonObject &obj);
    void robot_status_reloc_req_get(const QJsonObject &obj);
    void robot_status_loadmap_req_get(const QJsonObject &obj);
    void robot_status_fork_req_get(const QJsonObject &obj);
    void robot_status_dispatch_req_get(const QJsonObject &obj);
    void robot_status_all3_req_get(const QJsonObject &obj);
    void robot_status_current_lock_req_get(const QJsonObject &obj);

    void robot_status_mapmd5_req_get(const QJsonObject &obj);
    void robot_status_uploadfile_req_get(const QJsonObject &obj);
    void robot_status_listfile_req_get(const QJsonObject &obj);
    void robot_status_script_args_req_get(const QJsonObject &obj);
    void robot_status_armcalculate_req_get(const QJsonObject &obj);
    void robot_status_armtask_req_get(const QJsonObject &obj);
    void robot_status_armmove_req_get(const QJsonObject &obj);
    void robot_status_armoperation_req_get(const QJsonObject &obj);
    void robot_status_canframe_req_get(const QJsonObject &obj);
    void robot_status_gnsscheck_req_get(const QJsonObject &obj);
    void robot_status_gnss_list_req_get(const QJsonObject &obj);
    void robot_status_station_req_get(const QJsonObject &obj);
public Q_SLOTS:
    bool initConnection();
    bool checkConnection();
    void _SendDataForTest();
    void getError(QString msg);

signals:
    void robotAddressChanged();
    void diagnosticsChanged();
    void currentMapReported(const QString &name, const QString &revision);
    void updatePosition2();
    void updatePosition(double x, double y,double angle);
    void singal_sendToServerStatus(const QJsonObject&);
private:
    QVariantMap m_diagnostics;
    bool requestMapMetadata = true;
    bool mapReportedInStatus = false;
    void updateMapTelemetry(const QJsonObject &status);
    enum class StatusSound { None, Blocked, Navigation, Reached };
    StatusSound statusSound = StatusSound::None;
    void updateStatusSound(const QJsonObject &status);

    RProtocol m_protocol;
    QString defaultAddress;
    quint16 defaultTcpPort;
    QByteArray m_receiveData;//来自tcp返回的数据
    //默认请求
    QByteArray robot_status_all1_default_req_obj;
    void robot_status_info_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_run_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_loc_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_speed_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_block_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_battery_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_motor_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_laser_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_area_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_io_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_imu_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_emergency_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_rfid_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_ultrasonic_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_pgv_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_encoder_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_task_req_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_task_status_package_res(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_reloc_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_loadmap_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_slam_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_jack_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_fork_req_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_roller_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_dispatch_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_all3_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_current_lock_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_map_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_station_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_motor_req_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_params_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_uploadfile_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_downloadfile_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_listfile_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_script_info_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_script_detailslist_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_script_args_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_transparent_data_res(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_armstatus_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_armmove_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_armtask_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_armmove_rep_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_armoperation_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_canframe_res(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_gnsscheck_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_gnss_list_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_sound_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_model_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_alarm_res(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_alarm_res_receive(uint16_t revCommand, QByteArray& receivedata);
   
    void robot_status_all1_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_all2_res_receive(uint16_t revCommand, QByteArray& receivedata);
    void robot_status_transparent_data_res_receive(uint16_t revCommand, QByteArray& receivedata);
};
}


#endif // ROBOTSTATUS_H
