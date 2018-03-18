
#include "../header/manageconnection.h"


int
ManageConnection::file_connectToserver()
{
    Connection* conn = new Connection(this->_ctx, _listFileConnections.size());
    if (conn->conn_To_Server(this->_hostName.toStdString(), this->_port)){
        conn->set_session(this->_mainConnection->get_session());
        _listFileConnections.pb(conn);
        return conn->get_Id();
    } else {
        delete conn;
        return -1;
    }
}

bool
ManageConnection::share_File(QString sender, QString receiver, QString filepatch)
{
    int id = this->file_connectToserver();

    if (id == -1){
        std::cerr << "Log managerConnection: error create file connection to server!!"  << std::endl;
        return false;
    }
    else{
        std::cout << "@Log managerConnection: create file connecion success"            << std::endl;
    }

    //this->_listFileConnections.at(id)->sendRequsetUpload(filepatch.toStdString());
    bool ok = this->_listFileConnections.at(id)->share_File(sender.toStdString(), receiver.toStdString(), filepatch.toStdString());
    if (ok){
        FILE_TRANSACTION *_fileTransaction;
        _fileTransaction = new FILE_TRANSACTION;

        _fileTransaction->_sender       =   this->_listFileConnections.at(id)->get_Sender();
        _fileTransaction->_receiver     =   this->_listFileConnections.at(id)->get_Receiver();
        _fileTransaction->_url          =   this->_listFileConnections.at(id)->get_Url_File_Server();
        _fileTransaction->_filesize     =   this->_listFileConnections.at(id)->get_Data_Size_Send_Int();
        _fileTransaction->_status       =   0;

        std::cout << "Log file transaction: "    <<
                    "\n-_sender: "              << _fileTransaction->_sender    <<
                    "\n-_receiver: "            << _fileTransaction->_receiver  <<
                    "\n-_url: "                 << _fileTransaction->_url       <<
                    "\n-_filesize: "            << _fileTransaction->_filesize  <<
                    "\n-_status: "              << _fileTransaction->_status    << std::endl;

        this->_mainConnection->send_CMD_MSG_FILE(_fileTransaction);

        std::cout << "send_CMD_MSG_FILE ok" << std::endl;

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
ManageConnection::receive_File(QString _filename, QString _fileSize)
{
    int _id = this->file_connectToserver();
    if (_id == -1){
        std::cerr << "@Log managerConnection: error create file connection to server!!"     << std::endl;
        return false;
    }
    else {
        std::cout << "@Log managerConnection: create file connecion success"                << std::endl;
    }

    this->_listFileConnections.at(_id)->receive_File(_filename.toStdString(), _fileSize.toLongLong());

    //delete this->_listFileConnections.at(_id);
    //this->_listFileConnections.erase(this->_listFileConnections.begin()+_id);

}
