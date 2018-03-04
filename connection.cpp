#include "connection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}


Connection::Connection(SSL_CTX * ctxp, int id) : Id(id)
{
    this->ctx                   = ctxp;
    this->timeout.tv_sec        = 5;
    this->timeout.tv_usec       = 0;
    this->is_mainConnecion      = false;
    this->is_fileConnection     = false;
    this->fileHandle            = new FileHandle();
}

Connection::~Connection(){
    SSL_free(this->ssl);
    close(this->socketfd);
    delete this->fileHandle;
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
    this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "@connection log: fd " << this->socketfd << std::endl;
    if (this->socketfd < 0){
        std::cerr <<"@connection log: ERROR create socket!!!" << std::endl;
        return false;
    }

    //set non-blocking model for socket
    //this->setNonBlocking(this->socketfd);

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

    if ( connect(this->socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) ){
        std::cerr << "@connection log: ERROR connectiong to server!!!" << std::endl;
        return -1;
    }

    return true;
}

bool
Connection::TLSconn()
{
    this->ssl = SSL_new(this->ctx);

    SSL_set_fd(this->ssl, this->socketfd);

    SSL_set_connect_state(this->ssl);

    if (SSL_connect(this->ssl) == -1 ){
        std::cerr << "ERROR: ssl connection fail" << std::endl;
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

void
Connection::setNonBlocking(int &sock)
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
Connection::ConnToServer(std::string host, int port)
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
    std::cout <<"TLS connection established " << SSL_get_fd(this->ssl)  <<  std::endl;
    return true;
}

bool
Connection::handleClassifyConnection()
{
    Packet *pk = new Packet();

    if (is_mainConnecion)
        pk->appendData(CMD_IS_MAIN_CONNECTION);

    if (is_fileConnection)
        pk->appendData(CMD_IS_FILE_CONNECTION);

    SSL_write(this->ssl,  &pk->getData()[0], pk->getData().size() );
    delete pk;

    int             rc;
    struct timeval  time;

    FD_ZERO(&this->working_set);
    FD_SET(this->socketfd, &this->working_set);

    time    =   this->timeout;
    rc      =   select(this->socketfd + 1, &this->working_set, NULL, NULL, &time);

    if (rc == 0){
        std::cerr << "timeout classify connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    int bytes   = SSL_read(this->ssl, this->buffer, sizeof(this->buffer));
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

bool
Connection::sendLoginRequest(std::string username, std::string password)
{
    //username = "user1";
    //password = "user1";
    Packet*     pk;
    int         bytes, cmd;
    //send cmd to specify this connection is mainconnection
    this->is_fileConnection = false;
    this->is_mainConnecion  = true;
    this->handleClassifyConnection();

    //send login request
    pk = new Packet();

    pk->appendData(CMD_AUTHEN_LOGIN);
    pk->appendData(username);
    pk->appendData(password);

    std::cout << "username: " << username << " passwork: " << password << std::endl;
    std::cout << SSL_get_fd(this->ssl) << " send CMD request login "  << pk->getData().size() << " - " << pk->getData_stdString() << std::endl;

    SSL_write(this->ssl,  &pk->getData()[0], pk->getData().size() );

    delete pk;

    std::cout << "send CMD request login finished " << std::endl;

    //read data respond from server.
    bzero(this->buffer, sizeof(this->buffer));
    struct timeval time = this->timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->socketfd, &fdset);

    int rc = select(this->socketfd+1, &fdset, NULL, NULL, &time);

    if (rc == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->ssl, this->buffer, sizeof(this->buffer));
    pk      = new Packet(std::string(buffer,bytes));
    cmd     = pk->getCMDHeader();

    //std::cout << "read CMD reponse login finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;
    if (cmd == CMD_AUTHEN_SUCCESS) {
        std::cout << "login success" << std::endl;
        std::string sessionServerResponse = pk->getContent();
        std::cout << "session: " << sessionServerResponse << std::endl;
        this->session = sessionServerResponse;
    } else {
        std::cout << "login fail" << std::endl;
        return false;
    }

    delete pk;

    return true;
}

bool
Connection::sendRequsetUpload(std::string filepatch)
{
    Packet*     pk;
    int         bytes;
    int         cmd;
    int         rc;
    std::string filename    = this->fileHandle->getFileName(filepatch);

    this->is_fileConnection = true;
    this->is_mainConnecion  = false;
    this->handleClassifyConnection();

    //send login request
    pk = new Packet();

    pk->appendData(CMD_UPLOAD_FILE);
    pk->appendData(this->session);
    pk->appendData(filename);

    SSL_write(this->ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;

    std::cout << "send CMD request upload finished" << std::endl;

    bzero(this->buffer, sizeof(this->buffer));
    struct timeval time = this->timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->socketfd, &fdset);

    rc = select(this->socketfd+1, &fdset, NULL, NULL, &time);

    std::cerr << "log before select " << SSL_get_fd(this->ssl) << " " << rc << std::endl;

    if (rc == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->ssl, this->buffer, sizeof(this->buffer));
    std::cerr << "log crash because build packet " << bytes << std::endl;
    pk      = new Packet(std::string(buffer,bytes));
    std::cerr << "log crash because get cmd header " << bytes << std::endl;
    if (pk->IsAvailableData())
        cmd = pk->getCMDHeader();

    std::cout << "read CMD reponse login finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;

    if (cmd == CMD_UPLOAD_READY) {
        std::cout << "ready to upload file" << std::endl;
        if (pk->IsAvailableData()){
            std::string fileUrl = pk->getContent();
            std::cout << "file url: " << fileUrl << std::endl;
        }

        this->fileHandle->format_FileName(filepatch);
        //std::cout << filepatch << std::endl;
        this->fsend(filepatch);
    } else {
        std::cout << "request upload fail" << std::endl;
        return false;
    }

    return true;
}


bool Connection::fsend(std::string filepath){
    std::ifstream ifFile(filepath.c_str(), std::ios::in | std::ios::binary);

    ifFile.seekg(0, std::ios::end);
    long long fileSize = ifFile.tellg();

    std::cout << "fileSize : " << fileSize << std::endl;

    size_t total_chunks     = fileSize / sizeof(this->buffer);
    size_t size_last_chunk  = fileSize % sizeof(this->buffer);

    unsigned int dataSend = 0;

    int count = 1;
    ifFile.seekg (0, ifFile.beg);
    for(int i = 0; i < total_chunks; ++i){
        ifFile.read(this->buffer, sizeof(this->buffer));
        int si = SSL_write(this->ssl, this->buffer, sizeof(this->buffer));
        if (si < 0){
            std::cerr << " ssl wirte data error !!!! " << si << std::endl;
            int ret = 333;
            SSL_get_error(this->ssl,ret);
            switch ( ret) {
                case SSL_ERROR_WANT_READ:
                    std::cerr << "SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                    std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl;
                    break;
                case SSL_ERROR_WANT_X509_LOOKUP:
                    std::cerr << "SSL_ERROR_WANT_X509_LOOKUP" << std::endl;
                    break;
                case SSL_ERROR_SYSCALL:
                    std::cerr << "SSL_ERROR_SYSCALL" << std::endl;
                    break;
                case SSL_ERROR_NONE:
                    std::cerr << "SSL_ERROR_NONE" << std::endl;
                    break;
                case SSL_ERROR_WANT_CONNECT:
                    std::cerr << "SSL_ERROR_WANT_CONNECT" << std::endl;
                    break;
                default:
                    std::cerr << "SSL_ERROR... " << ret <<  std::endl;
                    break;
            }
        } else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << sizeof(this->buffer) << std::endl;
            ++count;
        }
    }

    if (size_last_chunk > 0){
        ifFile.read(this->buffer, size_last_chunk);
        int si = SSL_write(this->ssl, this->buffer, size_last_chunk);
        if (si < 0)
            std::cerr <<" ssl wirte data error !!!!" << std::endl;
        else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << size_last_chunk << std::endl;
            ++count;
        }
    }

    std::cout << "data sended: " << dataSend  << " of  Datasize: " << fileSize << std::endl;
    ///close(SSL_get_fd(this->ssl));
    return true;
}



































