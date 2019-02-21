#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTcpSocket>
#include <QTimerEvent>
#include <QThread>
#include <QTime>
#include <QtNetwork/qabstractsocket.h>

const char ReadScanCmd[3] = {0x16,0x54,0x0D};   // 开启读取数据
const quint16 Port = 5050;                      // 服务器端口
#define READ_SCAN_CODE_TIMEOUT 2500             // 读取数据超时 两款设备都设置为超时返回NG字符提示
#define CONNECT_TMIEOUT 100                     // 连接超时时间
#define PINGR_TMIE_NO_ANSER 4000                // 滴答 4秒无通信就ping
#define MAX_RECONNECT_TIMES 5                   // 最大重连次数
#define MAX_SCANCODE_ERROR_TIMES 2              // 最大扫码次数
#define MAX_PINGPONG_ERROR_TIMES 1              // 最大滴答失败次数
#define MAX_SCANCODE_LENGTH 12                  // 扫码数据长度

class Tcpsocket : public QObject
{
    Q_OBJECT
public:
    explicit Tcpsocket(QObject *parent = 0);
    ~Tcpsocket();

    bool ConnectToServer(QString Ip);                           // 连接服务器
    void timerEvent(QTimerEvent *event);                        // 超时处理
    bool WriteScanCmd(bool flag);                               // 发送命令
    void SetSocketID(int id){m_socketID = id;}                  // 设置当前连接的序号
    void setPingPong(bool flag);                                // 设置pingpong状态
    void setScanCodeNum(int num){m_maxScanCodeErrNum = num;}    // 设置最大扫码失败次数


signals:
    void sig_ScanCode(int,QString);             // 上传扫码数据
    void sig_StartWriteScanCode(bool);              // 开始发送数据
    void sig_StartConnectToServer();

public slots:
    void slot_WriteScanCmd(bool);                   // 发送数据
    void slot_ReadScanCode();                   // 获取数据

    void slot_stateChanged(QAbstractSocket::SocketState);
    void slot_ConnectToServer();

private:
    QString m_strServerIP;              // 保存服务器ip地址，预防断线重连
    int m_cError;                       // 读取扫码数据失败次数<获取条码数据有错>
    int m_cRepeat;                      // 未收到读取数据次数<两款设备都可设置超时提示符"NG">
    int m_socketID;                     // socket编号
    int m_TimerReadOutID;               // 超时定时器
    int m_TimerPingPongID;              // 询问连接状态
    int m_PingPongErrNum;               // 滴答失败次数
    bool m_bisPing;                     // 当前是否是ping操作
    bool m_bisConnect;                  // 是否连接服务器
    bool m_bisPingPong;                 // 是否滴答标志位

    int m_maxScanCodeErrNum;            // 最大扫码次数
    bool m_flagStartScanCode;           // 当前是否请求开始扫码
    QTime timeLastCommunication;        // 上次有操作时间

    QTcpSocket *m_socket;               // socket连接
    int m_ReConnectNum;                 // 当前断线重新次数
};

#endif // TCPSOCKET_H
