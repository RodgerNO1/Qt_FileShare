#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QModelIndex>
#include"sharedfile.h"
#include "sendfiledialog.h"
#include"settingsdialog.h"
#include"chatform.h"
#include"sendbroadcastform.h"
class Manager;
class FileServer;
class Downloader;
class QUdpSocket;

class QTabWidget;
class QTableView;
class QToolBar;
class QMenuBar;
class QListView;
class QStatusBar;
class QPushButton;
class QStandardItemModel;
//class QModelIndex;//作为参数不可这样这样声明
class QSplitter;
class QProgressBar;
class QMessageBox;
class QLabel;

enum MessageType{Mem_Joinnig,
                 Rep_Joinning,
                 Rep_SharedList,
                 Mem_Leaving,
                 New_Share,
                 File_Transport,
                 Accept_File_Transport,
                 Refuse_File_Transport,
                 Require_File,
                 Rep_Require_File,
                 TextMsg,
                 BroadCastMsg};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initUDPSocket();
    void readSettings();
    QString getIP(QString hostname);
    QString getUserName();
    void sendMessage(MessageType type, QString tohostname="0");  //发送UDP信息
    void sendNewShareMessage(SharedFile sf);
    void sendMessageRequireFile(QString fileName,QString toHostName);
    void sendMessageRepRequireFile(QString fileName, qint64 filesize, QString toHostName);

    void closeEvent(QCloseEvent *event);
//显示界面
    void showMenuBar();
    void showToolBar();
    void showMainView();
    void showStatusBar();
//显示左边view
    void showLeftView();
    void showSharedFileView();
    void showMySharedFileView();
    void showDownloadListView();
    void showDownloadedListView();
//显示右边view
    void showRightView();
    void showHostListview();
    //数据成员
    static QStandardItemModel *SharedFileModel;
    static QStandardItemModel *MySharedFileModel;
    static QStandardItemModel *DownloadListModel;
    static QStandardItemModel *DownloadedListModel;
    static QStandardItemModel *HostListModel;

    static QList<SharedFile> *downloadlist;
    static QMap<QString,MySharedFile> *myShareFileMap;

private:
    qint16 port;

    Manager *manager;
    FileServer *fileServer;
    Downloader *downloader;
    bool isDownloding;
    QUdpSocket *udpSocket;
    QString Def_SavePath;
//debug
    SendFileDialog *sendFileDialog;
    SettingsDialog *settingsDialog;
    ChatForm *chatForm;
    SendBroadcastForm *broadcastForm;

    QToolBar *toolBar;
    QMenuBar *menuBar;
    QTabWidget *tabWidget;
    QTableView *SharedFileTableView;
    QTableView *MySharedFileTableView;
    QTableView *DownloadListView;
    QTableView *DownloadedListView;
    QListView *HostListview;
    QStatusBar *statusBar;
//    QStandardItemModel *ShareFileModel;
    QSplitter *mainSplitter;
    QLabel *label_filename;
    QProgressBar *progressbar;
    int iSharedSelectIndex;
    int iMySharedSelectIndex;
    int iDownloadSelectIndex;
    int iHostSelectIndex;
signals:

public slots:
    void setSharedSelectIndex(QModelIndex SelectIndex);
    void setMySharedSelectIndex(QModelIndex SelectIndex);
    void setDownloadSelectIndex(QModelIndex SelectIndex);
    void setHostSelectIndex(QModelIndex SelectIndex);
    void addDownload();
    void addShareFile();
    void cancelShareFile();
    void cancelDownload();
    void sendFile();
    void sendFileByPath(QString filepath);
    void downloadFileInList(QString latestFinshFileName);//下载传输列表中的文件

    void openSettingDig();
    void showChatForm();
    void sendMsg(QString text, QString toHostName);
    void showBroadForm();
    void toBroadCastMsg(QString text);


    void processPendingDatagrams();//接收UDP数据包
    void updateProgress(int value);
};

#endif // MAINWINDOW_H
