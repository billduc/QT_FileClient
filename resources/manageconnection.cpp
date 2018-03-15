#include "../header/manageconnection.h"

extern "C"
{
    SSL_CTX* InitCTX(std::string fileCert);
    void setNonBlocking(int &sock);
}

ManageConnection::ManageConnection(QObject *parent) : QObject(parent)
{
    this->_ctx                      = this->InitCTX("/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem");
    this->_hostName                 = "localhost";
    this->_port                     = 443;
    this->_stopThreadMainConn       = false;
    this->_timeoutClient.tv_sec     = 3;
    this->_timeoutClient.tv_usec    = 0;

    this->_listFileConnections.clear();
    this->_ListFileTransactions.clear();
}

ManageConnection::~ManageConnection()
{
    //cancle thread
    this->_stopThreadMainConn = true;

    if (this->_threadMainConn_send->joinable())
        this->_threadMainConn_send->join();
    if (this->_threadMainConn_receive->joinable())
        this->_threadMainConn_receive->join();

    //clear memory
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


