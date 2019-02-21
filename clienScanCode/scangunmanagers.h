#ifndef TCPMANAGERS_H
#define TCPMANAGERS_H
/// RGV扫码枪
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QThread>
#include "tcpsocket.h"

typedef struct SCANDEVINFO{
    int devId;              // 设备id
    int state;              // 设备当前状态
    QString strDevName;     // 设备名称
    QString strCode;        // 扫码数据
}SCANDEVINFO;

typedef struct SCANDEVICE
{
    bool ScanCodeType;          // 设备类型<两款设备 一款扫码数值有后缀.康耐视false 一款没有后缀.霍尼韦尔true>
    bool flagPingPongRunning;   // 滴答状态
    bool flagConnect;           // 连接状态
    int numReconnectTimes;      // 断线重连次数<10次连接不上断线重连，每次1秒>
    Tcpsocket *socket;          // tcp连接对象
    QString IP;                 // 设备ip号
    SCANDEVINFO info;           // 设备信息
}SCANDEVICE;
// 当前设备状态
enum SCANSTATE{
    SCAN_GUN_NO_HAVE_ID     = -1,        // 未找的扫码设备
    SCAN_GUN_RESET          =  1,        // 当前设备复位状态
    SCAN_GUN_BUSY           =  2,        // 当前设备繁忙状态
    SCAN_GUN_OK             =  3,        // 当前设备扫码成功状态
    SCAN_GUN_FAILED         =  4         // 当前设备扫码失败状态
};
// 请求扫码 返回执行状态
enum REQUESTSCANSTATE{
    REQUEST_GUN_OK          =  0,        // 请求扫码成功<设备处于复位状态，请求成功后将进入繁忙状态>
    REQUEST_GUN_NO_HAVE_DEV = -1,        // 设备不存在
    REQUEST_GUN_NO_CONNECT  = -2,        // 设备连接失败,网络异常
    REQUEST_GUN_NO_RESET    = -3         // 设备尚未复位<多次请求>
};

class ScanGunManagers : public QObject
{
    Q_OBJECT
public:
    ScanGunManagers(QObject *parent = 0);
    ~ScanGunManagers();

    int StartScanCodeDevice(int scandeviceid);      // 开启设备扫码<可重复请求，但只有设备处于复位状态才会执行扫码动作>
    bool resetDeviceState(int scandeviceid);        // 复位设备状态
    SCANDEVINFO GetDeviceInfo(int scandeviceid);    // 获取扫码设备当前状态及信息

protected:
    void DeleteSocketList();                        // 释放所有连接内存
    Tcpsocket *AddSocket(QString);                  // 添加连接对
    void writeResultIniConfig(int scandevindex,QString ScanCode);           // 将获取到的扫码数据写入配置文件中

signals:
    void sig_OpenJsonFileErr();                                             // json配置文件打开失败

private slots:
    void slot_ScanCode(int,QString);                // 获取到读码器数据
    void slot_initSocket();                         // 初始化获取json配置文件

private:
    QList<SCANDEVICE *> m_TcpSocketList;            // 设备链表
    QSettings *ScanCodeSetting;                     // 扫码配置文件，预防断电。再次重启保留当前扫码状态
    QMutex mutexSettings;                           // 文件锁
    QThread *m_thread;
};

#endif // TCPMANAGERS_H
