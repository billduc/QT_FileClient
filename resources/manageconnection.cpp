#include "../header/manageconnection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}

ManageConnection::ManageConnection(QObject *parent) : QObject(parent)
{
    this->ctx       = this->InitCTX("/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem");
    this->hostname  = "localhost";
    this->port      = 443;
}

ManageConnection::~ManageConnection()
{

    delete this->mainConnection;
    rep(i,this->listConnnection.size()){
        delete this->listConnnection.at(i);
    }
    listConnnection.clear();
    SSL_CTX_free(this->ctx);
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
ManageConnection::main_connectToServer(QString host, int port)
{
    this->mainConnection = new Connection(this->ctx);
    return this->mainConnection->ConnToServer(this->hostname.toStdString(), this->port);
}

bool
ManageConnection::authenConnection(QString username, QString password)
{
    //this->main_connectToServer();
    return this->mainConnection->sendLoginRequest(username.toStdString(), password.toStdString());
}

int
ManageConnection::file_connectToserver()
{
    Connection * conn = new Connection(this->ctx, listConnnection.size());
    if (conn->ConnToServer(this->hostname.toStdString(), this->port)){
        conn->set_session(this->mainConnection->get_session());
        listConnnection.pb(conn);
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
        return -1;
    }
    else{
        std::cout <<"Log managerConnection: create file connecion success" << std::endl;
    }

    this->listConnnection.at(id)->sendRequsetUpload(filepatch.toStdString());

    delete this->listConnnection.at(id);
    this->listConnnection.erase(this->listConnnection.begin()+id);
}

QString
ManageConnection::get_Hostname()
{
    return this->hostname;
}

void
ManageConnection::set_Hostname(QString hostname)
{
    this->hostname = hostname;
}

int
ManageConnection::get_Port()
{
    return this->port;
}

void
ManageConnection::set_Port(int port)
{
    this->port = port;
}

bool
ManageConnection::send_CMD_MSG_FILE(QString _sender, QString _receiver, QString _urlfile){
    int id = this->file_connectToserver();

    if (id == -1){
        std::cerr << "Log managerConnection: error create file connection to server!!" << std::endl;
        return -1;
    }
    else{
        std::cout <<"Log managerConnection: create file connecion success" << std::endl;
    }

    //this->listConnnection.at(id)->sendRequsetUpload(filepatch.toStdString());

    delete this->listConnnection.at(id);
    this->listConnnection.erase(this->listConnnection.begin()+id);
}
