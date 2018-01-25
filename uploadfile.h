#ifndef UPLOADFILE_H
#define UPLOADFILE_H

#include <bits/stdc++.h>
#include <QObject>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <cerrno>

class UploadFile : public QObject
{
    Q_OBJECT
public:
    explicit UploadFile(QObject *parent = nullptr);

signals:

private:
    int initConnnect(std::string host, int port);

public slots:
    bool upFile(QString filename);
    void doAction();
};

#endif // UPLOADFILE_H
