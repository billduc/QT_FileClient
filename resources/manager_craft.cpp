#include "../header/manageconnection.h"


bool
ManageConnection::sendRequestUpload(QString filepatch)
{
    int id = this->file_connectToserver();

    if (id == -1){
        std::cerr << "Log managerConnection: error create file connection to server!!" << std::endl;
        return false;
    }
    else{
        std::cout <<"Log managerConnection: create file connecion success" << std::endl;
    }

    this->_listFileConnections.at(id)->send_Requset_Upload(filepatch.toStdString());

    delete this->_listFileConnections.at(id);
    this->_listFileConnections.erase(this->_listFileConnections.begin()+id);
}
