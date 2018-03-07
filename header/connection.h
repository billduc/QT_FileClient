#ifndef CONNECTION_H
#define CONNECTION_H

#include "filesys.h"
#include "packet.h"
#include "filehandle.h"

class Connection
{
public:
    Connection(SSL_CTX * ctxp, int id = 1);
    ~Connection();

    bool                TCPconn(std::string host, int port);
    bool                TLSconn();
    bool                ConnToServer(std::string host, int port);
    bool                sendLoginRequest(std::string username, std::string password);
    bool                send_Requset_Upload(std::string filepatch);
    bool                share_File(std::string sender, std::string receiver, std::string filepatch);
    std::string         get_session() {return session;}
    void                set_session(std::string s) {session = s;}
    int                 get_Id() {return this->Id;}
    void                set_Id(int id) {this->Id = id;}

    bool                fsend(std::string filepath);
    bool                send_File(std::string filepatch);

private:
    int                 socketfd;
    int                 Id;
    SSL_CTX*            ctx;
    SSL*                ssl;
    char                buffer[BUFFSIZE];
    bool                is_mainConnecion;
    bool                is_fileConnection;
    struct timeval      timeout;
    fd_set              working_set;
    std::string         session;
    FileHandle*         _file;
    std::string         _urlFileServer;

    void                set_Non_Blocking(int &sock);
    void                show_Certs(SSL *ssl);
    bool                handle_Classify_Connection();
    SSL_CTX*            InitCTX(std::string fileCert);
    bool                send_CMD_MSG_FILE(std::string _sender, std::string _receiver);
};

#endif // CONNECTION_H
