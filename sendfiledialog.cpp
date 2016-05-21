#include "ui_sendfiledialog.h"
#include "sendfiledialog.h"
#include "fileserver.h"
#include <QFileDialog>
#include <QDebug>
SendFileDialog::SendFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFileDialog)
{
    ui->setupUi(this);
    ui->sendBtn->setEnabled(false);
    setFixedSize(this->geometry().width(),this->geometry().height());

    fileServer=new FileServer;
    QObject::connect(fileServer,SIGNAL(updateProgressBar(int)),this,SLOT(updateProgress(int)));

    QObject::connect(ui->openBtn, SIGNAL(clicked()), this, SLOT(on_Open()));
    QObject::connect(ui->sendBtn, SIGNAL(clicked()), this, SLOT(on_Send()));
    QObject::connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(on_Close()));

}
void SendFileDialog::setLabelText(QString text){
     ui->label->setText(text);
}

void SendFileDialog::on_Open()  //打开
{
    theFilePath = QFileDialog::getOpenFileName(this);
    QFileInfo fileinfo(theFilePath);
    if(!theFilePath.isEmpty())
    {
//        theFileName = theFilePath.right(theFilePath.size() - theFilePath.lastIndexOf('/')-1);
        theFileName=fileinfo.fileName();
        theFileSize=fileinfo.size();
        ui->label->setText(tr("要传送的文件为：%1 ").arg(theFileName));
        ui->sendBtn->setEnabled(true);
    }
    qDebug()<<"---SendFileDialog---on_Open";
}

void SendFileDialog::on_Send()  //发送
{

    ui->label->setText(tr("等待对方接收... ..."));
    ui->openBtn->setEnabled(false);
    emit sendFileByPath(theFilePath);//发送信息到接收者

    fileServer->setFile(theFilePath);
    fileServer->startFileServer(45456);

    qDebug()<<"---SendFileDialog---on_Send";
}

void SendFileDialog::on_Close()//退出
{
    qDebug()<<"---SendFileDialog---on_Close";
    this->close();
}
void SendFileDialog::updateProgress(int value){
    ui->progressBar->setValue(value);
    if(value==100)ui->label->setText(tr("发送成功"));
}

SendFileDialog::~SendFileDialog()
{
    delete fileServer;
    delete ui;
}
