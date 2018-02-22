#ifndef CONNECTION_H
#define CONNECTION_H

#include "filesys.h"
#include "packet.h"

class Connection
{
public:
    Connection(SSL_CTX * ctxp);
    ~Connection();

    bool TCPconn(std::string host, int port);
    bool TLSconn();
    bool ConnToServer(std::string host, int port);
    bool sendLoginRequest(std::string username, std::string password);

private:
    int socketfd ;
    SSL_CTX *ctx;
    SSL *ssl;
    char buffer[BUFFSIZE];
    bool is_mainConnecion;
    void setNonBlocking(int &sock);
    SSL_CTX* InitCTX(std::string fileCert);
    void ShowCerts(SSL *ssl);
};

#endif // CONNECTION_H
