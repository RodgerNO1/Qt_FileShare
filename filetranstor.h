#ifndef FILETRANSTOR_H
#define FILETRANSTOR_H

#include <QObject>

class FileTranstor : public QObject
{
    Q_OBJECT
public:
    explicit FileTranstor(QObject *parent = 0);

signals:

public slots:
};

#endif // FILETRANSTOR_H