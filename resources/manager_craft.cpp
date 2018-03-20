#include "../header/manageconnection.h"


bool
ManageConnection::sendRequestUpload(QString filepatch)
{
    Connection *_conn = this->file_connectToserver();

    if (_conn == NULL){
        std::cerr << "Log managerConnection: error create file connection to server!!" << std::endl;
        return false;
    }
    else{
        std::cout <<"Log managerConnection: create file connecion success" << std::endl;
    }

    //this->_listFileConnections.at(id)->send_Requset_Upload(filepatch.toStdString());

    delete _conn;
    //this->_listFileConnections.erase(this->_listFileConnections.begin()+id);
}
