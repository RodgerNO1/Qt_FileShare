#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsDialog(QString defpath, QWidget *parent = 0);
    ~SettingsDialog();

    QString defpath;

private:
    Ui::SettingsDialog *ui;

public slots:
    void selectPath();
    void changePath();
    void cancelChange();
};

#endif // SETTINGSDIALOG_H
