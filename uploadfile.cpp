#include "uploadfile.h"

extern "C"
{
    int initConnnectC(std::string host, int port);
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

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0){
        std::cerr << "ERROR create socket";
        return EXIT_FAILURE;
    }

    server = gethostbyname( host.c_str() ) ;

    if (server == NULL) {
        std::cerr <<"ERROR, no such host\n";
        return EXIT_FAILURE;
    }

    bzero( (char *) &serv_addr, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(port);

    if (connect(socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) < 0 ){
        std::cerr << "ERROR connect to server" << std::endl;
        return EXIT_FAILURE;
    }


    std::cout <<"@Server connected" << std::endl;

    return socketfd;

}

int  UploadFile::initConnnect(std::string host, int port)
{
    return  initConnnectC(host, port);
}

bool UploadFile::upFile(QString filename)
{
   filename.remove(0,7);
   std::cout << filename.toStdString() << std::endl;

   int fd;
   fd =  initConnnect("172.16.80.25",443);
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
