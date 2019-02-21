#ifndef TEXTEDITAUTOSCROLL_H
#define TEXTEDITAUTOSCROLL_H

#include <QWidget>
#include <QTextEdit>
#include <QDateTime>

#define MAX_SHOWLINE 100

class TextEditAutoScroll : public QTextEdit
{
    Q_OBJECT
public:
    // /|\-
    TextEditAutoScroll(QWidget *parent);
    ~TextEditAutoScroll();
    // text尾部追加输出 color_t显示字体颜色
    void addText(QString text_t, Qt::GlobalColor color_t);
    // text尾部追加输出 textHead黄色字体显示 textCountent字体颜色由color_t决定
    void addText(QString textHead,QString textContent, Qt::GlobalColor color_t);
private:
    // 显示字体格式
    QTextCharFormat format;
    // 最大显示行数
    uint MaxLine;
    // 清空输出框
    void numTooMoreHandle();
public slots:
    // 滚动条自动下拉到底部
    void slotAutoScroll();
};


#endif // TextEditAUTOSCROLL_H
