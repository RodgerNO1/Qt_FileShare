#include "manager.h"
#include"mainwindow.h"

#include<QFileInfo>
#include<QFile>
#include<QTextStream>
#include<QFileIconProvider>
#include<QDebug>
#include<QDir>
#include<QTemporaryFile>
#include<QStandardItemModel>

Manager::Manager(QObject *parent):QObject(parent)
{

    //新建数据文件夹
    QString temppath=QDir::currentPath()+"/temp";
    QString datapath=QDir::currentPath()+"/data";
    QString Def_SavePath=QDir::currentPath()+"/Download";
    QDir dir;
    if(!dir.exists(temppath)){
        if(dir.mkdir(temppath)){
            qDebug()<<"mrdir:"<<temppath;
        }
    }else{
        qDebug()<<temppath<<"temppath:exists";
    }
    if(!dir.exists(datapath)){
        if(dir.mkdir(datapath)){
            qDebug()<<"mrdir:"<<datapath;
        }
    }else{
        qDebug()<<datapath<<"datapath:exists";
    }
    if(!dir.exists(Def_SavePath)){
        if(dir.mkdir(Def_SavePath)){
            qDebug()<<"mrdir:"<<Def_SavePath;
        }
    }else{
        qDebug()<<Def_SavePath<<"Def_SavePath:exists";
    }

}
void Manager::deleteTempDir(){
    QString temppath=QDir::currentPath()+"/temp";
    QDir dir(temppath);
    QFileInfoList fileList = dir.entryInfoList();
     foreach (QFileInfo fi, fileList) {
         if(fi.isFile()){
             fi.dir().remove(fi.fileName());
             qDebug()<<"remove:"<<fi.fileName();
         }

     }
}

QIcon Manager::getHostIcon(){
    return QIcon("://image/host_icon.png");
}
QIcon Manager::getDownIcon(){
    return QIcon("://image/down_icon.png");
}
QIcon Manager::getCancelIcon(){
    return QIcon("://image/cancel_icon.png");
}
QIcon Manager::getAddIcon(){
    return QIcon("://image/add_icon.png");
}
QIcon Manager::getSendIcon(){
    return QIcon("://image/send_icon.png");
}
QIcon Manager::getChatIcon(){
    return QIcon("://image/chat_icon.png");
}

QIcon Manager::getFileIcon(const QString filename){

    QFileIconProvider icon_provider;
    QIcon icon;

    QTemporaryFile tmp_file(QDir::currentPath()+"/temp/XXXXXX"+filename);
    tmp_file.setAutoRemove(false);

    if(tmp_file.open())
    {
        icon = icon_provider.icon(QFileInfo(tmp_file));
        qDebug()<<"getFileIcon:file_name:"<<tmp_file.fileName();

    }else{
        icon = icon_provider.icon(QFileInfo("file.data"));
        qDebug()<<"getFileIcon:fail,return default";
    }
    tmp_file.close();
    return icon;
}
QList<SharedFile> Manager::getShareFileList(){
    QList<SharedFile> ls;
    QFile file(QDir::currentPath()+"/data/a.data");
    if(file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream out(&file);
        QString sa,sb,sc,sd;
        int i=0;
        while(!out.atEnd()){
            out>>sa>>sb>>sc>>sd;

            ls.insert(i++,SharedFile(sa,sb,sc,sd));
        }

        file.close();
    }
    return ls;
}
QList<MySharedFile> Manager::getMySharedFileList(){
    QList<MySharedFile> ls;
    QFile file(QDir::currentPath()+"/data/mysharefile.data");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QDataStream out(&file);
        QString filename,filesize,filedate,filesharer,filepath;

        int i=0;
        while(!out.atEnd()){
            out>>filename>>filesize>>filedate>>filesharer>>filepath;
            MySharedFile mySharedFile(filename,filesize,filedate,filesharer,filepath);
            ls.insert(i++,mySharedFile);
            MainWindow::myShareFileMap->insert(mySharedFile.sFileName,mySharedFile);
            qDebug()<<"---getMySharedFileList::MainWindow::myShareFileMap add"<<mySharedFile.sFileName;
        }

        file.close();
    }
    return ls;
}

void Manager::addMyShareFile(MySharedFile mySharedFile){
    QFile file(QDir::currentPath()+"/data/mysharefile.data");
    if(file.open(QIODevice::Append | QIODevice::Text)) {
        QDataStream out(&file);
        out<<mySharedFile.sFileName
            <<mySharedFile.sFileSize
            <<mySharedFile.sFileDate
            <<mySharedFile.sFileSharer
            <<mySharedFile.sFilePath;

        file.close();
    }
    MainWindow::myShareFileMap->insert(mySharedFile.sFileName,mySharedFile);

    qDebug()<<"---addMyShareFile::insert finsh";
}
void Manager::cancelMyShareFile(QString fn){

    MainWindow::myShareFileMap->remove(fn);

    QFile f(QDir::currentPath()+"/data/mysharefile.data");
    f.remove();//删除文件重新写入
    QFile file(QDir::currentPath()+"/data/mysharefile.data");

    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QDataStream out(&file);

        QMap<QString, MySharedFile>::iterator it;

        for (it= MainWindow::myShareFileMap->begin(); it != MainWindow::myShareFileMap->end(); ++it){
            out<<it.value().sFileName
                <<it.value().sFileSize
                <<it.value().sFileDate
                <<it.value().sFileSharer
                <<it.value().sFilePath;
        }

        file.close();
        qDebug()<<"---Manager::cancelMyShareFile2";
    }
}
void Manager::readSharedFileList(){
    qDebug()<<"readSharedFileList()";
    QList<SharedFile> ls=getShareFileList();

    for(int i=0;i<ls.length()-1;i++){

        MainWindow::SharedFileModel->setItem(i,0,new QStandardItem( getFileIcon("a.exe"),ls.at(i).sFileName));
        MainWindow::SharedFileModel->setItem(i,1,new QStandardItem(ls.at(i).sFileSize));
        MainWindow::SharedFileModel->setItem(i,2,new QStandardItem(ls.at(i).sFileDate));
        MainWindow::SharedFileModel->setItem(i,3,new QStandardItem(ls.at(i).sFileSharer));
        qDebug()<<"readSharedFileList:"<<ls.at(i).sFileName;
    }

}
void Manager::readMySharedFileList(){

    QList<MySharedFile> ls=getMySharedFileList();

    for(int i=0;i<ls.length();i++){

        MainWindow::MySharedFileModel->setItem(i,0,new QStandardItem( getFileIcon(ls.at(i).sFileName),ls.at(i).sFileName));
        MainWindow::MySharedFileModel->setItem(i,1,new QStandardItem(ls.at(i).sFileSize));
        MainWindow::MySharedFileModel->setItem(i,2,new QStandardItem(ls.at(i).sFileDate));
        MainWindow::MySharedFileModel->setItem(i,3,new QStandardItem(ls.at(i).sFileSharer));
    }

}
void Manager::readDownloadList(){

//    QList<SharedFile> ls= getShareFileList();

//    for(int i=0;i<10;i++){

//        MainWindow::DownloadListModel->setItem(i,0,new QStandardItem( getFileIcon("a.pdf"),tr("file")));
//        MainWindow::DownloadListModel->setItem(i,1,new QStandardItem(tr("1.4M")));
//        MainWindow::DownloadListModel->setItem(i,2,new QStandardItem(tr("2016.5.1")));
//        MainWindow::DownloadListModel->setItem(i,3,new QStandardItem(tr("luojie")));
//    }

}
void Manager::readHostList(){

//    QList<SharedFile> ls= getShareFileList();

    for(int i=0;i<5;i++){

        MainWindow::HostListModel->setItem(i,new QStandardItem( getHostIcon(),tr("computer:%1").arg(i)));

    }

}

QString Manager::fileSizeToString(const qint64 s){
    qint64 size=s;
    QString c="B";
    if(size>1024){
        size=size/1024;
        c="KB";
        if(size>1024){
                size=size/1024;
                c="MB";
        }
    }
    return tr("%1").arg(size)+c;
}

Manager::~Manager()
{
}

