#ifndef SHAREDFILE_H
#define SHAREDFILE_H
#include<QString>


class SharedFile
{
public:
    QString sFileName;
    QString sFileSize;
    QString sFileDate;
    QString sFileSharer;
    SharedFile()
    {
        sFileName="filename";
        sFileSize="filesize";
        sFileDate="filedata";
        sFileSharer="fileshare";
    }
    SharedFile(QString filename, QString filesize, QString filedata, QString fileshare)
    {
        sFileName=filename;
        sFileSize=filesize;
        sFileDate=filedata;
        sFileSharer=fileshare;
    }
    bool operator ==(const SharedFile sf){
        if(sf.sFileName==sFileName&&
           sf.sFileSize==sFileSize&&
           sf.sFileDate==sFileDate&&
           sf.sFileSharer==sFileSharer)
            return true;
        else return false;
    }

};
class MySharedFile : public SharedFile
{
public:
    QString sFilePath;
    MySharedFile(){}

    MySharedFile(QString filename, QString filesize, QString filedate, QString fileshare,QString filepath):
        SharedFile(filename,filesize,filedate,fileshare)//父类初始化
    {

       sFilePath=filepath;
    }
    bool operator ==(const MySharedFile mysf){
        if(mysf.sFileName==sFileName&&
           mysf.sFileSize==sFileSize&&
           mysf.sFileDate==sFileDate&&
           mysf.sFileSharer==sFileSharer&&
           mysf.sFilePath==sFilePath)
            return true;
        else return false;
    }
};
#endif // SHAREDFILE_H
