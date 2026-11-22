#include "CRC.h"

//校验数据是按照16进制的形式
//UINT16 CRC16Data(QString data)
//{
//    QByteArray before_send_data;     //要校验的数据
//    before_send_data = QByteArray::fromHex(data.toLatin1()); //要校验的数据
//    quint8 array[before_send_data.count()];                  //定义一个校验数组

//    //校验算法
//    for(int i = 0;i<before_send_data.count();i++)
//    {
//        array[i] = before_send_data.mid(i,1).toHex().toInt(0,16);
//    }
//    quint16 data_crc = ccitt_crc16_Buf(array,sizeof(array));     //校验和
//    return data_crc;
//}
//校验的数据是按16进制的形式
UINT16 CRC16Data(QByteArray data)
{

  //  quint16 data_crc = ccitt_crc16_Buf(data);     //校验和
  //  return data_crc;
    return 0;
}
