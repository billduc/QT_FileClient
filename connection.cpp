#include "connection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}


Connection::Connection()
{

}

bool Connection::TCPconn(std::string ipAddr, int port){
    this->socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socketfd){
        std::cerr <<"ERROR create socket!!!" << std::endl;
        return false;
    }

    //set non-blocking model for socket
    this->setNonBlocking(this->socketfd);

    struct hostent *server;
    struct addrinfo *iServer;
    struct sockaddr_in serv_addr;

    server = gethostbyname(ipAddr.c_str());
    //iServer = getaddrinfo()

    if ( server == NULL ){
        std::cerr << "ERROR, no such host" << std::endl;
        return false;
    }

    bzero( (char *) &serv_addr, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET;

    bcopy( (char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length );

    serv_addr.sin_port = htons(port);

    if (connect(this->socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) < 0 ){
        std::cerr << "ERROR cannot connect to server" << std::endl;
        return false;
    }

    return true;
}

SSL_CTX* Connection::InitCTX(std::string fileCert) {
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */

    ctx = SSL_CTX_new( TLSv1_2_client_method() );   /* Create new context */

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        printf("Eroor: %s\n",stderr);
        abort();
    }

    //"/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem"
    if ( SSL_CTX_use_certificate_file(ctx, fileCert.c_str(),SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

void Connection::setNonBlocking(int &sock) {
    int opts = fcntl(sock,F_GETFL, 0);
    if (opts < 0) {
        std::cerr << "@log: Error getting socket flags" << std::endl;
        return;
    }

    opts = (opts | O_NONBLOCK);

    if (fcntl(sock,F_SETFL,opts) < 0) {
        std::cerr << "@log: Error setting socket to non-blocking" << std::endl;
        return;
    }
}
