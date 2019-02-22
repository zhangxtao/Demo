#ifndef GLOBALHOOK_H
#define GLOBALHOOK_H

#include <QObject>
#include <QtGlobal>
#include <QMutex>
#include <QSharedPointer>
#include <windows.h>

/**
  1、调用时 需将编译出来的.lib 和 .dll文件放入 .exe文件夹目录下
  2、调用工程文件需要添加对应的.lib文件路径到pro文件中
    LIBS += .../XXX.lib
  3、包含头文件 即可进行调用dll文件

  main.cpp中直接调用
  xHook.installMouseHook();
  xHook.installKeyHook();
  然后关联信号与槽函数
  connect;

  最后释放相应的hook

  全局键盘钩子 在钩取其他进程的某些按键时  会被杀毒软件当做是木马 给拦截

  */

#if defined(GLOBALHOOK_LIBRARY)
#  define GLOBALHOOKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GLOBALHOOKSHARED_EXPORT Q_DECL_IMPORT
#endif

#define xHook GlobalHook::instance().data()

class GLOBALHOOKSHARED_EXPORT GlobalHook : public QObject
{
    Q_OBJECT
private:
    GlobalHook(QObject *parent = 0);
    GlobalHook(const GlobalHook& hook) = delete;
    GlobalHook operator ==(const GlobalHook&) = delete;

public:
    virtual ~GlobalHook() final;
    static QSharedPointer<GlobalHook> &instance();

private:
    static QMutex m_mutex;
    static QSharedPointer<GlobalHook> m_hookInstance;

public:
    // 鼠标钩子 安装 卸载 钩子运行状态
    bool installMouseHook();
    bool uninstallMouseHook();
    bool isMouseHookRunning() const;
    // 键盘钩子
    bool installKeyHook();
    bool uninstallKeyHook();
    bool isKeyHookRunning() const;

signals:
    void sig_mouseEvent(PMOUSEHOOKSTRUCT pMouseHookStruct);
    void sig_keyEvent(PKBDLLHOOKSTRUCT pKeyHookStruct);

    // DLL 钩子接口
//#if define GLOBALHOOK_LIBRARY
public:
    void onMuoseEvent(PMOUSEHOOKSTRUCT pMouseHookStruct);
    void onKeyEvent(PKBDLLHOOKSTRUCT pKeyHookStruct);
//#endif
};

#endif // GLOBALHOOK_H
