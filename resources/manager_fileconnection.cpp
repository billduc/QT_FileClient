
#include "../header/manageconnection.h"


Connection*
ManageConnection::file_connectToserver()
{
    Connection* _conn = new Connection(this->_ctx, _listFileConnections.size());
    if (_conn->conn_To_Server(this->_hostName.toStdString(), this->_port)){
        _conn->set_session(this->_mainConnection->get_session());
        return _conn;
    } else {
        delete _conn;
        return NULL;
    }
}

bool
ManageConnection::share_File(QString sender, QString receiver, QString filepatch)
{
    Connection *_conn = this->file_connectToserver();

    if ( _conn == NULL){
        std::cerr   << "Log managerConnection: error create file connection to server!!"
                    << std::endl;
        return false;
    }
    else{
        std::cout   << "@Log managerConnection: create file connecion success"
                    << std::endl;

        connect(_conn, SIGNAL(signal_Persent_Progress(int)),
                this, SIGNAL(signal_Update_Persent_Progress(int)) );
    }

    //this->_listFileConnections.at(id)->sendRequsetUpload(filepatch.toStdString());
    bool ok = _conn->share_File(sender.toStdString(), receiver.toStdString(), filepatch.toStdString());

    if (ok){
        FILE_TRANSACTION *_fileTransaction;
        _fileTransaction = new FILE_TRANSACTION;

        _fileTransaction->_sender       =   _conn->get_Sender();
        _fileTransaction->_receiver     =   _conn->get_Receiver();
        _fileTransaction->_url          =   _conn->get_Url_File_Server();
        _fileTransaction->_filesize     =   _conn->get_Data_Size_Send_Int();
        _fileTransaction->_status       =   0;

        std::cout   <<  "Log file transaction: "
                    <<  "\n-_sender: "              << _fileTransaction->_sender
                    <<  "\n-_receiver: "            << _fileTransaction->_receiver
                    <<  "\n-_url: "                 << _fileTransaction->_url
                    <<  "\n-_filesize: "            << _fileTransaction->_filesize
                    <<  "\n-_status: "              << _fileTransaction->_status
                    << std::endl;

        this->_mainConnection->send_CMD_MSG_FILE(_fileTransaction);

        std::cout   << "send_CMD_MSG_FILE ok"
                    << std::endl;

        this->_ListFileTransactions.emplace_back(_fileTransaction);
        //std::cout << "check error1 " << std::endl;

    }

    //delete this->_listFileConnections.at(id);
    //std::cout << "check error2 " << std::endl;
    //this->_listFileConnections.erase(this->_listFileConnections.begin()+id);
    //std::cout << "check error3 " << std::endl;
    return true;
}

bool
ManageConnection::share_File_Save_Server(QString sender, QString receiver, QString filepatch)
{
    std::thread *_threadShareFile;
    _threadShareFile = new std::thread(&ManageConnection::share_File, this, sender, receiver, filepatch);
}

bool
ManageConnection::receive_File_Save_Server(QString _filename, QString _fileSize)
{
    std::thread *_threadReceiveFile;
    _threadReceiveFile = new std::thread(&ManageConnection::receive_File, this, _filename, _fileSize);
}


bool
ManageConnection::receive_File(QString _filename, QString _fileSize)
{
    Connection *_conn = this->file_connectToserver();
    if (_conn == NULL){
        std::cerr   << "@Log managerConnection: error create file connection to server!!"
                    << std::endl;
        return false;
    }
    else {
        std::cout   << "@Log managerConnection: create file connecion success"
                    << std::endl;

        connect(_conn, SIGNAL(signal_Persent_Progress(int)),
                this, SIGNAL(signal_Update_Persent_Progress(int)) );
    }

    _conn->receive_File(_filename.toStdString(), _fileSize.toLongLong());

    //delete this->_listFileConnections.at(_id);
    //this->_listFileConnections.erase(this->_listFileConnections.begin()+_id);

}
