#include "tcpsocket.h"

Tcpsocket::Tcpsocket(QObject *parent):QObject(parent)
{
    m_socket = NULL;
    m_strServerIP   = "";
    m_ReConnectNum  = 0;
    m_TimerReadOutID   = 0;
    m_TimerPingPongID  = 0;
    m_bisConnect    = false;
    m_bisPingPong   = false;
    m_flagStartScanCode = false;
    m_cRepeat       = 0;
    m_cError        = 0;
    m_PingPongErrNum = 0;
    m_maxScanCodeErrNum = 0;
    timeLastCommunication = QTime::currentTime();
    connect(this,&Tcpsocket::sig_StartWriteScanCode,this,&Tcpsocket::slot_WriteScanCmd,Qt::QueuedConnection);
    connect(this,&Tcpsocket::sig_StartConnectToServer,this,&Tcpsocket::slot_ConnectToServer);

}

Tcpsocket::~Tcpsocket()
{
    qDebug() << "~Tcpsocket start";
    if(m_TimerReadOutID != 0)
    {
        killTimer(m_TimerReadOutID);
    }
    if(m_TimerPingPongID != 0)
    {
        killTimer(m_TimerPingPongID);
    }
    if(m_socket != NULL)
    {
        delete m_socket;
    }
    qDebug() << "~Tcpsocket exit";
}
// 初始化连接服务器
bool Tcpsocket::ConnectToServer(QString Ip)
{
    if(m_socket == NULL)
    {
        m_strServerIP = Ip;
        emit sig_StartConnectToServer();
        return true;
    }
    return false;
}
// 超时函数
void Tcpsocket::timerEvent(QTimerEvent *event)
{
    // 扫码超时
    if(event->timerId() == m_TimerReadOutID)
    {
        if(m_TimerReadOutID != 0)
        {
            killTimer(m_TimerReadOutID);
            m_TimerReadOutID = 0;
        }
        if(m_TimerPingPongID != 0 && !m_flagStartScanCode)
        {
            m_bisPing = false;
            m_PingPongErrNum++;
            return ;
        }
        // 网络正常 扫码器异常，连续三次能正常连接上服务器，但发送扫码命令没有返回值，说明扫码器出问题
        if(++m_cRepeat > m_maxScanCodeErrNum)
        {
            // 读取失败
            emit sig_ScanCode(m_socketID,QString("FF"));
            m_flagStartScanCode = false;
            m_cRepeat = 0;
            return;
        }else{
            emit sig_StartWriteScanCode(true);
        }
    }
    // 滴答超时
    if(event->timerId() == m_TimerPingPongID)
    {
        if ( m_flagStartScanCode || timeLastCommunication.msecsTo(QTime::currentTime()) <= PINGR_TMIE_NO_ANSER - 1000)
        {
            return;
        }
        m_bisPing = true;
        // 连续2次滴答无反应 重连
        if(m_PingPongErrNum > MAX_PINGPONG_ERROR_TIMES)
        {
            if(m_TimerReadOutID != 0)
            {
                qDebug() << "killTimer m_TimerReadOutID" ;
                killTimer(m_TimerReadOutID);
                m_TimerReadOutID = 0;
            }
            m_PingPongErrNum = 0;

            disconnect(m_socket,&QTcpSocket::readyRead,this,&Tcpsocket::slot_ReadScanCode);
            m_cRepeat = 0;
            // 等待超时 断开连接
            m_socket->disconnectFromHost();
            return;
        }
        emit sig_StartWriteScanCode(false);
        qDebug() << m_socketID << "ping";
    }
}
//
bool Tcpsocket::WriteScanCmd(bool flag)
{
    if(flag)
        m_cRepeat = 0;
    // 判断当前连接是否正常
    if(m_bisConnect)
    {
        m_cError = 0;
        m_flagStartScanCode = true;
        emit sig_StartWriteScanCode(true);
        return true;
    }else{
        m_flagStartScanCode = false;
        return false;
    }
}
// 设置当前连接是否滴答
void Tcpsocket::setPingPong(bool flag)
{
    m_bisPingPong = flag;
}
// 开始扫码
void Tcpsocket::slot_WriteScanCmd(bool flag)
{
    if(flag)
    {
        qDebug() << "------------------------------slot_WriteScanCmd----------ScanCode Request-------";
//        if(m_TimerPingPongID != 0)
//            killTimer();
    }else{
        // 滴答
        qDebug() << "------------------------------slot_WriteScanCmd----------PingPong---------------";
    }

    m_socket->write(ReadScanCmd);
    m_TimerReadOutID = startTimer(READ_SCAN_CODE_TIMEOUT);
    m_socket->flush();
}
// 获取扫码器数据
void Tcpsocket::slot_ReadScanCode()
{
    qDebug() << "slot_ReadScanCode";
    QByteArray buffer;
    // 暂停读码超时
    if(m_TimerReadOutID != 0)
    {
        killTimer(m_TimerReadOutID);
        m_TimerReadOutID = 0;
    }

    timeLastCommunication = QTime::currentTime();
    // 读取缓冲区数据
    buffer = m_socket->readAll();
    qDebug() << "buffer:" << buffer;

    QString ScanCodeMsg = buffer;
    ScanCodeMsg.replace("\r\n","");
    // 对数据处理
    if(!ScanCodeMsg.isEmpty())
    {
        if(m_TimerPingPongID != 0 && m_bisPing)
        {
            // 收到pong操作
            m_bisPing = false;
            m_PingPongErrNum = 0;
            return;
        }
        if(!m_flagStartScanCode)
            return;
        // 扫码数据存在超时 and 数据包长度不符合应有的长度
        if(ScanCodeMsg == "NG" || ScanCodeMsg.length() != MAX_SCANCODE_LENGTH)
        {
            // 三次连续NG条码异常
            if(m_cError++ >= m_maxScanCodeErrNum)
            {
                emit sig_ScanCode(m_socketID,"NG");
                m_flagStartScanCode = false;
                return;
            }
            if(!m_bisPingPong)
            {
            // 启动再次扫码命令
                qDebug() << "sig_StartWriteScanCode();";
                emit sig_StartWriteScanCode(true);
            }

        }else{
            if(ScanCodeMsg.length() == MAX_SCANCODE_LENGTH)
            {
                // 无后缀扫码设备
                emit sig_ScanCode(m_socketID,ScanCodeMsg);
                m_flagStartScanCode = false;
            }else{
                qDebug() << "buffer length error" << ScanCodeMsg.length() << ScanCodeMsg;
            }

        }
    }
}
// 当前连接状态
void Tcpsocket::slot_stateChanged(QAbstractSocket::SocketState state)
{
    if(QAbstractSocket::UnconnectedState == state)
    {
        // 断开连接 启动重连
        m_socket->abort();
        m_bisConnect = false;
        qDebug() << "slot_stateChanged connect to Host:" << m_strServerIP;
        disconnect(m_socket,&QTcpSocket::readyRead,this,&Tcpsocket::slot_ReadScanCode);
        m_socket->connectToHost(m_strServerIP, Port);
    }
    if(QAbstractSocket::ConnectedState == state)
    {
        // 连接上服务器 根据滴答标志位 开启 or 关闭
        if(m_bisPingPong)
        {
            // 开启滴答
            if(m_TimerPingPongID == 0)
                m_TimerPingPongID = startTimer(PINGR_TMIE_NO_ANSER);

        }else{
            // 无滴答
            if(m_TimerPingPongID != 0)
                killTimer(m_TimerPingPongID);
        }
        if(m_flagStartScanCode)
            emit sig_StartWriteScanCode(true);
        qDebug() << m_strServerIP << "Connected successfully " << "ID:" << m_socketID;
        m_bisConnect = true;
        connect(m_socket,&QTcpSocket::readyRead,this,&Tcpsocket::slot_ReadScanCode,Qt::QueuedConnection);
    }
}

void Tcpsocket::slot_ConnectToServer()
{
    if(m_socket == NULL)
    {
        m_socket = new QTcpSocket;
        connect(m_socket,&QTcpSocket::stateChanged,this,&Tcpsocket::slot_stateChanged);
        m_socket->connectToHost(m_strServerIP,Port);
    }
}
