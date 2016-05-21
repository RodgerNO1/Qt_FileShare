#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H
#include <QDialog>

namespace Ui{
    class SendFileDialog;
}

class FileServer;

class SendFileDialog : public QDialog
{
    Q_OBJECT
public:
    SendFileDialog(QWidget *parent = 0);
    ~SendFileDialog();
    void setLabelText(QString text);

    FileServer *fileServer;
    QString theFilePath;
    QString theFileName;
    qint64 theFileSize;

private:
    Ui::SendFileDialog *ui;

private slots:
    void on_Open();
    void on_Send();
    void on_Close();
    void updateProgress(int value);
signals:
    void sendFileByPath(QString);

};

#endif // SENDFILEDIALOG_H
