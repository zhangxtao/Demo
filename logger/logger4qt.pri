#####
##   本地日志 可导向文件 也可导向窗口内输出位置
##   窗口控件需要实现一个槽函数 --onAppendLog(const QString&)
#####

# 定义所需的宏
DEFINES += LOG4QT_LIBRARY

# 将Log4Qt源码工程下的src/log4qt目录拷贝到自己的工程src目录中
# 定义Log4Qt源码根目录
LOG4QT_ROOT_PATH = $$PWD/log4qt

# 指定编译项目时应该被搜索的#include目录
INCLUDEPATH += $$LOG4QT_ROOT_PATH

# 将Log4Qt源代码添加至项目中
include($$LOG4QT_ROOT_PATH/log4qt.pri)

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

SOURCES += $$PWD/loggerwidget.cpp

HEADERS += $$PWD/loggerwidget.h
