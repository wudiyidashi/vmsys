#ifndef MCUBOARD_H
#define MCUBOARD_H

#include <QObject>

#include <IO/TcpSocketClient.h>
#include <functional>
namespace boardDevice {
    namespace mcuBoard {
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
        signals:
           
             void stateChange(QAbstractSocket::SocketState);
        private:
            io::TcpSocketClient* m_tcp_connection;
//            QTcpSocket m_tcp_connection;
            QByteArray m_lastmessage;
            std::function<void(QByteArray& data)> frameParse;//解析函数
        };
    }

    class McuBoard : public QObject
    {
        Q_OBJECT
    public:
        bool drawer_stat;
    public:
        explicit McuBoard(QObject *parent = nullptr);

        bool init();
        bool getConnectionState() const;
        void setConnectionState(bool newConnectionState);

        void DoorControl(const QByteArray& door_index);
    public slots:
        void drawerControl(bool drawer_stat);
    signals:
        void red_line(QByteArray);
        void connectionStateChanged();

    private slots:
        void slot_queryBoard();
        void slot_connectionStateChange(QAbstractSocket::SocketState state);
    private:
        QTimer timer;
        bool connectionState=false;
        mcuBoard::Protocol protocol;
        void frameParse(QByteArray& data);
        Q_PROPERTY(bool connectionState READ getConnectionState WRITE setConnectionState NOTIFY connectionStateChanged)
    };
}

#endif // MCUBOARD_H
