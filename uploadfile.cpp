#include "uploadfile.h"

UploadFile::UploadFile(QObject *parent) : QObject(parent)
{

}

void UploadFile::doAction(){
    std::cout << "test function c++" << std::endl;
}

int UploadFile::initConnnect(std::string host, int port){
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

}

bool UploadFile::upFile(std::string filename)
{

}
