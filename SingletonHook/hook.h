#ifndef HOOK_H
#define HOOK_H

#include <QObject>
#include <windows.h>
#include <QDebug>

LRESULT CALLBACK SingletonHook(int n_Code, WPARAM wParam, LPARAM lParam);

// 单例钩子 线程级钩子
class Hook : public QObject
{
    Q_OBJECT
public:
    static Hook &GetSingletonHook();

    void InitHook();
    void RelreaseHook();

    void showText(const QString msg) const;

signals:
public slots:

private:
    Hook(QObject *parent = 0);
    static Hook singletonHook;
    bool m_bHookState;
};

#endif // HOOK_H
