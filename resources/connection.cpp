#include "../header/connection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}


Connection::Connection(SSL_CTX * ctxp, int id) : _Id(id)
{
    this->_ctx                      = ctxp;
    this->_timeout.tv_sec           = 5;
    this->_timeout.tv_usec          = 0;
    this->_receivedPart             = 0;
    this->_isMainConnection         = false;
    this->_isFileConnection         = false;
    this->_statusLoginSuccess       = false;
    this->_statusSendFileFinished   = false;
    this->_dataWriteDoneState       = false;
    this->_file                     = new FileHandle();
}

Connection::~Connection(){
    SSL_free(this->_ssl);
    close(this->_socketFd);
    delete this->_file;
}

SSL_CTX*
Connection::InitCTX(std::string fileCert)
{
    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();   /* Load cryptos, et.al. */
    SSL_load_error_strings();       /* Bring in and register error messages */

    ctx = SSL_CTX_new( TLSv1_2_client_method() );   /* Create new context */

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        printf("Eroor: %s\n",stderr);
        abort();
    }

    if ( SSL_CTX_use_certificate_file(ctx, fileCert.c_str(),SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

bool
Connection::TCPconn(std::string ipAddr, int port)
{
    this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "@connection log: fd " << this->_socketFd << std::endl;
    if (this->_socketFd < 0){
        std::cerr <<"@connection log: ERROR create socket!!!" << std::endl;
        return false;
    }

    //set non-blocking model for socket
    //this->setNonBlocking(this->_socketFd);

    struct hostent *server;
    //struct addrinfo *iServer;
    struct sockaddr_in serv_addr;

    server = gethostbyname(ipAddr.c_str());

    if ( server == NULL ){
        std::cerr << "@connection log: ERROR, no such host" << std::endl;
        return false;
    }

    bzero( (char *) &serv_addr, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET;

    bcopy( (char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length );

    serv_addr.sin_port = htons(port);

    if ( connect(this->_socketFd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) ){
        std::cerr << "@connection log: ERROR connectiong to server!!!" << std::endl;
        return -1;
    }

    return true;
}

bool
Connection::TLSconn()
{
    this->_ssl = SSL_new(this->_ctx);

    SSL_set_fd(this->_ssl, this->_socketFd);

    SSL_set_connect_state(this->_ssl);

    if (SSL_connect(this->_ssl) == -1 ){
        std::cerr << "ERROR: ssl connection fail" << std::endl;
        int ret = 888;
        SSL_get_error(this->_ssl, ret);
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
        std::cout << "Log: Encryption: " << SSL_get_cipher(this->_ssl) << std::endl;
        return true;
    }
}

void
Connection::set_Non_Blocking(int &sock)
{
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

bool
Connection::conn_To_Server(std::string host, int port)
{
    if ( !this->TCPconn(host, port) ){
        std::cerr << "ERROR: Fail to connecting to server" << std::endl;
        return false;
    }

    std::cout << "TCP connection established" << std::endl;

    if ( !this->TLSconn() ){
        std::cerr <<"ERROR: Fail to establish TLS connection" << std::endl;
        return false;
    }
    std::cout <<"TLS connection established " << SSL_get_fd(this->_ssl)  <<  std::endl;
    return true;
}

void
Connection::send_CMD_HEADER(int _CMD)
{
    Packet *_pk = new Packet();
    _pk->appendData(_CMD);
    SSL_write(this->_ssl, &_pk->getData()[0], _pk->getData().size() );
    delete _pk;
    return;
}

bool
Connection::handle_Classify_Connection()
{
    Packet *pk = new Packet();

    if (this->_isMainConnection)
        pk->appendData(CMD_IS_MAIN_CONNECTION);

    if (this->_isFileConnection)
        pk->appendData(CMD_IS_FILE_CONNECTION);

    SSL_write(this->_ssl,  &pk->getData()[0], pk->getData().size() );
    delete pk;

    int             rc;
    struct timeval  time;

    FD_ZERO(&this->_workingSet);
    FD_SET(this->_socketFd, &this->_workingSet);

    time    =   this->_timeout;
    rc      =   select(this->_socketFd + 1, &this->_workingSet, NULL, NULL, &time);

    if (rc == 0){
        std::cerr << "timeout classify connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    int bytes   = SSL_read(this->_ssl, this->buffer, sizeof(this->buffer));
    pk          = new Packet(std::string(this->buffer,bytes));
    int cmd     = pk->getCMDHeader();

    std::cout << "cmd respond: " << cmd << std::endl;

    if (cmd == CMD_CLASSIFY_DONE){
        std::cout << "classify connection done." << std::endl;
    } else {
        std::cerr << "fail classify conneciton!!! exit." << std::endl;
        exit(EXIT_FAILURE);
    }

    delete pk;
    return true;
}

std::string
Connection::get_Url_File_Server()
{
    return this->_urlFileServer;
}

bool
Connection::send_CMD_MSG_FILE(std::string _sender, std::string _receiver)
{
    Packet*     _pk;
    //send CMD_MSG_FILE
    _pk = new Packet();

    _pk->appendData(CMD_MSG_FILE);
    //pk->appendData(this->_session);
    _pk->appendData(_sender);
    _pk->appendData(_receiver);
    _pk->appendData(this->_urlFileServer);
    _pk->appendData(this->_file->get_Size_stdString());

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
    return true;
}

int
Connection::get_CMD_HEADER()
{
    Packet*         _pk;
    int             _num_Fd_Incomming, _bytes, _cmd;
    struct timeval  _time = this->_timeout;
    fd_set          _fdset;

    FD_ZERO(&_fdset);
    FD_SET(this->_socketFd, &_fdset);

    _num_Fd_Incomming = select(this->_socketFd+1, &_fdset, NULL, NULL, &_time);

    std::cerr << "log before select " << SSL_get_fd(this->_ssl) << " " << _num_Fd_Incomming << std::endl;

    if (_num_Fd_Incomming == 0){
        std::cerr << "timeout!!!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bzero(this->buffer, sizeof(this->buffer));

    _bytes   = SSL_read(this->_ssl, this->buffer, 4);
    _pk      = new Packet(std::string(this->buffer,_bytes));
    std::cout << "Log Connection: size read header " << _bytes << std::endl;
    if (_pk->IsAvailableData())
        _cmd = _pk->getCMDHeader();

    delete _pk;
    return _cmd;
}
































