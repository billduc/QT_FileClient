#ifndef CONNECTION_H
#define CONNECTION_H

#include "filesys.h"

class Connection
{
public:
    Connection();

    bool TCPconn(std::string ipAddr, int port);
    bool TLSconn(std::string fileCert);

private:
    int socketfd ;
    SSL_CTX * ctx;
    SSL * ssl;
    char buffer[5000];

    void setNonBlocking(int &sock);
    SSL_CTX* InitCTX(std::string fileCert);
};

#endif // CONNECTION_H
