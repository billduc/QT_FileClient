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

    Connection*                 _mainConnection;
    std::vector<Connection*>    _listFileConnections;
    SSL_CTX*                    _ctx;
    QString                     _hostName;
    int                         _port;
    std::string                 _session;
    std::thread*                _threadMainConn;
    bool                        _stopThreadMainConn;
    struct timeval              _timeoutClient;

    SSL_CTX*                    InitCTX(std::string fileCert);

    void                        setNonBlocking(int &sock);
    void                        thread_Handle_Main_Connection();
    void                        wait_Auth_Main_Connection();
signals:

public slots:
    bool                        main_connectToServer(QString host, int port);
    bool                        auth_Connection(QString username, QString password);

    //these APIs for file connection
    int                         file_connectToserver();
    bool                        sendRequestUpload(QString filepatch);
    bool                        share_File(QString sender, QString receiver, QString filepatch);
};

#endif // MANAGECONNECTION_H
