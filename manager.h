#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QIcon>
#include <QList>
#include"sharedfile.h"
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();

    void deleteTempDir();
    QIcon getFileIcon(const QString filename);
    QList<SharedFile> getShareFileList();
    QList<MySharedFile> getMySharedFileList();
    void addMyShareFile(MySharedFile mySharedFile);
    void cancelMyShareFile(QString fn);
//    QList<SharedFile> getDownloadFileList();

    QIcon getHostIcon();
    QIcon getDownIcon();
    QIcon getCancelIcon();
    QIcon getAddIcon();
    QIcon getSendIcon();
    QIcon getChatIcon();
    QString fileSizeToString(qint64 s);

    void readSharedFileList();
    void readMySharedFileList();
    void readDownloadList();
    void readHostList();


signals:

public slots:
};

#endif // MANAGER_H
