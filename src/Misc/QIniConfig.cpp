#include "QIniConfig.h"

QIniConfig::QIniConfig():m_b_init_file(false),m_ini_file(nullptr),m_b_ConfigName(false),m_filename("")
{
    qDebug() << "Init Config";
}

QIniConfig* QIniConfig::m_iniConfig = nullptr;

QIniConfig* QIniConfig::GetInstance()
{
    if(!m_iniConfig)
    {
        m_iniConfig = new QIniConfig();
        return m_iniConfig;
    }
    return m_iniConfig;
}

QIniConfig::~QIniConfig()
{
   qDebug() << "Destroy Config";
   if(m_ini_file)
   {
       m_ini_file->close();
       delete m_ini_file;
       m_ini_file = nullptr;
   }
}

bool QIniConfig::Init(QString file_name)
{
    if(file_name.size()<1)
    {
        return false;
    }
    m_ini_file =new QFile(file_name);
     //支持读写操作:往后写
    if(m_ini_file->open(QIODevice::ReadWrite|QIODevice::Append))
    {
        m_filename = file_name;
        m_b_init_file = true; // 说明初始化成功
        IsConfigAttributeName();
        return true;
    }


    return false;
}

// 配置属性名 [attributeName]形式
bool QIniConfig::ConfigAttributeName(QString attr_name)
{
    if(attr_name.size()<1)
    {
        return false;
    }
    if(!m_ini_file->isOpen())
    {
        return false;
    }
    QString str_attribute = "[" + attr_name + "]" +"\n"; // 需要添加换行符，方便读取
    m_ini_file->write(str_attribute.toUtf8());
    m_b_ConfigName = true; //配置属性名成功
    return true;
}

bool QIniConfig::WriteToConfig(QString key_, QString val_)
{
    if(!m_ini_file)
    {
       return false;
    }
    if(!m_b_ConfigName)
    {
        return false;
    }
    QString str_key_val = key_ +"="+val_+"\n";
    m_ini_file->write(str_key_val.toUtf8());//写入
    return true;
}

bool QIniConfig::IsConfigAttributeName()
{
    QFile file_(m_filename);
    file_.open(QIODevice::ReadOnly);
    // 读取第一行即可[];//主要判断是否有[]
     QByteArray line = file_.readLine();
     QString str(line);
      qDebug() <<  str;
     if(str.size() < 1)
     {
         m_b_ConfigName = false;
     }
     else{
        if(str[0]==QString("[") && str[str.size()-2] ==QString("]"))
        {
            m_b_ConfigName = true;
        }
     }
    file_.close();
    return m_b_ConfigName;
}

QString QIniConfig::ReadValue(QString key_, QString attr_)
{
    QFile file_(m_filename);
    file_.open(QIODevice::ReadOnly);
    bool b_Found_Once = false;
    if(attr_ == "")
    {
        //等于空的情况，比较好处理，直接按行搜索即可
        while(!file_.atEnd())
        {
            QByteArray array_ = file_.readLine();
            QString str_(array_);
            if(str_.contains("="))
            {
                // 进行判断处理
                 str_.chop(1);//去掉空格
                 QStringList str_list = str_.split("=");
                 if(str_list[0] == key_)
                 {
                     qDebug() << "value is " << str_list[1];
                     file_.close();
                     return str_list[1];
                 }
            }
        }
        file_.close();
        return "";
    }
    else{
        while(!file_.atEnd())
        {

            QByteArray array_ = file_.readLine();
            QString str_(array_);
            // 此处为属性名
            if(IsConfigStr(str_))
            {
              if(b_Found_Once)
              {
                  file_.close();
                  return "";
              }
              QString str_configs =  GetConfigName(str_);
              if(str_configs == attr_)
              {
                  b_Found_Once = true;
                  qDebug() << "find";
              }
            }
            // 此处为key = value形式
            if(b_Found_Once)
            {
                if(str_.contains("="))
                {
                    // 进行判断处理
                     str_.chop(1);//去掉空格
                     QStringList str_list = str_.split("=");
                     if(str_list[0] == key_)
                     {
                          qDebug() << "value is " << str_list[1];
                         file_.close();
                         return str_list[1];
                     }
                }
            }
        }

        file_.close();
        return "";
    }
    file_.close();
    return "";

}

bool QIniConfig::IsConfigStr(QString str_)
{
    if(str_.size() < 3)
    {
        return false;
    }
    if(str_[0]==QString("[") && str_[str_.size()-2] == QString("]"))
    {
        return true;
    }
    return false;
}

QString QIniConfig::GetConfigName(QString str_)
{
    str_.chop(1);
    QString strss = str_.replace("[","");
    QString str_configName = strss.replace("]","");
    return str_configName;

}

