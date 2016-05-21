#include "downloader.h"
#include<QHostAddress>
Downloader::Downloader(QObject *parent) : QObject(parent)
{

    tcpClient = new QTcpSocket(this);
    tcpPort = 45455;
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(readMessage()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(closeConnect()));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
}

void Downloader::download(QString filesavepath,qint64 filesize,QString ipAddress,qint16 Port){
    qDebug()<<"----Downloader---文件保存到："<<filesavepath;
    localFile=new QFile(filesavepath);
    thefileSize=filesize;

    if(!localFile->open(QFile::WriteOnly)){
        qDebug()<<"----Downloader---无法读取文件 ";
        return;
    }
    hostAddress = QHostAddress(ipAddress);
    tcpPort=Port;
    newConnect();
}
void Downloader::newConnect()
{
    TotalBytes = 0;
    bytesReceived = 0;
    blockSize = 0;

    tcpClient->abort();
    tcpClient->connectToHost(hostAddress,tcpPort);
    time.start();
}
void Downloader::readMessage()
{
    float useTime = time.elapsed();

    bytesReceived += tcpClient->bytesAvailable();
    inBlock = tcpClient->readAll();
    localFile->write(inBlock);
    inBlock.resize(0);

//    double speed = bytesReceived / useTime;
    int downedRate=((float)bytesReceived/thefileSize)*100;
    emit updateProgressBar(downedRate);//发射更新进度条信号
    qDebug()<<tr("----Downloader---已接收 %1B(%2\%)用时：%3").arg(bytesReceived)
                                                           .arg(downedRate)
                                                           .arg((float)useTime/1000);

//    qDebug()<<tr("----Downloader---已接收 %1B (%2MB/s)")
//                                   .arg(bytesReceived)//已接收
//                                   .arg(speed*1000/(1024*1024),0,'f',2);//速度

    if(bytesReceived==thefileSize){
        qDebug()<<tr("----Downloader---接收完毕");
        localFile->close();   //接收完文件后，一定要关闭，不然可能出问题
        tcpClient->close();

        emit finshDownload(fileName);//发送完毕信号
    }

}
void Downloader::closeConnect(){
    qDebug()<<tr("----Downloader---连接关闭");
    localFile->close();   //接收完文件后，一定要关闭，不然可能出问题
    tcpClient->close();
}

void Downloader::displayError(QAbstractSocket::SocketError socketError) //错误处理
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError : break;
        default : qDebug() << tcpClient->errorString();
    }
}
Downloader::~Downloader(){
      delete tcpClient;
}
