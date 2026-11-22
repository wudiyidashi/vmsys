#ifndef LIFT_H
#define LIFT_H

#include <QObject>

#include <IO/TcpSocketClient.h>
#include <functional>
namespace boardDevice {
    namespace lift {
        class Protocol : public QObject
        {
            Q_OBJECT
        public:
            void init(std::function<void(QByteArray& data)> frameParse);
            Protocol();
            ~Protocol();
            void wirte(const QByteArray& data);
        public slots:
            void receiveReadyRead();
        private:
            io::TcpSocketClient* m_tcp_connection;

            QByteArray m_lastmessage;
            std::function<void(QByteArray& data)> frameParse;//解析函数
        };
    }

    class Lift : public QObject
    {
        Q_OBJECT
    public:
        bool drawer_stat;
    public:
        explicit Lift(QObject *parent = nullptr);

        bool init();
    public slots:

    signals:
    private:
        lift::Protocol protocol;
        void frameParse(QByteArray& data);
    };
}

#endif // LIFT_H
