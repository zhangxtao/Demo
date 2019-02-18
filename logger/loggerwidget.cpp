#include "loggerwidget.h"

Log4Qt::Logger *loggerWidgetFile = NULL;

void loggerwidget::initLoggerWidget(const QWidget* widget)
{
    if(loggerWidgetFile != NULL)
        return;

    Log4Qt::BasicConfigurator::configure();
    loggerWidgetFile = Log4Qt::Logger::rootLogger();
    loggerWidgetFile->removeAllAppenders();
    Log4Qt::WidgetAppender *appenderWidget = new Log4Qt::WidgetAppender();
    appenderWidget->setName("WidgetAppender");
    Log4Qt::TTCCLayout *layoutWidget = new Log4Qt::TTCCLayout(Log4Qt::TTCCLayout::TIME_ABSOLUTE);
    appenderWidget->setLayout(layoutWidget);
    appenderWidget->activateOptions();
    // 设置输出阈值
    appenderWidget->setThreshold(Log4Qt::Level::ALL_INT);

    // 设置日志信息输出的窗口组件
    appenderWidget->setLogWidget(widget);
    loggerWidgetFile->addAppender(appenderWidget);

    QThread::currentThread()->setObjectName("MainThread");
    Log4Qt::LogManager::setHandleQtMessages(true);

    QDir m_Savedir ;
    QDateTime dateNow = dateNow.currentDateTime();
    QString strPath = QDir::currentPath() + QString("/Log_%1-%2 %3/").arg(dateNow.date().year()).arg(dateNow.date().month()).
            arg(dateNow.date().day());

    if(!m_Savedir.exists(strPath))
        m_Savedir.mkpath(strPath);

    // 控制日志文本格式
    /**
     * @brief setConversionPattern
     * d 格式化日期
     * p 消息级别
     * n 平台相关分隔符
     */
    Log4Qt::PatternLayout *lay=new Log4Qt::PatternLayout(Log4Qt::PatternLayout::TTCC_CONVERSION_PATTERN);
    lay->setConversionPattern("%-d [%p] %m%n");
    lay->activateOptions();
    // 控制日志输出方式，控制台、文件、日滚动等
    Log4Qt::DailyRollingFileAppender* appender= new Log4Qt::DailyRollingFileAppender(lay,strPath+"/Log","_yyyy-MM-dd hh-mm'.log'");
    appender->setAppendFile(true);
    // 设置编码
    appender->setEncoding(QTextCodec::codecForName("UTF-8"));
    appender->setImmediateFlush(true);
    // 设置阈值级别为INFO
    appender->setThreshold(Log4Qt::Level::ALL_INT);
    appender->activateOptions();
    loggerWidgetFile->addAppender(appender);
}

void loggerwidget::colseLoggerWidget()
{
    if(loggerWidgetFile != NULL)
    {
        // 关闭 logger
        loggerWidgetFile->removeAllAppenders();
        loggerWidgetFile->loggerRepository()->shutdown();
        loggerWidgetFile = NULL;
    }
}
