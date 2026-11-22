#include "DataProcessUtl.h"


//校验数据是按照16进制的形式
uint16_t IO::DataProcessUtl::CRC16Data(const QString& data)
{
    QByteArray before_send_data;     //要校验的数据
    before_send_data = QByteArray::fromHex(data.toLatin1()); //要校验的数据
    static quint8 array[500];                  //定义一个校验数组
    //校验算法
    for(int i = 0;i<before_send_data.count();i++)
    {
        array[i] = before_send_data.mid(i,1).toHex().toInt(0,16);
    }
    quint16 data_crc = ccitt_crc16_Buf(array,before_send_data.count());     //校验和
    return data_crc;
}

//校验的数据是按16进制的形式
uint16_t IO::DataProcessUtl::CRC16Data(const QByteArray &data)
{
    const char *dataptr=data.constData();
    quint16 data_crc = ccitt_crc16_Buf((uint8_t*)dataptr,data.count());     //校验和
    return data_crc;
}
