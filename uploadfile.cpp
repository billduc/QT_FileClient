#include "uploadfile.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

extern "C"
{
    int initConnnectC(std::string host, int port);
    SSL_CTX* InitCTX(void);
    void ShowCerts(SSL* ssl);
}
UploadFile::UploadFile(QObject *parent) : QObject(parent)
{

}

void UploadFile::doAction(){
    std::cout << "test function c++" << std::endl;
}

int initConnnectC(std::string host, int port){
    int socketfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buffer[12345];


    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0){
        std::cerr << "ERROR create socket";
        return FAIL;
    }

    server = gethostbyname( host.c_str() ) ;

    if (server == NULL) {
        std::cerr <<"ERROR, no such host\n";
        return FAIL;
    }

    bzero( (char *) &serv_addr, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(port);

    if (connect(socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) < 0 ){
        std::cerr << "ERROR connect to server" << std::endl;
        return FAIL;
    }


    std::cout <<"@Server connected" << std::endl;

//    std::string md5code = md5("test");
//    send(socketfd, md5code.c_str(), md5code.length(),0);

//    bool okConn = false;

//    recv(socketfd, buffer, sizeof(buffer), 0);

//    if ( strcmp(buffer,"200 ok") ==0 ){
//        std::cout << "auth ok!! confirmed connection" << std::endl;
//        okConn = true;
//    }
//    else{
//        std::cout << "authentication failure!!!" << std::endl;
//        return FAIL;
//    }

    return socketfd;

}

SSL_CTX* InitCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        printf("Eroor: %s\n",stderr);
        abort();
    }

    if ( SSL_CTX_use_certificate_file(ctx, "/media/veracrypt1/projects/QT_FileClient/CA/ca.crt.pem",SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

int  UploadFile::initConnnect(std::string host, int port)
{
    this->ctx = InitCTX();
    this->ssl = SSL_new(this->ctx);

    int sd = initConnnectC(host, port);
    if (sd > 0 )
        std::cout << "finish connect tcp" << std::endl;
    else
        std::cout << "connect tcp fail" << std::endl;

    if (sd < 0)
        return FAIL;
    else{
        SSL_set_fd(this->ssl, sd);
        SSL_set_connect_state(this->ssl);
        //SSL_connect(this->ssl);
        //std::cout << "ssl connected" << std::endl;
        if ( SSL_connect(this->ssl) == -1 )   /* perform the connection */
            {
                printf("Error: ssl connection\n");
                ERR_print_errors_fp(stderr);
                switch (SSL_get_error(this->ssl,1) ) {
                case SSL_ERROR_WANT_READ:
                    std::cout << "SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    std::cout << "SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                default:
                    std::cout << "SSL_ERROR... " << SSL_get_error(this->ssl,1) <<  std::endl;
                    break;
                }
                return FAIL;
            }
        else
        {
            printf("Connected with %s encryption\n", SSL_get_cipher(this->ssl));
            ShowCerts(this->ssl);

            std::string md5code = md5("test");
            //send(socketfd, md5code.c_str(), md5code.length(),0);

            SSL_write(this->ssl, md5code.c_str(), md5code.length());

            bool okConn = false;

            bzero(this->buffer, sizeof this->buffer);

            //recv(socketfd, buffer, sizeof(buffer), 0);
            SSL_read(this->ssl, this->buffer, sizeof(this->buffer));

            if ( strcmp(this->buffer,"200 ok") ==0 ){
                std::cout << "auth ok!! confirmed connection" << this->buffer << std::endl;
                okConn = true;
                return sd;
            }
            else{
                std::cout << "authentication failure!!! " << this->buffer << std::endl;
                return FAIL;
            }
        }

    }
}

std::string getFileName(std::string filepath){
    int post = -1;
    //linux operator
    for(int i = sizeof(filepath) - 1; i >=0; --i){
        if (filepath[i] == '/')
        {
            post = i;
            break;
        }
    }
    if (post == -1)
        return std::string(filepath);
    else{
        //cout << post  <<" " << filepath << " " << sizeof(filepath)<< " " << filepath.length() - post << endl;
        std::string filename = filepath.substr(post+1, filepath.length() - post);
        return filename;
    }
}

bool UploadFile::fsend(std::string filepath){
    std::ifstream ifFile(filepath.c_str(), std::ios::in | std::ios::binary);

    ifFile.seekg(0, std::ios::end);
    long long fileSize = ifFile.tellg();

    std::cout << "fileSize : " << fileSize << std::endl;

    size_t total_chunks = fileSize / sizeof(this->buffer);
    size_t size_last_chunk = fileSize % sizeof(this->buffer);

    unsigned int dataSend = 0;

    int count = 1;

    for(int i = 0; i < total_chunks; ++i){
        ifFile.read(this->buffer, sizeof(this->buffer));
        int si = SSL_write(this->ssl, this->buffer, sizeof(this->buffer));
        if (si < 0)
            std::cerr << " ssl wirte data error !!!!" << std::endl;
        else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << sizeof(this->buffer) << std::endl;
            ++count;
        }
    }

    if (size_last_chunk > 0){
        ifFile.read(this->buffer, size_last_chunk);
        int si = SSL_write(this->ssl, this->buffer, size_last_chunk);
        if (si < 0)
            std::cerr <<" ssl wirte data error !!!!" << std::endl;
        else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << size_last_chunk << std::endl;
            ++count;
        }
    }

    std::cout << "data sended: " << dataSend  << " of  Datasize: " << fileSize << std::endl;
    close(SSL_get_fd(this->ssl));
    return true;
}

bool UploadFile::Csend(std::string filepath){

    FILE * fp = fopen(filepath.c_str(), "r");

    if (fp == NULL){
        std::cerr << "Cannot open file" << std::endl;
        return true;
    } else {
        bzero( this->buffer, sizeof (this->buffer));
        int fs_block_size;

        struct stat statbuf;

        if (stat(filepath.c_str(), &statbuf) == -1){
            std::cerr << "get stat file error!!!" << std::endl;
        }

        long long totalSize = statbuf.st_size;
        long long dataSend = 0;

        //fseek(fp, 0, SEEK_SET);
        int count = 1;
        while ( (fs_block_size = fread(this->buffer, sizeof(char), sizeof(this->buffer), fp ) ) > 0 ){
            std::cout << "send file " << count << "  " << fs_block_size << std::endl;
            ++count;
            //if (send(fd, this->buffer, fs_block_size,0) < 0)
            dataSend += fs_block_size;

            if (SSL_write(this->ssl, this->buffer, fs_block_size) < 0)
            {
                std::cerr << "ERROR: Fail to send file\n" << std::endl;
                break;
            }
            bzero(this->buffer, sizeof(this->buffer));
        }

        std::cout << "Send file done!!!! totalSize " << totalSize << " sended " << dataSend << std::endl;

        long FileSize = ftell(fp);

        std::cout <<"file size: " <<  FileSize << std::endl;

        close(SSL_get_fd(this->ssl));
        return true;
    }
}

bool UploadFile::upFile(QString filename)
{
   filename.remove(0,7);
   std::cout << "file path: " << filename.toStdString() << std::endl;

   int fd;
   fd =  initConnnect("172.16.80.26",443);
   //fd =  initConnnect("localhost",443);

   if (fd < 0){
        std::cerr <<"cannot establish connection to server" << std::endl;
        return false;
   } else {

        std::cout << "file name: " <<  getFileName(filename.toStdString()) << std::endl;

        std::string cmd = "upload " + getFileName(filename.toStdString());

        //send (socketfd, cmd.c_str(), cmd.length(), 0);
        SSL_write(this->ssl, cmd.c_str(), cmd.length());

        //recv(socketfd, buffer, sizeof(buffer), 0 );
        bzero(this->buffer, sizeof(this->buffer));
        SSL_read(this->ssl, this->buffer, sizeof(this->buffer));

        std::cout <<"server answer: " << this->buffer << std::endl;

        this->fsend(filename.toStdString());

        return true;
   }
}
