#include "globalhook.h"

// 静态变量
QMutex GlobalHook::m_mutex;
QSharedPointer<GlobalHook> GlobalHook::m_hookInstance;
// 全局变量
HINSTANCE g_hInstance = Q_NULLPTR;
HHOOK g_hMouseHook = Q_NULLPTR;
HHOOK g_hKeyHook = Q_NULLPTR;

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL, // DLL句柄
        DWORD fdwReason,    // 调动情况
        LPVOID lpReserved);
// 鼠标钩子处理函数
LRESULT CALLBACK MouseProc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam);
// 键盘钩子处理函数
LRESULT CALLBACK KeyProc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam);

GlobalHook::GlobalHook(QObject *parent)
    :QObject(parent)
{

}

GlobalHook::~GlobalHook()
{
    uninstallKeyHook();
    uninstallMouseHook();
}

QSharedPointer<GlobalHook> &GlobalHook::instance()
{
    if(m_hookInstance.isNull())
    {
        QMutexLocker locker(&m_mutex);
        if(m_hookInstance.isNull())
            m_hookInstance = QSharedPointer<GlobalHook>(new GlobalHook());
    }
    return m_hookInstance;
}

bool GlobalHook::installMouseHook()
{
    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL,(HOOKPROC)MouseProc,g_hInstance,0);
    return Q_LIKELY(g_hMouseHook);
}

bool GlobalHook::uninstallMouseHook()
{
    if(Q_UNLIKELY(!g_hMouseHook))
    {
        return true;
    }

    bool success = false;
    success = UnhookWindowsHookEx(g_hMouseHook);
    if(success)
    {
        g_hMouseHook = Q_NULLPTR;
    }

    return success;
}

bool GlobalHook::isMouseHookRunning() const
{
    return Q_LIKELY(g_hMouseHook);
}

bool GlobalHook::installKeyHook()
{
    g_hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyProc,g_hInstance,0);
    return Q_LIKELY(g_hKeyHook);
}

bool GlobalHook::uninstallKeyHook()
{
    if(Q_UNLIKELY(!g_hKeyHook))
    {
        return true;
    }

    bool success = false;
    success = UnhookWindowsHookEx(g_hKeyHook);
    if(success)
    {
        g_hKeyHook = Q_NULLPTR;
    }

    return success;
}

bool GlobalHook::isKeyHookRunning() const
{
    return Q_LIKELY(g_hKeyHook);
}


//#if define GLOBALHOOK_LIBRARY
void GlobalHook::onMuoseEvent(PMOUSEHOOKSTRUCT pMouseHookStruct)
{
    emit sig_mouseEvent(pMouseHookStruct);
}
void GlobalHook::onKeyEvent(PKBDLLHOOKSTRUCT pKeyHookStruct)
{
    emit sig_keyEvent(pKeyHookStruct);
}
//#endif

// 鼠标钩子处理函数
LRESULT CALLBACK MouseProc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam)
{
    PMOUSEHOOKSTRUCT pmshs = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);
    xHook->onMuoseEvent(pmshs);
    return CallNextHookEx(g_hMouseHook,nCode,wParam,lParam);
}

// 键盘钩子处理函数
LRESULT CALLBACK KeyProc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam)
{
    PKBDLLHOOKSTRUCT pkbhs = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
    xHook->onKeyEvent(pkbhs);
    return CallNextHookEx(g_hKeyHook,nCode,wParam,lParam);
}

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,
        DWORD fdwReason,
        LPVOID lpReserved)
{
    Q_UNUSED(lpReserved);

    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
    {
        g_hInstance = hinstDLL;
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
    {
        g_hInstance = Q_NULLPTR;
        break;
    }
    }

    return TRUE;
}

