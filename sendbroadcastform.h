#ifndef SENDBROADCASTFORM_H
#define SENDBROADCASTFORM_H

#include <QWidget>

namespace Ui {
class SendBroadcastForm;
}

class SendBroadcastForm : public QWidget
{
    Q_OBJECT

public:
    explicit SendBroadcastForm(QWidget *parent = 0);
    ~SendBroadcastForm();

private:
    Ui::SendBroadcastForm *ui;
signals:
    void broadcastMsg(QString);
public slots:
    void onSendBtn();
    void onCancelBtn();
};

#endif // SENDBROADCASTFORM_H
