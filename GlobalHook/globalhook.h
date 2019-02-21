#ifndef GLOBALHOOK_H
#define GLOBALHOOK_H

#include <QObject>
#include <QtGlobal>
#include <QMutex>
#include <QSharedPointer>
#include <windows.h>
#include <QDebug>

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
