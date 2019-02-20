#include "checkguid.h"

QSettings *setting = NULL;

CheckGUID::CheckGUID()
{

}

bool CheckGUID::CheckIniAndSysGUID()
{
    if(GetIniGUID() == GetSysGUID())
    {
        return true;
    }else{
        return false;
    }
}

void CheckGUID::releaseFile()
{
    if(setting != NULL)
    {
        delete setting;
        setting=NULL;
    }
}

QString CheckGUID::GetIniGUID()
{
    if(setting == NULL)
    {
        setting = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini",QSettings::IniFormat);
        setting->setIniCodec(QTextCodec::codecForName("GB2312"));
    }

    return setting->value("GUID","-1").toString();
}

QString CheckGUID::GetSysGUID()
{
    QString CheckID = "";
    // CPU
    QString cpu_id = "";
    QProcess p(0);
    p.start("wmic CPU get ProcessorID");
    p.waitForStarted();
    p.waitForFinished();
    cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
    cpu_id = cpu_id.remove("ProcessorId").trimmed();

    CheckID = cpu_id.remove(0,8);
    CheckID.insert(4,"-");
    CheckID.insert(9,"-");
    // MAC
    QStringList mac_list;
    QString strMac;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    for (int i=0; i<ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        // 过滤掉本地回环地址、没有开启的地址
        if (iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            // 过滤掉虚拟地址
            if (!(iface.humanReadableName().contains("VMware",Qt::CaseInsensitive)))
            {
                strMac = iface.hardwareAddress();
                mac_list.append(strMac);
            }
        }
    }
    for(int i=0;i<mac_list.count();i++)
    {
        if(QString(mac_list.at(i)).replace(":","").length() == 12)
            CheckID += QString(mac_list.at(i)).replace(":","").insert(4,"-").insert(9,"-").insert(14,"-");
    }
    // HDID
    CHAR szHDID[256];
    if(GetHDID(szHDID))
    {
        CheckID += QString(szHDID).simplified().insert(4,"-");
    }else{
        qDebug() << "获取硬盘序列号失败!";
    }

    qDebug() << CheckID;
    return CheckID;
}

bool CheckGUID::GetHDID(PCHAR pIDBufer)
{
    HANDLE hDevice=NULL;
    hDevice=::CreateFileA("\\\\.\\PhysicalDrive0",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
    if(!hDevice)
        hDevice=::CreateFileA("\\\\.\\Scsi0",GENERIC_READ |GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);

    if(!hDevice)
        return FALSE;

    DWORD dwBytesReturned=0;
    GETVERSIONINPARAMS gVersionParsams;
    ZeroMemory(&gVersionParsams,sizeof(GETVERSIONINPARAMS));


    if(!DeviceIoControl(hDevice,SMART_GET_VERSION,NULL,NULL,&gVersionParsams,sizeof(GETVERSIONINPARAMS),&dwBytesReturned, NULL)
            || dwBytesReturned==0 || gVersionParsams.bIDEDeviceMap <= 0)
    {
        ::CloseHandle(hDevice);
        return FALSE;
    }

    SENDCMDINPARAMS scip;

    ZeroMemory( &scip,sizeof(SENDCMDINPARAMS));
    scip.cBufferSize=IDENTIFY_BUFFER_SIZE;
    scip.irDriveRegs.bSectorCountReg=1;
    scip.irDriveRegs.bSectorNumberReg=1;
    scip.irDriveRegs.bDriveHeadReg=0xA0;
    scip.irDriveRegs.bCommandReg=0xEC;

    BYTE btBuffer[1024]={ 0 };

    if(!DeviceIoControl(hDevice,SMART_RCV_DRIVE_DATA,&scip,sizeof(SENDCMDINPARAMS),
                        btBuffer,1024,&dwBytesReturned,NULL))
    {
        ::CloseHandle(hDevice);
        return FALSE;
    }

    int nPos=0x24;
    while(btBuffer[nPos]<128)
    {
        *pIDBufer=btBuffer[nPos++];
        pIDBufer++;
    }
    *pIDBufer=00;
    ::CloseHandle(hDevice);
    return TRUE;
}

