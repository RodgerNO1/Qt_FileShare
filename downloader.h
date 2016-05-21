#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include<QtNetwork>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    ~Downloader();
    void download(QString filesavepath, qint64 filesize, QString ipAddress,qint16 Port=45455);
private:
    QTcpSocket *tcpClient;
    quint16 blockSize;
    QHostAddress hostAddress;
    qint16 tcpPort;

    qint64 TotalBytes;
    qint64 bytesReceived;
    qint64 bytesToReceive;

    qint64 thefileSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

    QTime time;

signals:
    void finshDownload(QString filename);
    void updateProgressBar(int value);
public slots:
    void newConnect();
    void readMessage();
    void closeConnect();
    void displayError(QAbstractSocket::SocketError);

};

#endif // DOWNLOADER_H
