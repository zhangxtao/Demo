#ifndef LOGGERWIDGET_H
#define LOGGERWIDGET_H

#include <QCoreApplication>
#include <QTextCodec>
#include <log4qt/logger.h>
#include <log4qt/ttcclayout.h>
#include <log4qt/fileappender.h>
#include <log4qt/loggerrepository.h>
#include <log4qt/basicconfigurator.h>
#include <log4qt/WidgetAppender.h>
#include <log4qt/logmanager.h>
#include <log4qt/dailyrollingfileappender.h>
#include "log4qt/patternlayout.h"
#include <QThread>
#include <QWidget>
#include <QDir>

extern Log4Qt::Logger *loggerWidgetFile;

class loggerwidget
{
public:
    // 初始化logger资源
    static void initLoggerWidget(const QWidget* widget);
    // 释放logger
    static void colseLoggerWidget();
};



#endif // LOGGERWIDGET_H
