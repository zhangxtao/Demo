#include "readwriteconfigfile.h"

ReadWriteConfigFile::ReadWriteConfigFile(QObject *parent) : QObject(parent)
{
    iniSetting = NULL;
}

ReadWriteConfigFile::~ReadWriteConfigFile()
{
    if(iniSetting != NULL)
    {
        delete iniSetting;
        iniSetting = NULL;
    }
}

bool ReadWriteConfigFile::initIniFile()
{
    if(iniSetting == NULL)
    {
        iniSetting = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini",QSettings::IniFormat);
        iniSetting->setIniCodec(QTextCodec::codecForName("GB2312"));
    }
    return true;
}

int ReadWriteConfigFile::initXmlFile()
{
    QFile localFile(QCoreApplication::applicationDirPath()+"/config.xml");

    if(!localFile.exists())
    {
        qDebug() << "The appropriate configuration file was not found"
                 << QCoreApplication::applicationDirPath()+"/config.xml";
        return OPENXMLSTATE::FILEXML_NO_FOUND;
    }

    if(!localFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open config Xml ERROR";
        return OPENXMLSTATE::FILEXML_OPEN_ERR;
    }

    QXmlStreamReader xmlStreamReader(&localFile);
    while(!xmlStreamReader.atEnd())
    {
        QXmlStreamReader::TokenType type = xmlStreamReader.readNext();
        if(type == QXmlStreamReader::StartDocument)
        {
            continue;
        }else if(type == QXmlStreamReader::EndElement){
            continue;
        }else if(type == QXmlStreamReader::Characters){
            continue;
        }

        if(xmlStreamReader.name() == "name")
        {
            xmlStreamReader.readNext();
            qDebug() << "xml name:" << xmlStreamReader.text().toString();
        }

        if(xmlStreamReader.hasError())
        {
            qDebug() << QString::fromLocal8Bit("Err:%1 line:%2 column:%3")
                        .arg(xmlStreamReader.errorString())
                        .arg(xmlStreamReader.lineNumber())
                        .arg(xmlStreamReader.columnNumber());
            return OPENXMLSTATE::FILEXML_FORMAT_ERR;
        }

    }

    return OPENXMLSTATE::FILEXML_INIT_OK;
}

int ReadWriteConfigFile::initJsonFile(QList<SCANDEVICE*> &listDev)
{
    QFile localFile(QCoreApplication::applicationDirPath()+"/config.json");

    if(!localFile.exists())
    {
        qDebug() << "The appropriate configuration file was not found"
                 << QCoreApplication::applicationDirPath()+"/config.json";
        return OPENJSONSTATE::FILEJSON_NO_FOUND;
    }

    if(!localFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open config json ERROR";
        return OPENJSONSTATE::FILEJSON_OPEN_ERR;
    }

    QByteArray allData = localFile.readAll();
    qDebug() << "alldata " << allData;
    localFile.close();

    QJsonParseError jsonErr;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&jsonErr));
    if(jsonErr.error != QJsonParseError::NoError)
    {
        qDebug() << "file to Json Err";
        return OPENJSONSTATE::FILEJSON_TO_JSONDOC_ERR;
    }

    QJsonArray array = jsonDoc.array();

    // []多组数据 判断数组是否出错
    // 单组数据将不需要判断 jsonDoc.object()
    // jsonDoc.object().value("").toString;
    if(array.count() == 0)
    {
        qDebug() << "json format Err";
        return OPENJSONSTATE::FILEJSON_FORMAT_ERR;
    }

    for(int index=0;index<array.count();index++)
    {
        QJsonObject &obj = array.at(index).toObject();
        SCANDEVICE *dev = new SCANDEVICE;
        dev->DeviceID = obj["DeviceID"].toInt();
        dev->IP = obj["IP"].toString();
        dev->DevType = obj["devType"].toBool();
        dev->PingPongState = obj["pingpongState"].toBool();
        dev->FailureNum = obj["FailureNum"].toInt();
        dev->DeviceName = obj["deviceName"].toString();

        listDev.append(dev);

        qDebug() << "dev name:" << dev->DeviceName;
    }
    return OPENJSONSTATE::FILEJSON_INIT_OK;
}

QString ReadWriteConfigFile::readIniFile(QString Cmd)
{
    if(iniSetting == NULL)
        return "NULL";
    QMutexLocker locker(&mutexIni);
    // 读取失败返回"-1"
    return iniSetting->value(Cmd,"-1").toString();
}

void ReadWriteConfigFile::writeIniFile(QString Cmd, QString Data)
{
    if(iniSetting == NULL)
        return;
    QMutexLocker locker(&mutexIni);

    iniSetting->setValue(Cmd,Data);
}

int ReadWriteConfigFile::writeXmlFile()
{
    QFile localFile(QCoreApplication::applicationDirPath() + "/config.xml");

    if(!localFile.exists())
    {
        qDebug() << "The appropriate configuration file was not found"
                 << QCoreApplication::applicationDirPath() + "/config.xml";
        return OPENXMLSTATE::FILEXML_NO_FOUND;
    }

    if(!localFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open config Xml ERROR";
        return OPENXMLSTATE::FILEXML_OPEN_ERR;
    }

    QXmlStreamWriter xmlStreamWrite(&localFile);

    xmlStreamWrite.setAutoFormatting(true);
    xmlStreamWrite.writeStartDocument();

    xmlStreamWrite.writeStartElement("config");
    xmlStreamWrite.writeTextElement("name",QString("Rapoo"));

    xmlStreamWrite.writeEndElement();
    xmlStreamWrite.writeEndDocument();
    localFile.close();

    return OPENXMLSTATE::FILEXML_INIT_OK;
}

int ReadWriteConfigFile::writeJsonFile()
{
    QFile localFile(QCoreApplication::applicationDirPath() + "/config.json");
    if(!localFile.exists())
    {
        return OPENJSONSTATE::FILEJSON_NO_FOUND;
    }

    if(!localFile.open(QIODevice::WriteOnly))
    {
        return OPENJSONSTATE::FILEJSON_OPEN_ERR;
    }

    localFile.write(strToJson().toUtf8());

    // 修改json配置文件 比较麻烦


    localFile.close();
    return OPENJSONSTATE::FILEJSON_INIT_OK;
}

QString ReadWriteConfigFile::strToJson()
{
    QJsonObject AbnormalJsonObj,jsonObj;
    QJsonArray jsonCode;
    jsonCode.insert(0,"Mouse");
    jsonCode.insert(1,"10N");

    jsonObj.insert("Type",jsonCode);
    jsonObj.insert("date","0225");

    AbnormalJsonObj.insert("req",QJsonValue(jsonObj));
    AbnormalJsonObj.insert("seq",11);
    AbnormalJsonObj.insert("name","Rapoo");

    QJsonDocument document;
    document.setObject(AbnormalJsonObj);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    QString json(array);

    return json;
}
