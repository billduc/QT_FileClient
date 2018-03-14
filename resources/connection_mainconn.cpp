#include "../header/connection.h"

bool
Connection::send_Login_Request(std::string _username, std::string _password)
{
    Packet*         _pk;
    int             bytes, cmd;
    std::string     _sessionServerResponse;
    char            _buffer[BUFFSIZE];
    //send cmd to specify this connection is mainconnection
    this->_isFileConnection     = false;
    this->_isMainConnection     = true;
    this->handle_Classify_Connection();

    //send login request
    _pk = new Packet();

    _pk->appendData(CMD_AUTHEN_LOGIN);
    _pk->appendData(_username);
    _pk->appendData(_password);

    std::cout << "username: " << _username << " passwork: " << _password << std::endl;
    std::cout << SSL_get_fd(this->_ssl) << " send CMD request login "  << _pk->getData().size() << " - " << _pk->getData_stdString() << std::endl;

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size() );

    delete _pk;

    std::cout << "send CMD request login finished " << std::endl;

    //read data respond from server.
    bzero(_buffer, sizeof(_buffer));
    struct timeval time = this->_timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->_socketFd, &fdset);

    int rc = select(this->_socketFd+1, &fdset, NULL, NULL, &time);

    if (rc <= 0){
        std::cerr << "timeout/error send login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    _pk     = new Packet(std::string(_buffer,bytes));
    if (_pk->IsAvailableData())
        cmd = _pk->getCMDHeader();

    std::cout << "read CMD reponse login finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;
    if (cmd == CMD_AUTHEN_SUCCESS) {
        std::cout << "login success" << std::endl;
        if (_pk->IsAvailableData())
            _sessionServerResponse = _pk->getContent();
        std::cout << "session: " << _sessionServerResponse << std::endl;
        this->_session = _sessionServerResponse;
        this->set_Status_Login_Success(true);
    } else {
        std::cout << "login fail" << std::endl;
    }

    delete _pk;

    return this->_statusLoginSuccess;
}

void
Connection::send_PING()
{
    Packet *_pk = new Packet();
    _pk->appendData(PING);
    SSL_write(this->_ssl, &_pk->getData()[0], _pk->getData().size() );
    delete _pk;
    return;
}

void
Connection::set_Status_Login_Success(bool _state)
{
    this->_statusLoginSuccess = _state;
}

bool
Connection::get_Status_Login_Success()
{
    return this->_statusLoginSuccess;
}


