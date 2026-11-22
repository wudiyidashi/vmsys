#ifndef QINICONFIG_H
#define QINICONFIG_H


#include <QFile>
#include <QString>
#include <QDebug>
#include <QMutex>

// 配置文件类
class QIniConfig{
public:
    QIniConfig();
    ~QIniConfig();

    static QIniConfig* GetInstance();
    bool Init(QString file_name);
    bool ConfigAttributeName(QString attr_name); //配置属性名
    bool WriteToConfig(QString key_,QString val_); //写入配置文件
    bool IsConfigAttributeName();//判断是否有[];从而判断是否已经设置配置属性名
    QString ReadValue(QString key_,QString attr_=""); //读取key的值 //没找到则返回""
    bool IsConfigStr(QString str_); // 判断是否为配置字符
    QString GetConfigName(QString str_); //从配置名格式中获取属性名[ii] 获取ii
private:
    bool m_b_init_file; // 是否初始化文件
    QFile* m_ini_file; //配置文件的句柄
    bool m_b_ConfigName; //是否配置属性名
    static QIniConfig* m_iniConfig;
    QString m_filename; //配置文件名：
};


#endif // QINICONFIG_H
