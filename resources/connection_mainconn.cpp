#include "../header/connection.h"

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
    _pk     = new Packet(std::string(buffer,bytes));
    cmd     = _pk->getCMDHeader();

    //std::cout << "read CMD reponse login finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;
    if (cmd == CMD_AUTHEN_SUCCESS) {
        std::cout << "login success" << std::endl;
        std::string _sessionServerResponse = _pk->getContent();
        std::cout << "session: " << _sessionServerResponse << std::endl;
        this->_session = _sessionServerResponse;
        this->set_Status_Login_Success(true);
    } else {
        std::cout << "login fail" << std::endl;
        return false;
    }

    delete _pk;

    return true;
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


