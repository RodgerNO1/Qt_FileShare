#include "fileserver.h"

FileServer::FileServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(sendFile()));
}

void FileServer::startFileServer(qint16 linstenport){
    tcpPort=linstenport;//监听端口

    loadSize = 1024;//缓冲区大小1K
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;

    if(!tcpServer->listen(QHostAddress::Any,tcpPort))//开始监听
    {
        qDebug() << tcpServer->errorString();
        tcpServer->close();
        return;
    }else qDebug() <<"---FileServer---开始监听端口："<<tcpPort;
}

void FileServer::setFile(QString filepath){
    localFile = new QFile(filepath);
    theFileName=localFile->fileName();
    if(!localFile->open((QFile::ReadOnly))){//以只读方式打开
        qDebug() <<"---FileServer---无法读取文件 ";
        return;
    }
}

void FileServer::sendFile(){
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection,SIGNAL(bytesWritten(qint64)),SLOT(updateClientProgress(qint64)));
    //bytesWritten SIGNAL 与 updateClientProgress SLOT 构成一个事件循环，直到文件发送完毕

    qDebug() <<"---FileServer---开始传送文件！";
    bytesToWrite=totalBytes = localFile->size();
    //文件总大小
    outBlock = localFile->read(qMin(bytesToWrite,loadSize));//从文件中读取数据到缓冲区
    bytesToWrite = totalBytes - clientConnection->write(outBlock);
    bytesWritten+=outBlock.size();
    qDebug()<<"---FileServer---已发送："<<bytesWritten<<" 剩余："<<bytesToWrite<<" 总共："<<totalBytes;
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    //发送成功后会发射SIGNAL(bytesWritten(qint64))

    int sentRate=((float)bytesWritten/totalBytes)*100;
    emit updateProgressBar(sentRate);//发射更新进度条信号

    outBlock.resize(0);


}
void FileServer::updateClientProgress(qint64 numBytes) //更新进度条，实现文件的传送
{
    bytesWritten +=numBytes;
    //已经发送数据的大小
    if(bytesToWrite > 0) //如果还有数据待发送
    {
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));//从文件中读取数据
      //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
      //就发送剩余数据的大小
        bytesToWrite -= (int)clientConnection->write(outBlock);
       //发送完一次数据后还剩余数据的大小
        int sentRate=((float)bytesWritten/totalBytes)*100;
        emit updateProgressBar(sentRate);//发射更新进度条信号

        outBlock.resize(0);
       //清空发送缓冲区
        qDebug()<<"---FileServer---已发送："<<bytesWritten<<" 剩余："<<bytesToWrite;
    }
    else//如果数据发送完毕
    {
        totalBytes = 0;
        bytesWritten = 0;
        bytesToWrite = 0;
        //归零
        localFile->close(); //如果没有发送任何数据，则关闭文件
        qDebug()<<tr("---FileServer---传送文件 %1 成功").arg(fileName);
        clientConnection->close();
//        emit finshTransfer(theFileName);

    }

}
FileServer::~FileServer(){
    delete tcpServer;
}
