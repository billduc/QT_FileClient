#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "filesys.h"

class FileHandle
{
public:
    FileHandle();
    ~FileHandle();

    bool            openFile(std::string fileName);
    char*           readFileBlock(unsigned long &sizeInBytes);
    std::string     getFileName(std::string);
    void            format_FileName(std::string&);
private:
    std::ofstream   writeStream; //stream to read data from socket and write to file
    std::ifstream   readStream;  //stream to read from file and send to socket
    char            buffer[MAXLINE];
};

#endif // FILEHANDLE_H
