#include "sendbroadcastform.h"
#include "ui_sendbroadcastform.h"

SendBroadcastForm::SendBroadcastForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendBroadcastForm)
{
    ui->setupUi(this);
    setWindowTitle("广播");
    setFixedSize(this->geometry().width(),this->geometry().height());//设置固定大小
    connect(ui->sendBtn,SIGNAL(clicked()),this,SLOT(onSendBtn()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(onCancelBtn()));
}
void SendBroadcastForm::onSendBtn(){
    emit broadcastMsg(ui->textEdit->toPlainText());
    ui->textEdit->clear();
}
void SendBroadcastForm::onCancelBtn(){
    close();
}
SendBroadcastForm::~SendBroadcastForm()
{
    delete ui;
}
