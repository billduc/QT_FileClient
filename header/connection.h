#ifndef CONNECTION_H
#define CONNECTION_H

#include "filesys.h"
#include "packet.h"
#include "filehandle.h"

class Connection
{
public:
    Connection(SSL_CTX * ctxp, int id = 1);
    ~Connection();


    //APIs global
    bool                TCPconn(std::string host, int port);
    bool                TLSconn();
    bool                conn_To_Server(std::string host, int port);
    std::string         get_session() {return this->_session;}
    void                set_session(std::string s) {this->_session = s;}
    int                 get_Id() {return this->_Id;}
    void                set_Id(int id) {this->_Id = id;}
    int                 get_SocketFd() {return this->_socketFd;}

    //APIs handle main connection
    bool                send_Login_Request(std::string username, std::string password);
    bool                get_Status_Login_Success();
    void                set_Status_Login_Success(bool _state);
    void                send_PING();

    //APIs handle file connection
    bool                send_Requset_Upload(std::string filepatch);
    bool                share_File(std::string sender, std::string receiver, std::string filepatch);
    bool                send_File(std::string filepatch);
    std::string         get_Url_File_Server();
    bool                get_Status_Send_File_Finished();
    void                set_Status_Send_File_Finished(bool _state);

    //craft function
    bool                fsend(std::string filepath);


private:
    int                 _socketFd;
    int                 _Id;
    SSL_CTX*            _ctx;
    SSL*                _ssl;
    bool                _isMainConnection;
    bool                _isFileConnection;
    struct timeval      _timeout;
    fd_set              _workingSet;
    std::string         _session;
    FileHandle*         _file;
    std::string         _urlFileServer;
    bool                _statusSendFileFinished;
    bool                _statusLoginSuccess;

    char                buffer[BUFFSIZE];

    int                 get_CMD_HEADER();

    void                set_Non_Blocking(int &sock);
    void                show_Certs(SSL *ssl);
    bool                handle_Classify_Connection();
    SSL_CTX*            InitCTX(std::string fileCert);
    bool                send_CMD_MSG_FILE(std::string _sender, std::string _receiver);

    //these APIs for file connection
    bool                send_CMD_UPLOAD_FINISH();
    bool                check_Respond_CMD_SAVE_FILE_FINISH();
};

#endif // CONNECTION_H
