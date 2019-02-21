#include "TextEditAutoScroll.h"

TextEditAutoScroll::TextEditAutoScroll(QWidget *parent)
    :QTextEdit(parent)
{
    MaxLine = 0;
    connect(this, SIGNAL(textChanged()), this, SLOT(slotAutoScroll()));
	this->setAutoFillBackground(true);//用于填充颜色

    QPalette pl = this->palette();
    // 设置背景色调
    pl.setBrush(QPalette::Base,QBrush(Qt::black));
    this->setPalette(pl);
    // 设置纵向滚动条
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 设置控件只读属性
	this->setReadOnly(true);
    // 取消控件光标
	this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setEnabled(false);
	this->setWindowModified(false);
}

TextEditAutoScroll::~TextEditAutoScroll()
{

}

void TextEditAutoScroll::addText(QString text_t, Qt::GlobalColor color_t)
{
    numTooMoreHandle();
    QTextCursor cursor(this->textCursor());
    format.clearForeground();
    format.setForeground(QBrush(color_t));

    this->setEnabled(true);
    cursor.setCharFormat(format);
    cursor.insertText(text_t+ "\r\n");
    this->setEnabled(false);
    MaxLine++;
}
void TextEditAutoScroll::addText(QString textHead,QString textContent, Qt::GlobalColor color_t)
{
    numTooMoreHandle();
	
    this->setEnabled(true);
    QTextCursor cursor(this->textCursor());
    format.clearForeground();
    format.setForeground(QBrush(Qt::yellow));
    cursor.setCharFormat(format);
	QDateTime dtm = QDateTime::currentDateTime();
	QString msg = dtm.toString("yyyy-MM-dd HH:mm:ss");
	QString str = msg + textHead;
	cursor.insertText(str);
	
    format.setForeground(QBrush(color_t));
    cursor.setCharFormat(format);
    cursor.insertText( " "+ textContent + "\r\n");
    this->setEnabled(false);
    MaxLine++;
}
void TextEditAutoScroll::numTooMoreHandle()
{
    if(MaxLine > MAX_SHOWLINE)
    {
        MaxLine = 0;
        this->clear();
    }
}
void TextEditAutoScroll::slotAutoScroll()
{
    QTextCursor cursor =  this->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);
}

