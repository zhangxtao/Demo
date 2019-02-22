#include "hook.h"

Hook *Hook::singletonHook = new Hook();
HHOOK m_hHook;

Hook::Hook(QObject *parent) : QObject(parent)
{
    m_bHookState = false;
}

Hook::~Hook()
{
    if(m_bHookState)
    {
        UnhookWindowsHookEx(m_hHook);
        m_bHookState = false;
    }
    if(singletonHook != NULL)
    {
        delete singletonHook;
        singletonHook = NULL;
    }
}

Hook *Hook::GetSingletonHook()
{
    return singletonHook;
}

void Hook::InitHook()
{
    if(!m_bHookState)
    {
        // 设置钩子
        /**
            SetWindowsHookEx
            1、钩子类型 2、回调函数地址 3、实例句柄 4、线程id
            return 失败返回NULL 成功返回处理过程句柄
            WH_MOUSE_LL 底层鼠标钩子
            WH_KETBOARD_LL 底层鼠标钩子
          */
        m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL,SingletonHook,GetModuleHandle(NULL),0);
        if(m_hHook != NULL)
            m_bHookState = true;
    }
}

void Hook::RelreaseHook()
{
    if(m_bHookState)
    {
        if(m_hHook != NULL)
        {
            // 释放钩子
            UnhookWindowsHookEx(m_hHook);
            m_hHook = NULL;
            m_bHookState = false;
        }
    }
}
// 测试接口
void Hook::showText(const QString msg) const
{
    qDebug() << "Hook msg:" << msg;
}

LRESULT CALLBACK SingletonHook(int n_Code, WPARAM wParam, LPARAM lParam)
{
    // 鼠标结构
    LPMSLLHOOKSTRUCT pMouse = (LPMSLLHOOKSTRUCT)lParam;
    // 键盘结构
    PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
    Hook *localHook = Hook::GetSingletonHook();

    switch(n_Code)
    {
    case HC_ACTION:{
        // 捕获到的鼠标事件
        if(wParam == WM_MOUSEMOVE)
        {
            localHook->showText(QString("Move %1:%2").arg(pMouse->pt.x).arg(pMouse->pt.y));
        }else if(wParam == WM_LBUTTONDOWN){
            localHook->showText(QString("WM_LBUTTONDOWN"));
        }else if(wParam == WM_LBUTTONUP){
            localHook->showText(QString("WM_LBUTTONUP"));
        }else if(wParam == WM_RBUTTONDOWN){
            localHook->showText(QString("WM_RBUTTONDOWN"));
        }else if(wParam == WM_RBUTTONUP){
            localHook->showText(QString("WM_RBUTTONUP"));
        }else if(wParam == WM_MBUTTONDOWN){
            localHook->showText(QString("WM_MBUTTONDOWN"));
        }else if(wParam == WM_MBUTTONUP){
            localHook->showText(QString("WM_MBUTTONUP"));
        }
        // 键盘事件
        if(wParam == WM_KEYDOWN || wParam == WM_KEYUP)
        {
            qDebug() << "Normal " << pKey->vkCode << pKey->scanCode;
        }else if(wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP)
        {
            // Alt
            qDebug() << "Sys " << pKey->vkCode << pKey->scanCode;
        }
    }
    }

    // 继续钩子
    return CallNextHookEx(m_hHook, n_Code, wParam, lParam);
}
