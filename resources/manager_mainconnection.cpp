#include "../header/manageconnection.h"

void
ManageConnection::wait_Auth_Main_Connection()
{
    for(;;){
        sleep(1);
        if (this->_mainConnection->get_Status_Login_Success())
            return;
    }
}

void
ManageConnection::thread_Handle_Main_Connection()
{
    struct timeval  _time;
    fd_set          _mainSet;
    int             _rc = -1;
    int             _socketFd;
    int             _cmd;

    if (this->_mainConnection->get_Status_Login_Success()){
        _socketFd   = this->_mainConnection->get_SocketFd();
        _time       = this->_timeoutClient;
        FD_ZERO(&_mainSet);
        FD_SET(_socketFd,&_mainSet);
        this->_mainConnection->send_PING();

        for(;;){
            _rc = select(_socketFd+1, &_mainSet, NULL, NULL, &_time );
            if (_rc > 0){
                _cmd = this->_mainConnection->get_CMD_HEADER();

            }
        }
    }
}

bool
ManageConnection::main_connectToServer(QString host, int _port)
{
    this->_mainConnection = new Connection(this->_ctx);
    return this->_mainConnection->conn_To_Server(this->_hostName.toStdString(), this->_port);
}

bool
ManageConnection::auth_Connection(QString username, QString password)
{
    //this->main_connectToServer();
    this->_mainConnection->send_Login_Request(username.toStdString(), password.toStdString());
    if (this->_mainConnection->get_Status_Login_Success()){
        this->_threadMainConn = new std::thread(&ManageConnection::thread_Handle_Main_Connection, this);
        return true;
    }
    return false;
}
