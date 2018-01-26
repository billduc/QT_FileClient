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
    return ctx;
}

int  UploadFile::initConnnect(std::string host, int port)
{
    this->ctx = InitCTX();
    this->ssl = SSL_new(this->ctx);

    int sd = initConnnectC(host, port);

    std::cout << "finish connect tcp" << std::endl;

    SSL_set_fd(this->ssl, sd);

    if (sd < 0)
        return FAIL;
    else{
        if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
            {
                printf("Eroor: ssl connection\n");
                ERR_print_errors_fp(stderr);
                return FAIL;
            }
        else {

            std::string md5code = md5("test");
            //send(socketfd, md5code.c_str(), md5code.length(),0);

            SSL_write(this->ssl, md5code.c_str(), md5code.length());

            bool okConn = false;

            bzero(this->buffer, sizeof this->buffer);

            //recv(socketfd, buffer, sizeof(buffer), 0);
            SSL_read(this->ssl, this->buffer, sizeof(this->buffer));

            if ( strcmp(this->buffer,"200 ok") ==0 ){
                std::cout << "auth ok!! confirmed connection" << std::endl;
                okConn = true;
                return sd;
            }
            else{
                std::cout << "authentication failure!!!" << std::endl;
                return FAIL;
            }
        }

    }
}

bool UploadFile::upFile(QString filename)
{
   filename.remove(0,7);
   std::cout << filename.toStdString() << std::endl;

   int fd;
   //fd =  initConnnect("172.16.80.26",443);
   fd =  initConnnect("localhost",443);
   char buffer[12345];

   if (fd < 0){
        std::cerr <<"cannot establish connection to server" << std::endl;
        return false;
   } else {
        FILE * fp = fopen(filename.toStdString().c_str(), "r");

        if (fp == NULL){
            std::cerr << "Cannot open file" << std::endl;
        } else {
            bzero( buffer, sizeof buffer);
            int fs_block_size;

            while (fs_block_size = fread(buffer, sizeof(char), sizeof(buffer), fp ) > 0 ){
                std::cout << "send file " << fs_block_size << std::endl;
                if (send(fd, buffer, fs_block_size,0) < 0){
                    std::cerr << "ERROR: Fail to send file\n" << std::endl;
                    break;
                }
                bzero(buffer, sizeof(buffer));
            }

            std::cout << "Done!!!!" << std::endl;

            long FileSize = ftell(fp);

            std::cout <<"file size: " <<  FileSize << std::endl;
        }
        return true;
   }
}
