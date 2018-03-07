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

    SSL_CTX*                    getCTX();
    QString                     get_Hostname();
    int                         get_Port();
    void                        set_Hostname(QString);
    void                        set_Port(int);

private:

    Connection*                 mainConnection;
    std::vector<Connection*>    listConnnection;
    SSL_CTX*                    ctx;
    QString                     hostname;
    int                         port;
    std::string                 session;
    SSL_CTX*                    InitCTX(std::string fileCert);
    void                        setNonBlocking(int &sock);

signals:

public slots:
    bool                        main_connectToServer(QString host, int port);
    bool                        authenConnection(QString username, QString password);
    int                         file_connectToserver();
    bool                        sendRequestUpload(QString filepatch);
    bool                        send_CMD_MSG_FILE(QString _sender, QString _receiver, QString _urlfile);
};

#endif // MANAGECONNECTION_H
