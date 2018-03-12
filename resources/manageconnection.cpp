#include "../header/manageconnection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}

ManageConnection::ManageConnection(QObject *parent) : QObject(parent)
{
    this->_ctx       = this->InitCTX("/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem");
    this->_hostName  = "localhost";
    this->_port      = 443;
}

ManageConnection::~ManageConnection()
{

    delete this->_mainConnection;
    rep(i,this->_listFileConnections.size()){
        delete this->_listFileConnections.at(i);
    }
    _listFileConnections.clear();
    SSL_CTX_free(this->_ctx);
}

SSL_CTX*
ManageConnection::InitCTX(std::string fileCert)
{
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */

    ctx = SSL_CTX_new( TLSv1_2_client_method() );   /* Create new context */

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        printf("Eroor: %s\n",stderr);
        abort();
    }

    //"/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem"
    if ( SSL_CTX_use_certificate_file(ctx, fileCert.c_str(),SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

void
ManageConnection::setNonBlocking(int &sock)
{
    int opts = fcntl(sock,F_GETFL, 0);
    if (opts < 0) {
        std::cerr << "@log: Error getting socket flags" << std::endl;
        return;
    }

    opts = (opts | O_NONBLOCK);

    if (fcntl(sock,F_SETFL,opts) < 0) {
        std::cerr << "@log: Error setting socket to non-blocking" << std::endl;
        return;
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
    return this->_mainConnection->send_Login_Request(username.toStdString(), password.toStdString());
}

int
ManageConnection::file_connectToserver()
{
    Connection * conn = new Connection(this->_ctx, _listFileConnections.size());
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

QString
ManageConnection::get_Hostname()
{
    return this->_hostName;
}

void
ManageConnection::set_Hostname(QString _hostName)
{
    this->_hostName = _hostName;
}

int
ManageConnection::get_Port()
{
    return this->_port;
}

void
ManageConnection::set_Port(int _port)
{
    this->_port = _port;
}


