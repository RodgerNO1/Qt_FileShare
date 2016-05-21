#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>

namespace Ui {
class ChatForm;
}

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QString title,QWidget *parent = 0);
    ~ChatForm();
    QString toHostName;

private:
    Ui::ChatForm *ui;
signals:
    void sendMsg(QString,QString);

public slots:
    void on_sendBtn();
    void on_closeBtn();
    void readMsg(QString fromhost,QString text);
};

#endif // CHATFORM_H
