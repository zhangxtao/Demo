#ifndef HOOK_H
#define HOOK_H

#include <QObject>
#include <windows.h>
#include <QDebug>
#include <QThread>

/**
 *
 *  调用方法
 * // 获取对象
 *  singletonHook = Hook::GetSingletonHook();
 * // 初始化
 *  singletonHook->InitHook();
 *
 * // 释放钩子对象
 * singletonHook->RelreaseHook();
 *
 */


LRESULT CALLBACK SingletonHook(int n_Code, WPARAM wParam, LPARAM lParam);

// 单例钩子 线程钩子
class Hook : public QObject
{
    Q_OBJECT
public:
    Hook(QObject *parent = 0);
    ~Hook();
    static Hook *GetSingletonHook();

    void InitHook();
    void RelreaseHook();

    void showText(const QString msg) const;

signals:
public slots:

private:

    static Hook *singletonHook;
    bool m_bHookState;
};

#endif // HOOK_H
