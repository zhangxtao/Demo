#include "scangunmanagers.h"

ScanGunManagers::ScanGunManagers(QObject *parent)
    :QObject(parent)
{
    m_TcpSocketList.clear();
    m_thread = NULL;

    ScanCodeSetting = new QSettings(QCoreApplication::applicationDirPath()+"/ScanCodeDeviceConfig.ini",QSettings::IniFormat);
    // 延时创建扫码对象
    QTimer::singleShot(1000,this,&ScanGunManagers::slot_initSocket);
}

ScanGunManagers::~ScanGunManagers()
{
    qDebug() << "~ScanGunManagers start";
    DeleteSocketList();

    delete ScanCodeSetting;
    ScanCodeSetting=NULL;

    if(m_thread != NULL)
    {
        m_thread->exit();
        m_thread->wait(3000);
        delete m_thread;
    }
    qDebug() << "~ScanGunManagers exit";
}
// 删除对应扫码组
void ScanGunManagers::DeleteSocketList()
{
    foreach(SCANDEVICE *scandev,m_TcpSocketList)
    {
        disconnect(scandev->socket,&Tcpsocket::sig_ScanCode,this,&ScanGunManagers::slot_ScanCode);
        // 使正在重连的线程退出重连循环
        scandev->socket->deleteLater();
    }
}
// 添加扫码配对组
Tcpsocket *ScanGunManagers::AddSocket(QString IP)
{
    if(m_thread == NULL)
    {
        m_thread = new QThread;
    }
    Tcpsocket *socket = new Tcpsocket();
    connect(socket,&Tcpsocket::sig_ScanCode,this,&ScanGunManagers::slot_ScanCode,Qt::QueuedConnection);
    socket->moveToThread(m_thread);
    socket->ConnectToServer(IP);
    m_thread->start();
    return socket;
}
// 将配置扫码数据写入配置文件中
void ScanGunManagers::writeResultIniConfig(int scandevindex, QString ScanCode)
{
    QMutexLocker locker(&mutexSettings);
    // 将扫码数据写入配置文件中
    ScanCodeSetting->setValue(QString("ScanCodeDevice%1").arg(scandevindex),ScanCode);
}
// 开始扫码设备
int ScanGunManagers::StartScanCodeDevice(int scandeviceid)
{
    foreach(SCANDEVICE *scandev,m_TcpSocketList)
    {
        if(scandev->info.devId == scandeviceid)
        {
            // 只有当前设备状态处于复位状态，才会向扫码枪发送扫码请求
            if(scandev->info.state == SCANSTATE::SCAN_GUN_RESET)
            {
                // 获取对应的连接，并开启扫码
                if(scandev->socket->WriteScanCmd(true))
                {
                    // 当前处于繁忙状态
                    scandev->info.state = SCANSTATE::SCAN_GUN_BUSY;
                    // 与服务器正常连接
                    return REQUESTSCANSTATE::REQUEST_GUN_OK;
                }else{
                    // 已断开与服务端的连接
                    return REQUESTSCANSTATE::REQUEST_GUN_NO_CONNECT;
                }
            }else{
                return REQUESTSCANSTATE::REQUEST_GUN_NO_RESET;
            }
        }
    }
    // 未找到相对应的设备
    return REQUESTSCANSTATE::REQUEST_GUN_NO_HAVE_DEV;
}
// 获取扫码枪数据
SCANDEVINFO ScanGunManagers::GetDeviceInfo(int scandeviceid)
{
    SCANDEVINFO info;
    foreach(SCANDEVICE *scandev,m_TcpSocketList)
    {
        if(scandev->info.devId == scandeviceid)
        {
            return scandev->info;
        }
    }
    info.devId = scandeviceid;
    info.state = SCANSTATE::SCAN_GUN_NO_HAVE_ID;
    return info;
}
// 复位扫码枪状态
bool ScanGunManagers::resetDeviceState(int scandeviceid)
{
    foreach(SCANDEVICE *scandev,m_TcpSocketList)
    {
        if(scandev->info.devId == scandeviceid)
        {
            scandev->info.state = SCANSTATE::SCAN_GUN_RESET;
            scandev->info.strCode.clear();
            // 清空配置文件中相应的扫码缓存数据
            writeResultIniConfig(scandev->info.devId,"");
            return true;
        }
    }
    return false;
}

void ScanGunManagers::slot_initSocket()
{
    QFile loadFile(QCoreApplication::applicationDirPath()+"/ScanConfig.json");

    if(!loadFile.exists())
    {
        qDebug() << "The appropriate configuration file was not found";
        emit sig_OpenJsonFileErr();
        return;
    }

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open config json ERROR";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonRrr;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&jsonRrr));
    if(jsonRrr.error != QJsonParseError::NoError)
    {
        qDebug() << "json error";
        return;
    }

    QJsonArray array = jsonDoc.array();

    if(array.count()==0)
        qDebug() << "Err:InitScanCode Socket loadJsonFile";

    for(int i=0;i<array.count();i++)
    {
        QJsonObject& obj = array.at(i).toObject();

        SCANDEVICE *dev     = new SCANDEVICE;
        dev->IP             = obj["IP"].toString();
        dev->info.devId     = obj["DeviceID"].toInt();
        dev->socket         = AddSocket(dev->IP);
        dev->info.state     = SCANSTATE::SCAN_GUN_RESET;
        dev->ScanCodeType   = obj["devType"].toBool();
        dev->numReconnectTimes  = 0;
        dev->info.strDevName = obj.value("DeviceName").toString().toUtf8();
        dev->flagConnect    = false;
        qDebug() << obj["IP"].toString() << "dev->deviceID ="<<dev->info.devId<< "deviceName:" << dev->info.strDevName;

        // 滴答标志位
        if(obj["PingPongState"].toBool())
            dev->flagPingPongRunning = true;
        else
            dev->flagPingPongRunning = false;


        dev->socket->setPingPong(dev->flagPingPongRunning);

        // 扫码失败次数
        dev->socket->setScanCodeNum(obj["FailureNum"].toInt());
        // 获取配置文件中扫码缓存
        dev->info.strCode = ScanCodeSetting->value(QString("ScanCodeDevice%1").arg(dev->info.devId)).toString();
        if(!dev->info.strCode.isEmpty())
        {
            if(dev->info.strCode == "ERROR")
                dev->info.state = SCANSTATE::SCAN_GUN_FAILED;
            else
                dev->info.state = SCANSTATE::SCAN_GUN_OK;
        }
        dev->socket->SetSocketID(dev->info.devId);
        m_TcpSocketList.append(dev);
    }
    return;
}

void ScanGunManagers::slot_ScanCode(int id,QString msg)
{
    qDebug() << "id:" << id << "msg:" << msg;
    foreach(SCANDEVICE *scandev,m_TcpSocketList)
    {
        if(scandev->info.devId == id && scandev->info.state == SCANSTATE::SCAN_GUN_BUSY)
        {
            if(msg == "NG")
            {
                writeResultIniConfig(id,"ERROR");
                scandev->info.strCode = "ERROR";
                // <1、条码异常>
                scandev->info.state = SCANSTATE::SCAN_GUN_FAILED;
            }else if(msg == "FF"){
                writeResultIniConfig(id,"ERROR");
                scandev->info.strCode = "ERROR";
                // <2、扫码器异常>
                scandev->info.state = SCANSTATE::SCAN_GUN_FAILED;
            }else{
                // 数据缓存 会重复请求扫码结果
                scandev->info.strCode = msg;
                scandev->info.state = SCANSTATE::SCAN_GUN_OK;
                writeResultIniConfig(id,msg);
                // 打包数据
            }
        }
    }
}

