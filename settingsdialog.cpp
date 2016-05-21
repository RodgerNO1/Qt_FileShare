#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include<QFileInfo>
#include<QFileIconProvider>
#include<QSettings>
SettingsDialog::SettingsDialog(QString def_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("设置");
    setFixedSize(this->geometry().width(),this->geometry().height());//设置固定大小
    connect(ui->selectBtn,SIGNAL(clicked()),this,SLOT(selectPath()));
    connect(ui->okBtn,SIGNAL(clicked()),this,SLOT(changePath()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(cancelChange()));

    ui->lineEdit->setText(def_path);
    this->defpath=def_path;
    QFileIconProvider iconp;
    ui->selectBtn->setIcon(iconp.icon(QFileIconProvider::Folder));
}
void SettingsDialog::selectPath(){
    QString p=QFileDialog::getExistingDirectory(this,tr("选择文件夹"),defpath);
    if(!p.isEmpty()){
        ui->lineEdit->setText(p);
    }

}
void SettingsDialog::changePath(){
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("Def_SavePath",ui->lineEdit->text());
    settings.endGroup();
    close();//关闭对话框
}
void SettingsDialog::cancelChange(){
    ui->lineEdit->setText(defpath);
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Settings");
    settings.setValue("Def_SavePath",defpath);
    settings.endGroup();
}
SettingsDialog::~SettingsDialog()
{
    delete ui;
}
