#ifndef READWRITECONFIGFILE_H
#define READWRITECONFIGFILE_H
// 配置文件
// ini xml json
#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QTextCodec>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QXmlStreamReader>
#include <QDebug>

struct SCANDEVICE{
    int DeviceID;
    QString IP;
    bool DevType;
    bool PingPongState;
    int FailureNum;
    QString DeviceName;
};

enum OPENJSONSTATE{
    FILEJSON_INIT_OK        = 0,    // 操作成功
    FILEJSON_NO_FOUND       = -1,   // 文件不存在
    FILEJSON_OPEN_ERR       = -2,   // 打开文件失败
    FILEJSON_TO_JSONDOC_ERR = -3,   // 转换类型失败
    FILEJSON_FORMAT_ERR     = -4    // 文件内容格式有误
};

enum OPENXMLSTATE{
    FILEXML_INIT_OK     = 0,
    FILEXML_NO_FOUND    = -1,
    FILEXML_OPEN_ERR    = -2,
    FILEXML_FORMAT_ERR  = -3
};


class ReadWriteConfigFile : public QObject
{
    Q_OBJECT
public:
    explicit ReadWriteConfigFile(QObject *parent = 0);
    ~ReadWriteConfigFile();

    // 初始化读取配置文件
    bool initIniFile();
    int initXmlFile();
    int initJsonFile(QList<SCANDEVICE*> &listDev);

    // ini文件读写
    QString readIniFile(QString Cmd);
    void writeIniFile(QString Cmd,QString Data);

    // json Xml写文件
    int writeXmlFile();
    int writeJsonFile();

    // json数据包生成
    QString strToJson();

signals:

public slots:

private:
    // ini文件锁
    QMutex mutexIni;
    // ini文件句柄
    QSettings *iniSetting;
};

#endif // READWRITECONFIGFILE_H
