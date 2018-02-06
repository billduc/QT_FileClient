#include "connection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}


Connection::Connection()
{
    this->ctx = this->InitCTX("/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem");
}

Connection::~Connection(){
    SSL_CTX_free(this->ctx);
}

bool Connection::TCPconn(std::string ipAddr, int port){
    this->socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socketfd < 0){
        std::cerr <<"ERROR create socket!!!" << std::endl;
        return false;
    }

    //set non-blocking model for socket
    //this->setNonBlocking(this->socketfd);

    struct hostent *server;
    //struct addrinfo *iServer;
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

    if ( connect(this->socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) ){
        std::cerr << "ERROR connectiong to server!!!" << std::endl;
        return -1;
    }

    return true;
}

bool Connection::TLSconn(std::string fileCerts){
    this->ssl = SSL_new(this->ctx);

    SSL_set_fd(this->ssl, this->socketfd);

    SSL_set_connect_state(this->ssl);

    if (SSL_connect(this->ssl) == -1 ){
        std::cerr <<"ERROR: ssl connection fail" << std::endl;
        int ret = 888;
        SSL_get_error(this->ssl, ret);
        switch (ret) {
            case SSL_ERROR_WANT_READ:
                std::cerr << "SSL_ERROR_WANT_READ" << std::endl;
                break;
            case SSL_ERROR_WANT_WRITE:
                std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl;
                break;
            case SSL_ERROR_WANT_CONNECT:
                std::cerr << "SSL_ERROR_WANT_CONNECT:" << std::endl;
                break;
            case SSL_ERROR_SYSCALL:
                std::cerr << "SSL_ERROR_SYSCALL" << std::endl;
                break;
            case SSL_ERROR_ZERO_RETURN:
                std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl;
                break;
            case SSL_ERROR_WANT_X509_LOOKUP:
                std::cerr << "SSL_ERROR_WANT_X509_LOOKUP" << std::endl;
                break;
            default:
                break;
        }
        return false;
    } else {
        std::cout << "Log: Establish TLS connection with server " << std::endl;
        std::cout << "Log: Encryption: " << SSL_get_cipher(this->ssl) << std::endl;
        return true;
    }
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

    //"/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pemc"
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

bool Connection::ConnToServer(std::string host, int port, std::string fileCert){
    if ( !this->TCPconn(host, port) ){
        std::cerr << "ERROR: Fail to connecting to server" << std::endl;
        return false;
    }

    std::cout << "TCP connection established" << std::endl;

    if ( !this->TLSconn(fileCert) ){
        std::cerr <<"ERROR: Fail to establish TLS connection" << std::endl;
        return false;
    }
    std::cout <<"TLS connection established" << std::endl;
    return true;
}

bool Connection::sendLoginRequest(std::string username, std::string password){
    username = "user1";
    password = "user1";

    Packet *pk = new Packet();

    pk->appendData(CMD_AUTHEN_LOGIN);
    pk->appendData(username);
    pk->appendData(password);

    /*
    rep(i,pk->getData().size()){
        std::cout << pk->getData().at(i);
    }
    */
    std::cout << "send CMD request login" << std::endl;

    char arr[123];
    memset(arr, 0, 123);
    rep(i,pk->getData().size()){
        arr[i] = pk->getData().at(i);
    }

    SSL_write(this->ssl,  arr, strlen(arr));
    std::cout << "send CMD request login finished " << arr << std::endl;
    return true;
}








































