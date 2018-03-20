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
ManageConnection::thread_Handle_Main_Connection_Keepalive()
{
    struct timeval  _time;
    fd_set          _mainSet;
    int             _rc = -1;
    int             _socketFd;
    int             _cmd;

    if (this->_mainConnection->get_Status_Login_Success()){
        _socketFd   = this->_mainConnection->get_SocketFd();

        FD_ZERO(&_mainSet);
        FD_SET(_socketFd,&_mainSet);
        for(;;){
            std::cout   << "thread send CMD"
                        << std::endl;

            if (this->_stopThreadMainConn)
                return;
            _time       = this->_timeoutClient;
            _rc = select(_socketFd+1, NULL, &_mainSet, NULL, &_time );
            if (_rc > 0){
                this->_mainConnection->send_PING();
                sleep(10);
            }
        }
    }
}

void
ManageConnection::thread_Handle_Main_Connection_Receive_CMD()
{
    struct timeval  _time;
    fd_set          _mainSet;
    int             _rc         = -1;
    int             _socketFd;
    int             _cmd;

    if (this->_mainConnection->get_Status_Login_Success()){
        _socketFd   = this->_mainConnection->get_SocketFd();

        for(;;){
            std::cout   << "thread receive CMD"
                        << std::endl;

            if (this->_stopThreadMainConn)
                return;
            _time       = this->_timeoutClient;
            FD_ZERO(&_mainSet);
            FD_SET(_socketFd,&_mainSet);
            _rc = select(_socketFd+1, &_mainSet, NULL,  NULL, &_time);
            if (_rc > 0){
                _cmd = this->_mainConnection->get_CMD_HEADER();
                std::cout   << "CMD respond: " << _cmd
                            << std::endl;
                    switch (_cmd) {
                    case PONG:
                        std::cout   << "receive PONG paket from server"
                                    << std::endl;

                        break;
                    case CMD_MSG_FILE:
                        std::cout   << "receive CMD_MSG_FILE paket from server"
                                    << std::endl;

                        FILE_TRANSACTION *_fileTransaction;
                        _fileTransaction = this->_mainConnection->handle_Receive_CMD_MSG_FILE();
                        emit signal_Notify_Download(QString::fromStdString(_fileTransaction->_sender),
                                                    QString::fromStdString(_fileTransaction->_receiver),
                                                    QString::fromStdString(_fileTransaction->_url),
                                                    QString::number(_fileTransaction->_filesize));
                        break;
                    default:
                        std::cout   << "unknow CMD!!!!"
                                    << std::endl;
                        break;
                }
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

        std::cout   << "check log thread "
                    << std::endl;

        this->_threadMainConn_send      = new std::thread(&ManageConnection::thread_Handle_Main_Connection_Keepalive, this);
        this->_threadMainConn_receive   = new std::thread(&ManageConnection::thread_Handle_Main_Connection_Receive_CMD, this);

        std::cout   << "check log thread 2"
                    << std::endl;
        return true;
    } else {
        delete this->_mainConnection;
        return false;
    }
}


void
ManageConnection::send_CMD_MSG_FILE()
{

}
