#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>
#include <IO/TcpSocketClient.h>
#include <functional>
namespace boardDevice {
    namespace bat {
        class Protocol : public QObject
        {
            Q_OBJECT
        public:
            bool init(std::function<void(QByteArray& data)> frameParse);
            Protocol();
            ~Protocol();
            void wirte(const QByteArray& data);
            bool isConnected() const;
        public slots:
            void receiveReadyRead();
        signals:
            void connectionStateChanged();
        private:
            io::TcpSocketClient* m_tcp_connection;

            QByteArray m_lastmessage;
            std::function<void(QByteArray& data)> frameParse;//解析函数
        };
    }

    class Bat : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(quint8 power READ getPower WRITE setPower NOTIFY powerChanged)
        Q_PROPERTY(bool connectionState READ isConnected NOTIFY connectionStateChanged)
    public:
        bool drawer_stat;
        quint8 power=70;
        explicit Bat(QObject *parent = nullptr);

        bool init();
        bool isConnected() const;
        quint8 getPower() const;
        void setPower(quint8 newPower);

    public slots:

        void realTimeReq();
        void startTimerReq();
    signals:
        void powerChanged();
        void connectionStateChanged();
        void bat_charge_stat(QByteArray);

    private:
        bat::Protocol protocol;
        QTimer timer;
        void frameParse(QByteArray& data);

    };
}

struct BmsCMD
{
    uint8_t func:7;//功能
    uint8_t dic:1;//bit7=0 时为查询, bit7=1 时为应答
};
#endif // BATTERY_H
