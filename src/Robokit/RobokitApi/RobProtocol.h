#ifndef ROBOTPROTOCOL_H
#define ROBOTPROTOCOL_H

#include <QObject>

#include <stdint.h>
#include <QTime>
#include <QElapsedTimer>
#include <QDeadlineTimer>
#include <QHash>
#include <QSet>
#include <functional>
//#include <Misc/Log.h>
#include <IO/TcpSocketClient.h>


namespace robokit {


enum ProbocolTableErrors  {
    OK = 0,
    OVER_TIME = 1,
    FORMAT_ERROR = 2,
};

using protocol_cb_t = std::function<void(uint16_t, QByteArray&)>;

class RProtocol : public QObject
{
    Q_OBJECT
public:
    explicit RProtocol(QObject *parent = nullptr);
    explicit RProtocol(io::TcpSocketClient*, QObject* parent = nullptr);
    bool initTcpConnection(const QHostAddress &address,const quint16 port);
    bool WriteTcpData(uint16_t sendCommand,
        const QByteArray& sendData,
        protocol_cb_t callback, bool matchResponse = false);
    qint64 getProtocolState();
    void SetNnalyzeCallBackFunc(protocol_cb_t callback);
public slots:
    void getError(QString msg);

signals:
    void errorMessage(QString msg);
    void stateChange(QAbstractSocket::SocketState);
private:
    io::TcpSocketClient* m_tcpConnection;

    protocol_cb_t m_receiveFinishCallback = nullptr;
    ProbocolTableErrors m_status;
    QByteArray m_message_cache;

    QByteArray m_lastmessage;//存放所有读取的数据
    QElapsedTimer m_elapsedtime;//用来监测时间
    QString m_lasterror;
    quint16 nextRequestNumber = 10000;
    struct MatchedReply {
        quint16 command;
        QDeadlineTimer deadline;
        protocol_cb_t callback;
    };
    QHash<quint16, MatchedReply> matchedReplies;
    QSet<quint16> matchedResponseTypes;
    int m_timeout = 3000;
    bool callBackFunNoModify = true;
private slots:
    void receiveTcpReadyRead();

};
}
#endif // ROBOTPROTOCOL_H
