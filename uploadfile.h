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
#include <fstream>
#include <sys/stat.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

#define FAIL    -1


#include "md5.h"

class UploadFile : public QObject
{
    Q_OBJECT
public:
    explicit UploadFile(QObject *parent = nullptr);

signals:

private:
    int initConnnect(std::string host, int port);
    bool fsend(std::string filepath);
    bool Csend(std::string filepath);

    SSL_CTX *ctx;
    SSL *ssl;
    int bytes;
    char buffer[4096];

public slots:
    bool upFile(QString filename);
    void doAction();
};

#endif // UPLOADFILE_H
