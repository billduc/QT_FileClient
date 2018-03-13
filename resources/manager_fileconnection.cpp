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
        std::cerr << "Log managerConnection: error create file connection to server!!" << std::endl;
        return false;
    }
    else{
        std::cout <<"@Log managerConnection: create file connecion success" << std::endl;
    }

    //this->_listFileConnections.at(id)->sendRequsetUpload(filepatch.toStdString());
    this->_listFileConnections.at(id)->share_File(sender.toStdString(), receiver.toStdString(), filepatch.toStdString());
    delete this->_listFileConnections.at(id);
    this->_listFileConnections.erase(this->_listFileConnections.begin()+id);
}
