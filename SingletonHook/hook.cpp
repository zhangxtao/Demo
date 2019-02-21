#include "hook.h"

Hook Hook::singletonHook;
HHOOK m_hHook;
Hook::Hook(QObject *parent) : QObject(parent)
{
    m_bHookState = false;
}

Hook &Hook::GetSingletonHook()
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
          */
        m_hHook = SetWindowsHookEx(WH_MOUSE_LL,SingletonHook,GetModuleHandle(NULL),0);
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

void Hook::showText(const QString msg) const
{
    qDebug() << "Hook msg:" << msg;
}

LRESULT CALLBACK SingletonHook(int n_Code, WPARAM wParam, LPARAM lParam)
{
    LPMSLLHOOKSTRUCT pMouse = (LPMSLLHOOKSTRUCT)lParam;
    Hook &localHook = Hook::GetSingletonHook();

    switch(n_Code)
    {
    case HC_ACTION:{
        // 捕获到的鼠标事件
        if(wParam == WM_MOUSEMOVE)
        {
            localHook.showText(QString("Move %1:%2").arg(pMouse->pt.x).arg(pMouse->pt.y));
        }else if(wParam == WM_LBUTTONDOWN){
            localHook.showText(QString("WM_LBUTTONDOWN"));
        }else if(wParam == WM_LBUTTONUP){
            localHook.showText(QString("WM_LBUTTONUP"));
        }else if(wParam == WM_RBUTTONDOWN){
            localHook.showText(QString("WM_RBUTTONDOWN"));
        }else if(wParam == WM_RBUTTONUP){
            localHook.showText(QString("WM_RBUTTONUP"));
        }else if(wParam == WM_MBUTTONDOWN){
            localHook.showText(QString("WM_MBUTTONDOWN"));
        }else if(wParam == WM_MBUTTONUP){
            localHook.showText(QString("WM_MBUTTONUP"));
        }
    }
    }

    // 继续钩子
    return CallNextHookEx(m_hHook, n_Code, wParam, lParam);
}
