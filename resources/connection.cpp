#include "../header/connection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}


Connection::Connection(SSL_CTX * ctxp, int id) : _Id(id)
{
    this->_ctx                   = ctxp;
    this->_timeout.tv_sec        = 5;
    this->_timeout.tv_usec       = 0;
    this->_isMainConnection      = false;
    this->_isFileConnection     = false;
    this->_file                 = new FileHandle();
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

bool
Connection::send_Login_Request(std::string username, std::string password)
{
    Packet*     _pk;
    int         bytes, cmd;
    //send cmd to specify this connection is mainconnection
    this->_isFileConnection     = false;
    this->_isMainConnection     = true;
    this->handle_Classify_Connection();

    //send login request
    _pk = new Packet();

    _pk->appendData(CMD_AUTHEN_LOGIN);
    _pk->appendData(username);
    _pk->appendData(password);

    std::cout << "username: " << username << " passwork: " << password << std::endl;
    std::cout << SSL_get_fd(this->_ssl) << " send CMD request login "  << _pk->getData().size() << " - " << _pk->getData_stdString() << std::endl;

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size() );

    delete _pk;

    std::cout << "send CMD request login finished " << std::endl;

    //read data respond from server.
    bzero(this->buffer, sizeof(this->buffer));
    struct timeval time = this->_timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->_socketFd, &fdset);

    int rc = select(this->_socketFd+1, &fdset, NULL, NULL, &time);

    if (rc == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->_ssl, this->buffer, sizeof(this->buffer));
    _pk      = new Packet(std::string(buffer,bytes));
    cmd     = _pk->getCMDHeader();

    //std::cout << "read CMD reponse login finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;
    if (cmd == CMD_AUTHEN_SUCCESS) {
        std::cout << "login success" << std::endl;
        std::string sessionServerResponse = _pk->getContent();
        std::cout << "session: " << sessionServerResponse << std::endl;
        this->_session = sessionServerResponse;
    } else {
        std::cout << "login fail" << std::endl;
        return false;
    }

    delete _pk;

    return true;
}

bool
Connection::send_Requset_Upload(std::string filepatch)
{
    Packet*     pk;
    int         bytes;
    int         cmd;
    int         rc;
    std::string filename        = this->_file->get_File_Name(filepatch);

    this->_isFileConnection     = true;
    this->_isMainConnection     = false;
    this->handle_Classify_Connection();
    std::cout << "#log Connecion: before set file patch "<< filepatch << "inner class: " << this->_file->get_File_Patch() << std::endl;
    this->_file->format_File_Patch(filepatch);
    this->_file->set_File(filepatch);
    std::cout << "#log Connecion: before set file patch "<< filepatch << "inner class: " << this->_file->get_File_Patch() << std::endl;
    //build and send login request CMD_UPLOAD_FILE
    pk = new Packet();
    pk->appendData(CMD_UPLOAD_FILE);
    pk->appendData(this->_session);
    pk->appendData(filename);
    pk->appendData(this->_file->get_Size_stdString());

    SSL_write(this->_ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;

    std::cout << "send CMD request upload finished" << std::endl;

    bzero(this->buffer, sizeof(this->buffer));
    struct timeval time = this->_timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->_socketFd, &fdset);

    rc = select(this->_socketFd+1, &fdset, NULL, NULL, &time);

    std::cerr << "log before select " << SSL_get_fd(this->_ssl) << " " << rc << std::endl;

    if (rc == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->_ssl, this->buffer, sizeof(this->buffer));
    pk      = new Packet(std::string(buffer,bytes));
    if (pk->IsAvailableData())
        cmd = pk->getCMDHeader();

    std::cout << "read CMD reponse upload finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;

    if (cmd == CMD_UPLOAD_READY) {
        std::cout << "ready to upload file" << std::endl;
        if (pk->IsAvailableData()){
            this->_urlFileServer = pk->getContent();
            std::cout << "file url: " << this->_urlFileServer << std::endl;
        }

        //this->fsend(filepatch);
        this->send_File(filepatch);
    } else {
        std::cout << "request upload fail" << std::endl;
        return false;
    }

    return true;
}

bool
Connection::share_File(std::string _sender, std::string _receiver, std::string _filepatch){
    if (this->send_Requset_Upload(_filepatch)){
        std::cout << "send file to server done";
        this->send_CMD_UPLOAD_FINISH();
        if (this->check_Respond_CMD_SAVE_FILE_FINISH()){
            std::cout << "server save file finish. this connectin can be close" << std::endl;
            return true;
        } else {
            std::cerr << "some this wrong when save file to server!!! check again" << std::endl;
            return false;
        }
    }
    else{
        std::cerr << "some this wrong when send file to server! check again" << std::endl;
        return false;
    }
}

bool Connection::fsend(std::string filepath)
{
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
        int si = SSL_write(this->_ssl, this->buffer, sizeof(this->buffer));
        if (si < 0){
            std::cerr << " ssl wirte data error !!!! " << si << std::endl;
            int ret = 333;
            SSL_get_error(this->_ssl,ret);
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
        int si = SSL_write(this->_ssl, this->buffer, size_last_chunk);
        if (si < 0)
            std::cerr <<" ssl wirte data error !!!!" << std::endl;
        else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << size_last_chunk << std::endl;
            ++count;
        }
    }

    std::cout << "data sended: " << dataSend  << " of  Datasize: " << fileSize << std::endl;
    ///close(SSL_get_fd(this->_ssl));
    return true;
}

/*
 * send file to server
 * @_filepatch patch of file to send to server. (fomated according operation)
 */
bool
Connection::send_File(std::string _filepatch){
    long long   _size;
    long long   _dataSend   = 0;
    int         _count       = 1;
    this->_file->set_File(_filepatch);
    this->_file->open_File_To_Read();
    _size = this->_file->get_Size();

    size_t _totalChunks     =   _size / BUFFSIZE;
    size_t _sizeLastChunk   =   _size % BUFFSIZE;


    rep(i,_totalChunks){
        bzero(this->buffer, BUFFSIZE);
        this->_file->read_File_Block(this->buffer, BUFFSIZE);
        int si = SSL_write(this->_ssl, this->buffer, BUFFSIZE);
        _dataSend += si;
        std::cout << " ssl send ok " << _count  << ": " << si <<  " - " << sizeof(this->buffer) << std::endl;
        ++_count;
    }

    if (_sizeLastChunk > 0){
        bzero(this->buffer, BUFFSIZE);
        this->_file->read_File_Block(this->buffer, _sizeLastChunk);
        int si = SSL_write(this->_ssl, this->buffer, _sizeLastChunk);
        _dataSend += si;
        std::cout << " ssl send ok " << _count  << ": " << si <<  " - " << _sizeLastChunk << std::endl;
        ++_count;
    }

    std::cout << "data sended: " << _dataSend  << " of  Datasize: " << _size << std::endl;
    this->_file->close_Read_Stream();

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
        std::cerr << "timeout login request connection!!!" << std::endl;
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



bool
Connection::send_CMD_UPLOAD_FINISH()
{
    //send CMD_UPLOAD_FINISH
    Packet*     _pk;
    _pk = new Packet();

    _pk->appendData(CMD_UPLOAD_FINISH);

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
    return true;
}

bool
Connection::check_Respond_CMD_SAVE_FILE_FINISH()
{
    int _cmd = this->get_CMD_HEADER();
    if (_cmd == CMD_SAVE_FILE_FINISH)
        return true;
    else
        return false;
}
































