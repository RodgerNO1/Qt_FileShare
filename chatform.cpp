#include "chatform.h"
#include "ui_chatform.h"
#include <QDateTime>
#include<QHostInfo>
ChatForm::ChatForm(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);
    setWindowTitle(title);
    setFixedSize(this->geometry().width(),this->geometry().height());//设置固定大小
    toHostName=title;
    connect(ui->sendBtn,SIGNAL(clicked()),this,SLOT(on_sendBtn()));
    connect(ui->closeBtn,SIGNAL(clicked()),this,SLOT(on_closeBtn()));
}
void ChatForm::readMsg(QString fromhost,QString text){
    QString time = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
    ui->textBrowser->setTextColor(Qt::blue);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("[ " +fromhost+" ] "+ time);

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("楷体",12));
    ui->textBrowser->append(text);
}

void ChatForm::on_sendBtn(){
    QString text=ui->textEdit->toPlainText();
    QString time = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
    QString myhost=QHostInfo::localHostName();
    ui->textBrowser->setTextColor(Qt::blue);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("[ " +myhost+" ] "+ time);

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("楷体",12));
    ui->textBrowser->append(text);
    ui->textEdit->clear();
    emit sendMsg(text,toHostName);
}
void ChatForm::on_closeBtn(){
    close();
}
ChatForm::~ChatForm()
{
    delete ui;
}
