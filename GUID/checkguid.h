#ifndef CHECKGUID_H
#define CHECKGUID_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QTextCodec>
#include <QProcess>
#include <QNetworkInterface>
#include <Windows.h>

/**
 * @brief main.cpp
 * if(!CheckGUID::CheckIniAndSysGUID())
    {
        // 身份校验失败
        QMessageBox::warning(0,("Authorization failure"),
                             ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"),0,0,0);

        CheckGUID::releaseFile();
        return 0;

    }
 *
 *
 */

extern QSettings *setting;

class CheckGUID
{
public:
    // GUID校验
    static bool CheckIniAndSysGUID();
    // 释放文件
    static void releaseFile();

private:
    // 获取配置文件中GUID
    static QString GetIniGUID();
    // 获取系统参数 生成GUID
    static QString GetSysGUID();
    // 获取硬盘物理序列号
    static bool GetHDID(PCHAR pIDBufer);

};

#endif // CHECKGUID_H
