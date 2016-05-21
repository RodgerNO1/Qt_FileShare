#ifndef FILESERVER_H
#define FILESERVER_H

#include <QObject>
#include<QtNetwork>
class FileServer : public QObject
{
    Q_OBJECT
public:
    explicit FileServer(QObject *parent = 0);
    ~FileServer();
    void startFileServer(qint16 linstenport=45455);
    void setFile(QString filepath);
private:

    qint16 tcpPort;
    QTcpServer *tcpServer;
    QString fileName;
    QString theFileName;
    QFile *localFile;

    qint64 totalBytes;//总共数据大小
    qint64 bytesWritten;//已发送数据大小
    qint64 bytesToWrite;//还剩数据大小
    qint64 loadSize;//缓冲区大小
    QByteArray outBlock;//缓存一次发送的数据

    QTcpSocket *clientConnection;

    QTime time;//计时器

signals:
//    void finshTransfer(QString filename);
    void updateProgressBar(int value);
public slots:
    void sendFile();
    void updateClientProgress(qint64 numBytes);
};

#endif // FILESERVER_H
