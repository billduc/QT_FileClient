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
    bool                TCPconn(std::string _host, int _port);
    bool                TLSconn();
    bool                conn_To_Server(std::string host, int port);
    std::string         get_session() {return this->_session;}
    void                set_session(std::string s) {this->_session = s;}
    int                 get_Id() {return this->_Id;}
    void                set_Id(int id) {this->_Id = id;}
    int                 get_SocketFd() {return this->_socketFd;}
    int                 get_CMD_HEADER();
    void                send_CMD_HEADER(int _CMD);

    //APIs handle main connection
    bool                send_Login_Request(std::string _username, std::string _password);
    bool                get_Status_Login_Success();
    void                set_Status_Login_Success(bool _state);
    void                send_PING();
    void                send_CMD_MSG_FILE(FILE_TRANSACTION *_fileTransaction);
    FILE_TRANSACTION*   handle_Upload_CMD_MSG_FILE();

    //APIs handle file connection
    bool                send_Requset_Upload(std::string _filepatch);
    bool                send_Requset_Download(std::string _fileURL, long long _fileSize);
    bool                share_File(std::string _sender, std::string _receiver, std::string _filepatch);
    bool                receive_File(std::string _fileURL, long long _fileSize);
    bool                send_File(std::string _filepatch);
    bool                get_Status_Send_File_Finished();
    void                set_Status_Send_File_Finished(bool _state);
    void                write_Data(std::string _fileURL, long long _fileSize);
    std::string         get_Url_File_Server();
    std::string         get_Sender();
    std::string         get_Receiver();
    std::string         get_Data_Size_Send_stdString();
    long long           get_Data_Size_Send_Int();
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
    bool                _dataWriteDoneState;
    int                 _receivedPart;

    char                buffer[BUFFSIZE];

    //properties for File connection
    std::string         _sender;
    std::string         _receiver;
    std::string         _dataSizeSend_stdString;
    long long           _dataSizeSend_int;



    void                set_Non_Blocking(int &sock);
    void                show_Certs(SSL *ssl);
    bool                handle_Classify_Connection();
    SSL_CTX*            InitCTX(std::string fileCert);


    //these APIs for file connection
    bool                send_CMD_UPLOAD_FINISH();
    bool                check_Respond_CMD_SAVE_FILE_FINISH();
    bool                check_Respond_CMD_DOWNLOAD_FINISH();
};

#endif // CONNECTION_H
