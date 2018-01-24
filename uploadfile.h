#ifndef UPLOADFILE_H
#define UPLOADFILE_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QFile>
#include <iostream>


class uploadFile : public QObject
{

public:
    uploadFile();
    void doAction();
};

#endif // UPLOADFILE_H
