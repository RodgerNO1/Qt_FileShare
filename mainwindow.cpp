#include "mainwindow.h"
#include"manager.h"
#include"downloader.h"
#include"fileserver.h"

#include <QStandardItemModel>
#include <QTableWidget>
#include <QTableView>
#include <QMenuBar>
#include <QStatusBar>
#include <QPushButton>
#include<QHeaderView>
#include<QMimeData>
#include <QMenu>
#include<QAction>
#include<QDebug>
#include<QString>
#include<QModelIndexList>
#include<QLabel>
#include<QVBoxLayout>
#include<QProgressBar>
#include<QListView>
#include<QListWidget>
#include<QSplitter>
#include<QModelIndex>
#include<QMessageBox>
#include<QFileDialog>
#include<QToolBar>

QStandardItemModel *MainWindow::SharedFileModel=new QStandardItemModel();
QStandardItemModel *MainWindow::MySharedFileModel=new QStandardItemModel();
QStandardItemModel *MainWindow::DownloadListModel=new QStandardItemModel();
QStandardItemModel *MainWindow::DownloadedListModel=new QStandardItemModel();
QStandardItemModel *MainWindow::HostListModel=new QStandardItemModel();

QList<SharedFile> *MainWindow::downloadlist=new QList<SharedFile>;
QMap<QString,MySharedFile> *MainWindow::myShareFileMap=new QMap<QString,MySharedFile>;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    iSharedSelectIndex(0),
    iMySharedSelectIndex(0),
    iDownloadSelectIndex(0),
    iHostSelectIndex(0),
    isDownloding(false)
{
    manager=new Manager;
    fileServer=new FileServer;
    fileServer->startFileServer(45455);

    chatForm=NULL;//初始化为空

    downloader=new Downloader;
    connect(downloader,SIGNAL(updateProgressBar(int)),this,SLOT(updateProgress(int)));
    connect(downloader,SIGNAL(finshDownload(QString)),this,SLOT(downloadFileInList(QString)));

    readSettings();//初始化配置
//    manager->readHostList();
    manager->readMySharedFileList();

    initUDPSocket();

    setGeometry(200,100,800,600);
    showMenuBar();
    showMainView();
    showStatusBar();



//广播自己的主机名
    sendMessage(Mem_Joinnig);
}
void MainWindow::showMenuBar(){
    menuBar=new QMenuBar;
    QList<QAction*> actList;
    QAction *actSetting= new QAction(tr("设置"),this);
    QAction *actBroadCastMsg= new QAction(tr("广播信息"),this);
    actList.append(actSetting);
    actList.append(actBroadCastMsg);
    connect(actSetting,SIGNAL(triggered()),this,SLOT(openSettingDig()));
    connect(actBroadCastMsg,SIGNAL(triggered()),this,SLOT(showBroadForm()));
//    menuBar->addMenu(tr("共享文件库"));
//    menuBar->addMenu(tr("我的共享"));
//    menuBar->addMenu(tr("下载列表"));
//    menuBar->addMenu(tr("关于"));
    menuBar->addActions(actList);

    setMenuBar(menuBar);
}
void MainWindow::showBroadForm(){
    broadcastForm=new SendBroadcastForm;
    broadcastForm->show();
    connect(broadcastForm,SIGNAL(broadcastMsg(QString)),this,SLOT(toBroadCastMsg(QString)));
}
void MainWindow::toBroadCastMsg(QString text){
    sendMessage(BroadCastMsg,text);
}

void MainWindow::openSettingDig(){
    settingsDialog=new SettingsDialog(Def_SavePath);
    settingsDialog->show();
}

void MainWindow::showToolBar(){


}
void MainWindow::showMainView(){
    mainSplitter=new QSplitter(Qt::Horizontal,this);

    tabWidget=new QTabWidget(mainSplitter);
    tabWidget->setMinimumWidth(500);

    //setView
    showLeftView();
    showRightView();

    setCentralWidget(mainSplitter);
}
void MainWindow::showLeftView(){
    showSharedFileView();
    showMySharedFileView();
    showDownloadListView();
    showDownloadedListView();
}
void MainWindow::showSharedFileView(){

    MainWindow::SharedFileModel->setHorizontalHeaderItem(0, new QStandardItem(tr("名称")));
    MainWindow::SharedFileModel->setHorizontalHeaderItem(1,  new QStandardItem(tr("大小")));
    MainWindow::SharedFileModel->setHorizontalHeaderItem(2,  new QStandardItem(tr("日期")));
    MainWindow::SharedFileModel->setHorizontalHeaderItem(3,  new QStandardItem(tr("分享者")));

    SharedFileTableView=new QTableView;
    SharedFileTableView->resizeRowsToContents();
    SharedFileTableView->setModel(MainWindow::SharedFileModel);
    SharedFileTableView->setSortingEnabled(true);
    SharedFileTableView->setSelectionBehavior ( QAbstractItemView::SelectRows);//按行选择
    SharedFileTableView->setSelectionMode ( QAbstractItemView::SingleSelection); //单行选择
    SharedFileTableView->setShowGrid(false);//隐藏表格线
    SharedFileTableView->setWordWrap(true);
    SharedFileTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更
    SharedFileTableView->setAlternatingRowColors(true);   //设置隔一行变一颜色
//    SharedFileTableView->resizeColumnToContents (0);

    SharedFileTableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actDownload = new QAction(QIcon(manager->getDownIcon()),tr("下载"),this);
    SharedFileTableView->addAction(actDownload);

    connect(SharedFileTableView,SIGNAL(pressed(QModelIndex)),this,SLOT(setSharedSelectIndex(QModelIndex)));
    connect(actDownload, SIGNAL(triggered()), this, SLOT(addDownload()));

    tabWidget->addTab(SharedFileTableView,tr("共享文件库"));
}
void MainWindow::showMySharedFileView(){
    MainWindow::MySharedFileModel->setHorizontalHeaderItem(0, new QStandardItem(tr("名称")));
    MainWindow::MySharedFileModel->setHorizontalHeaderItem(1,  new QStandardItem(tr("大小")));
    MainWindow::MySharedFileModel->setHorizontalHeaderItem(2,  new QStandardItem(tr("日期")));
    MainWindow::MySharedFileModel->setHorizontalHeaderItem(3,  new QStandardItem(tr("分享者")));

    MySharedFileTableView=new QTableView;
    MySharedFileTableView->resizeRowsToContents();
    MySharedFileTableView->setModel(MainWindow::MySharedFileModel);
    MySharedFileTableView->setSortingEnabled(true);
    MySharedFileTableView->setSelectionBehavior ( QAbstractItemView::SelectRows);//按行选择
    MySharedFileTableView->setSelectionMode ( QAbstractItemView::SingleSelection); //单行选择
    MySharedFileTableView->setShowGrid(false);//隐藏表格线
    MySharedFileTableView->setWordWrap(true);
    MySharedFileTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更
    MySharedFileTableView->setAlternatingRowColors(true);   //设置隔一行变一颜色
//    MySharedFileTableView->resizeColumnToContents (0);

    MySharedFileTableView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actCancelShare = new QAction(manager->getCancelIcon(),tr("取消分享"),this);
    QAction *actAddShare = new QAction(manager->getAddIcon(),tr("添加分享"),this);
    MySharedFileTableView->addAction(actAddShare);
    MySharedFileTableView->addAction(actCancelShare);


    connect(MySharedFileTableView,SIGNAL(pressed(QModelIndex)),this,SLOT(setMySharedSelectIndex(QModelIndex)));
    connect(actAddShare, SIGNAL(triggered()), this, SLOT(addShareFile()));
    connect(actCancelShare, SIGNAL(triggered()), this, SLOT(cancelShareFile()));
    tabWidget->addTab(MySharedFileTableView,tr("我的共享"));

}
void MainWindow::showDownloadListView(){
    MainWindow::DownloadListModel->setHorizontalHeaderItem(0, new QStandardItem(tr("名称")));
    MainWindow::DownloadListModel->setHorizontalHeaderItem(1,  new QStandardItem(tr("大小")));
    MainWindow::DownloadListModel->setHorizontalHeaderItem(2,  new QStandardItem(tr("日期")));
    MainWindow::DownloadListModel->setHorizontalHeaderItem(3,  new QStandardItem(tr("分享者")));

    DownloadListView=new QTableView;
    DownloadListView->resizeRowsToContents();
    DownloadListView->setModel(MainWindow::DownloadListModel);
    DownloadListView->setSortingEnabled(false);//不可排序
    DownloadListView->setSelectionBehavior ( QAbstractItemView::SelectRows);//按行选择
    DownloadListView->setSelectionMode ( QAbstractItemView::SingleSelection); //单行选择
    DownloadListView->setShowGrid(false);//隐藏表格线
    DownloadListView->setWordWrap(true);
    DownloadListView->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更
    DownloadListView->setAlternatingRowColors(true);   //设置隔一行变一颜色
//    DownloadListView->resizeColumnToContents (0);

    DownloadListView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actCancelDownload = new QAction(manager->getCancelIcon(),tr("取消下载"),this);
    DownloadListView->addAction(actCancelDownload);

    connect(DownloadListView,SIGNAL(pressed(QModelIndex)),this,SLOT(setDownloadSelectIndex(QModelIndex)));
    connect(actCancelDownload, SIGNAL(triggered()), this, SLOT(cancelDownload()));
    tabWidget->addTab(DownloadListView,tr("传输列表"));

}
void MainWindow::showDownloadedListView(){
    MainWindow::DownloadedListModel->setHorizontalHeaderItem(0, new QStandardItem(tr("名称")));
    MainWindow::DownloadedListModel->setHorizontalHeaderItem(1,  new QStandardItem(tr("大小")));
    MainWindow::DownloadedListModel->setHorizontalHeaderItem(2,  new QStandardItem(tr("分享者")));
    MainWindow::DownloadedListModel->setHorizontalHeaderItem(3,  new QStandardItem(tr("完成时间")));

    DownloadedListView=new QTableView;
    DownloadedListView->resizeRowsToContents();
    DownloadedListView->setModel(MainWindow::DownloadedListModel);
    DownloadedListView->setSortingEnabled(false);//不可排序
    DownloadedListView->setSelectionBehavior ( QAbstractItemView::SelectRows);//按行选择
    DownloadedListView->setSelectionMode ( QAbstractItemView::SingleSelection); //单行选择
    DownloadedListView->setShowGrid(false);//隐藏表格线
    DownloadedListView->setWordWrap(true);
    DownloadedListView->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更
    DownloadedListView->setAlternatingRowColors(true);   //设置隔一行变一颜色
//    DownloadedListView->resizeColumnToContents (0);

//    DownloadedListView->setContextMenuPolicy(Qt::ActionsContextMenu);
//    QAction *actCancelDownload = new QAction(manager->getCancelIcon(),tr("取消下载"),this);
//    DownloadedListView->addAction(actCancelDownload);

//    connect(DownloadedListView,SIGNAL(pressed(QModelIndex)),this,SLOT(setDownloadSelectIndex(QModelIndex)));
//    connect(actCancelDownload, SIGNAL(triggered()), this, SLOT(cancelDownload()));
    tabWidget->addTab(DownloadedListView,tr("完成列表"));
}

void MainWindow::showRightView(){
    QWidget *rightWiget=new QWidget(mainSplitter);
    QVBoxLayout  *rightLayout=new QVBoxLayout();
    rightWiget->setLayout(rightLayout);


    QLabel *lable=new QLabel(tr("<b>在线主机</b>"));
    rightLayout->addWidget(lable);

    showHostListview();
    rightLayout->addWidget(HostListview);


}
void MainWindow::showHostListview(){

    HostListview=new QListView;
    HostListview->setModel(HostListModel);
    HostListview->setIconSize(QSize(40,40));

    HostListview->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actDownload = new QAction(manager->getSendIcon(),tr("发送文件"),this);
    HostListview->addAction(actDownload);
    QAction *actSendMsg = new QAction(manager->getChatIcon,tr("发送消息"),this);
    HostListview->addAction(actSendMsg);

    connect(HostListview,SIGNAL(pressed(QModelIndex)),this,SLOT(setHostSelectIndex(QModelIndex)));
    connect(actDownload, SIGNAL(triggered()), this, SLOT(sendFile()));
    connect(actSendMsg, SIGNAL(triggered()), this, SLOT(showChatForm()));
}
void MainWindow::sendFile(){
    sendFileDialog=new SendFileDialog(this);
    sendFileDialog->setWindowTitle(tr("发送文件给%1").arg(HostListModel->item(iHostSelectIndex,0)->text()));
    sendFileDialog->show();

    connect(sendFileDialog,SIGNAL(sendFileByPath(QString)),this,SLOT(sendFileByPath(QString)));
    qDebug()<<"sendFile:index"<<iHostSelectIndex;

}
void MainWindow::sendFileByPath(QString filepath)
{
    qDebug()<<"sendFileByPath:"<<filepath;
    sendMessage(File_Transport,HostListModel->item(iHostSelectIndex,0)->text());
}

void MainWindow::showChatForm(){
    chatForm=new ChatForm(HostListModel->item(iHostSelectIndex,0)->text());
    chatForm->show();
    connect(chatForm,SIGNAL(sendMsg(QString,QString)),this,SLOT(sendMsg(QString,QString)));
}
void MainWindow::sendMsg(QString text,QString toHostName){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString hostname=QHostInfo::localHostName();
    QString username=getUserName();

    out<<TextMsg<<hostname<<username<<text;
    udpSocket->writeDatagram(data.data(), data.size(),
                             QHostAddress(getIP(toHostName)), 45454);
    qDebug()<<"---sendMsg--text:"<<text;
}

void MainWindow::showStatusBar(){
    statusBar=new QStatusBar;
    statusBar->showMessage(tr("ready"));

    progressbar=new QProgressBar;
    label_filename=new QLabel(tr("没有下载"));
    progressbar->setValue(0);
    statusBar->addPermanentWidget(label_filename);
    statusBar->addPermanentWidget(progressbar);//添加进度条，addWidget()不行
    setStatusBar(statusBar);

}
void MainWindow::readSettings(){
    Def_SavePath=QDir::currentPath()+"/Download";
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    if(!settings.contains("File")){//配置文件不存在
        settings.setValue("File", 1);
        settings.setValue("Def_SavePath",Def_SavePath);
        qDebug()<<"----readSettings---配置文件不存在";
    }else{
        Def_SavePath=settings.value("Def_SavePath").toString();
        qDebug()<<"----readSettings---配置文件已存在";
    }
    settings.endGroup();

}

void MainWindow::initUDPSocket(){
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(45454, QUdpSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));
}

void MainWindow::addDownload(){
    QString filename=SharedFileModel->item(iSharedSelectIndex,0)->text();
    QString filesize=SharedFileModel->item(iSharedSelectIndex,1)->text();
    QString filedate=SharedFileModel->item(iSharedSelectIndex,2)->text();
    QString filesharer=SharedFileModel->item(iSharedSelectIndex,3)->text();

    int btn = QMessageBox::information(this,tr("下载"),
                             tr("确定下载%1分享的%2？")
                             .arg(filesharer,filename),
                             QMessageBox::Yes,QMessageBox::No);
    if(btn == QMessageBox::Yes){
        downloadlist->append(SharedFile(filename,filesize,filedate,filesharer));
        QList<QStandardItem*> lss;
        //注意这个问题
        lss.append(SharedFileModel->item(iSharedSelectIndex,0)->clone());
        lss.append(SharedFileModel->item(iSharedSelectIndex,1)->clone());
        lss.append(SharedFileModel->item(iSharedSelectIndex,2)->clone());
        lss.append(SharedFileModel->item(iSharedSelectIndex,3)->clone());

        DownloadListModel->appendRow(lss);
        downloadFileInList("no_latest");//开始下载
    }

    qDebug()<<"download:index"<<iSharedSelectIndex;

}
void MainWindow::cancelDownload(){
    QString filename=SharedFileModel->item(iDownloadSelectIndex,0)->text();
    QString filesize=SharedFileModel->item(iDownloadSelectIndex,1)->text();
    QString filedate=SharedFileModel->item(iDownloadSelectIndex,2)->text();
    QString filesharer=SharedFileModel->item(iDownloadSelectIndex,3)->text();

    int btn = QMessageBox::information(this,tr("取消下载"),
                             tr("确定取消下载%1？")
                             .arg(filename),
                             QMessageBox::Yes,QMessageBox::No);
    if(btn == QMessageBox::Yes){

        downloadlist->removeOne(SharedFile(filename,filesize,filedate,filesharer));
        DownloadListModel->removeRow(iDownloadSelectIndex);
    }

    qDebug()<<"cancelDownload:index"<<iDownloadSelectIndex;

}
void MainWindow::addShareFile(){
    QString filePath = QFileDialog::getOpenFileName(this);
    if(!filePath.isEmpty())
    {
        QFileInfo fileinfo(filePath);
        QString filename(fileinfo.fileName());
        QString filesize=manager->fileSizeToString(fileinfo.size());
        QString filedate(fileinfo.lastModified().toString("yyyy.MM.dd hh:mm:ss"));
        QString filesharer(QHostInfo::localHostName());

        QList<QStandardItem*> ls;
        ls.append(new QStandardItem(manager->getFileIcon(filename),filename));
        ls.append(new QStandardItem(filesize));
        ls.append(new QStandardItem(filedate));
        ls.append(new QStandardItem(filesharer));

        manager->addMyShareFile(MySharedFile(filename,filesize,filedate,filesharer,filePath));
        MySharedFileModel->appendRow(ls);
        //广播我的新分享文件
        sendNewShareMessage(SharedFile(filename,filesize,filedate,filesharer));
    }


}
void MainWindow::cancelShareFile()
{
    QString fn=MySharedFileModel->item(iMySharedSelectIndex,0)->text();

    int btn = QMessageBox::information(this,tr("取消分享"),
                             tr("取消分享%1？")
                             .arg(fn),
                             QMessageBox::Yes,QMessageBox::No);
    if(btn==QMessageBox::Yes){
        manager->cancelMyShareFile(fn);
        MySharedFileModel->removeRow(iMySharedSelectIndex);
    }

    qDebug()<<"cancelShareFile:index"<<iMySharedSelectIndex;

}

//--------------下载传输列表中的文件--------------------
void MainWindow::downloadFileInList(QString latestFinshFileName){
    qDebug()<<"----downloadFileInList----latestFinshFileName:"<<latestFinshFileName;
    QString fileName,toHostName;
    if(latestFinshFileName=="no_latest"&&!isDownloding){//添加新下载时，没有下载任务，则立即下载
         qDebug()<<"----downloadFileInList----添加新下载:";
         statusBar->showMessage("添加了新下载");

         isDownloding=true;//设置已开始下载为true
         fileName=DownloadListModel->item(0,0)->text();
         toHostName=DownloadListModel->item(0,3)->text();
         sendMessageRequireFile(fileName,toHostName);
         qDebug()<<"----downloadFileInList----开始下载:"<<fileName;
    }
    if(latestFinshFileName!="no_latest"){ //已下载一个文件
        DownloadListModel->removeRow(0);//将已下载文件从待下载列表中移除
        //添加到已下载列表
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("MM-dd hh:mm:ss");
        QList<QStandardItem*> lss;
        //注意这个问题
        lss.append(SharedFileModel->item(iSharedSelectIndex,0)->clone());
        lss.append(SharedFileModel->item(iSharedSelectIndex,1)->clone());
        lss.append(SharedFileModel->item(iSharedSelectIndex,3)->clone());
        lss.append(new QStandardItem(current_date));

        DownloadedListModel->appendRow(lss);
        progressbar->setValue(0);
        label_filename->setText("没有下载");
        qDebug()<<"----downloadFileInList----完成下载:"<<latestFinshFileName;

        if(DownloadListModel->rowCount()>0){//有待下载文件，则下载


            isDownloding=true;//设置已开始下载为true
            fileName=DownloadListModel->item(0,0)->text();
            toHostName=DownloadListModel->item(0,3)->text();
            sendMessageRequireFile(fileName,toHostName);
            qDebug()<<"----downloadFileInList----开始下载:"<<fileName;

         }else{
            isDownloding=false;//下载列表为空，设置已开始下载为false
            qDebug()<<"----downloadFileInList----下载列表为空:";
        }
    }
}
void MainWindow::sendMessageRequireFile(QString fileName,QString toHostName){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString hostname=QHostInfo::localHostName();
    QString username=getUserName();

    out<<Require_File<<hostname<<username<<fileName;
    udpSocket->writeDatagram(data.data(), data.size(),
                             QHostAddress(getIP(toHostName)), 45454);
    qDebug()<<"---sendMessageRequireFile--fileName"<<fileName<<" toHostName:"<<toHostName;
}
void MainWindow::sendMessageRepRequireFile(QString fileName,qint64 filesize,QString toHostName){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString hostname=QHostInfo::localHostName();
    QString username=getUserName();

    out<<Rep_Require_File<<hostname<<username<<fileName<<filesize;
    udpSocket->writeDatagram(data.data(), data.size(),
                             QHostAddress(getIP(toHostName)), 45454);
    qDebug()<<"---sendMessageRepRequireFile--fileName"<<fileName<<" toHostName:"<<toHostName;
}
void MainWindow::sendNewShareMessage(SharedFile sf){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString hostname=QHostInfo::localHostName();
    QString username=getUserName();

    out<<New_Share<<hostname<<username<<sf.sFileName<<sf.sFileSize<<sf.sFileDate<<sf.sFileSharer;
    udpSocket->writeDatagram(data.data(), data.size(),
                             QHostAddress::Broadcast, 45454);
    qDebug()<<"---send new_sharefile--datasize"<<data.size()<<"filename:"<<sf.sFileName
           <<" filesize"<<sf.sFileSize
          <<" filedate"<<sf.sFileDate
         <<" filesharer"<<sf.sFileSharer;

}

void MainWindow::sendMessage(MessageType type, QString tohostname){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString hostname=QHostInfo::localHostName();
    QString username=getUserName();
    //根据类型加载不同UDP信息
    switch (type) {
        case Mem_Joinnig:
        {
            out<<type<<hostname<<username;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress::Broadcast, 45454);
            qDebug()<<tr("---Im_Joinnig---");
            break;
        }
        case Rep_Joinning:
        {
            //回复新加入者以我的信息
            out<<type<<hostname<<username;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress(getIP(tohostname)), 45454);
            qDebug()<<tr("---Rep_Joinning---");
            //回复新加入者以我的分享文件列表
            QMap<QString, MySharedFile>::iterator it;

            for (it= MainWindow::myShareFileMap->begin(); it != MainWindow::myShareFileMap->end(); ++it){
                sendNewShareMessage(SharedFile(it.value().sFileName,
                                               it.value().sFileSize,
                                               it.value().sFileDate,
                                               it.value().sFileSharer));
            }

            break;
        }
        case Mem_Leaving:
        {
            out<<type<<hostname<<username;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress::Broadcast, 45454);
            qDebug()<<tr("---Im_Leaving---");
            break;
        }
        case File_Transport:
        {
            out<<type<<hostname<<username<<sendFileDialog->theFileName<<sendFileDialog->theFileSize;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress(getIP(tohostname)), 45454);
            qDebug()<<tr("---Accept_File_Transport---");
            break;
        }
        case Accept_File_Transport:
        {
            out<<type<<hostname<<username;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress(getIP(tohostname)), 45454);
            qDebug()<<tr("---Accept_File_Transport---");
            break;
        }
        case Refuse_File_Transport:
        {
            out<<type<<hostname<<username;
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress(getIP(tohostname)), 45454);
            qDebug()<<tr("---Refuse_File_Transport---");
            break;
        }
        case BroadCastMsg:
        {
            out<<type<<hostname<<username<<tohostname;//tohostname为要广播的消息
            udpSocket->writeDatagram(data.data(), data.size(),
                                     QHostAddress::Broadcast, 45454);
            qDebug()<<"---BroadCastMsg---text:"<<tohostname;
            break;
        }


    }



}

void MainWindow::processPendingDatagrams()//接收UDP数据包的槽
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size());
        QDataStream in(&data,QIODevice::ReadOnly);
        int type=-1;
        QString fromhostname="unkown";
        QString fromusername="unkown";
        in>>type>>fromhostname>>fromusername;
        //根据类型来处理数据
        switch (type){
            case Mem_Joinnig:
            {

                if(QHostInfo::localHostName()!=fromhostname){//新加入的不是自己
                    //更新在线主机列表
                    HostListModel->appendRow(new QStandardItem(manager->getHostIcon(),fromhostname));
                }
                sendMessage(Rep_Joinning,fromhostname);
                qDebug()<<"---Mem_Joinnig--upDate:HostListModel";

                break;
            }
            case Rep_Joinning:
            {
                HostListModel->appendRow(new QStandardItem(manager->getHostIcon(),fromhostname));
                qDebug()<<"---Rep_Joinning--to:"<<fromhostname;
                break;
            }
            case Mem_Leaving:
            {
                    int rowNum = HostListModel->findItems(fromhostname,Qt::MatchExactly).first()->row();
                    HostListModel->removeRow(rowNum);
                    qDebug()<<"---Mem_Leaving--host:"<<fromhostname;
                break;
            }
            case New_Share:
            {
                //更新共享库列表
                QString filename,filesize,filedate,filesharer;
                in>>filename>>filesize>>filedate>>filesharer;

                QList<QStandardItem*> ls;
                ls.append(new QStandardItem(manager->getFileIcon(filename),filename));
                ls.append(new QStandardItem(filesize));
                ls.append(new QStandardItem(filedate));
                ls.append(new QStandardItem(filesharer));
                qDebug()<<"---recive new_sharefile--"<<data.size()<<" filename:"<<filename
                       <<" filesize"<<filesize
                      <<" filedate"<<filedate
                     <<" filesharer"<<filesharer;

                MainWindow::SharedFileModel->appendRow(ls);

                break;
            }
            case File_Transport:
            {

                QString rvfilename;
                qint64 filesize;
                in>>rvfilename>>filesize;

                int btn = QMessageBox::information(this,tr("接收文件"),
                                         tr("来自%1的文件：%2(%3)是否接收？")
                                         .arg(fromhostname).arg(rvfilename).arg(manager->fileSizeToString(filesize)),
                                         QMessageBox::Yes,QMessageBox::No);

                if(btn == QMessageBox::Yes)
                {
                    sendMessage(Accept_File_Transport,fromhostname);

                    QString filesavepath = QFileDialog::getSaveFileName(0,tr("保存文件"),rvfilename);
                    if(!filesavepath.isEmpty())
                    {
                       downloader->download(filesavepath,filesize,getIP(fromhostname),45456);
                       QString filename=filesavepath.right(filesavepath.size() - filesavepath.lastIndexOf('/')-1);//从路径中获取文件名
                       progressbar->setValue(0);
                       label_filename->setText(filename);
                    }

                }
                else{
                    sendMessage(Refuse_File_Transport,fromhostname);
                }
                break;
            }
            case Accept_File_Transport:
            {
                sendFileDialog->setLabelText(tr("开始发送！"));
                statusBar->showMessage(tr("开始发送"));
                break;
            }
            case Refuse_File_Transport:
            {
                sendFileDialog->setLabelText(tr("对方已拒绝接受！"));
                break;
            }
            case Require_File:
            {
                QString fn,fp;
                in>>fn;
                MySharedFile mysf=myShareFileMap->value(fn);
                fp=mysf.sFilePath;
                QFileInfo fi(fp);
                fileServer->setFile(fp);
                sendMessageRepRequireFile(fn,fi.size(),fromhostname);
                break;
            }
            case Rep_Require_File:
            {
                QString fn;
                qint64 filesize;
                in>>fn>>filesize;

                downloader->download(Def_SavePath+"/"+fn,filesize,getIP(fromhostname),45455);
                progressbar->setValue(0);
                statusBar->showMessage(tr("正在下载"));
                label_filename->setText(fn);

                break;
            }
            case TextMsg:
            {
                QString text;
                in>>text;
                if(chatForm==NULL){
                    chatForm=new ChatForm(fromhostname);
                    chatForm->show();
                    connect(chatForm,SIGNAL(sendMsg(QString,QString)),this,SLOT(sendMsg(QString,QString)));
                }
                chatForm->readMsg(fromhostname,text);

                break;
            }
            case BroadCastMsg:
            {
                QString text;
                in>>text;//tohostname为要广播的消息
                QMessageBox::information(this,tr("收到%1的广播消息").arg(fromhostname),
                                         text,
                                         QMessageBox::Yes);
                qDebug()<<"---reciver BroadCastMsg---text:"<<text;
                break;
            }

        }

        qDebug()<<tr("---Received datagram from %1----").arg(fromhostname);
    }
}

void MainWindow::updateProgress(int value){
    progressbar->setValue(value);

}

void MainWindow::setSharedSelectIndex(QModelIndex SharedFileModelIndex){
    iSharedSelectIndex=SharedFileModelIndex.row();

    qDebug()<<"iSharedSelectIndex:"<<iSharedSelectIndex;
}
void MainWindow::setMySharedSelectIndex(QModelIndex SharedFileModelIndex){
    iMySharedSelectIndex=SharedFileModelIndex.row();

    qDebug()<<"iMySharedSelectIndex:"<<iMySharedSelectIndex;
}
void MainWindow::setDownloadSelectIndex(QModelIndex SharedFileModelIndex){
    iDownloadSelectIndex=SharedFileModelIndex.row();

    qDebug()<<"iDownloadSelectIndex:"<<iDownloadSelectIndex;
}
void MainWindow::setHostSelectIndex(QModelIndex SharedFileModelIndex){
    iHostSelectIndex=SharedFileModelIndex.row();

    qDebug()<<"iHostSelectIndex:"<<iHostSelectIndex;
}

QString MainWindow::getUserName()  //获取用户名
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1)
        {

            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return false;
}

QString MainWindow::getIP(QString hostname)  //获取ip地址
{

    QHostInfo info=QHostInfo::fromName(hostname);
    if (info.error() != QHostInfo::NoError)
        {
             qDebug() << "Lookup failed:" << info.errorString();
             return "0.0.0.0";
        }

    for (int i = 0;i < info.addresses().size();i++)
        {
            if(info.addresses()[i].protocol()==QAbstractSocket::IPv4Protocol){
             return info.addresses()[i].toString();
            }
        }
       return "0.0.0.0";
}

void MainWindow::closeEvent(QCloseEvent *event){
    qDebug()<<"closeEvent:";
    sendMessage(Mem_Leaving);
}
MainWindow::~MainWindow()
{
    manager->deleteTempDir();//删除临时文件

    delete manager;
    delete fileServer;
    delete downloader;
    delete udpSocket;
}
