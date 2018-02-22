#ifndef MANAGECONNECTION_H
#define MANAGECONNECTION_H

#include <QObject>
#include "filesys.h"
#include "connection.h"

class ManageConnection : public QObject
{
    Q_OBJECT
public:
    explicit ManageConnection(QObject *parent = nullptr);
    ~ManageConnection();

    SSL_CTX * getCTX();


private:
    Connection * mainConnection;
    std::vector<Connection*> listConnnection;
    SSL_CTX *ctx;

    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);


signals:

public slots:
    bool connectToServer(QString host, int port);
    bool authenConnection(QString username, QString password);
};

#endif // MANAGECONNECTION_H
