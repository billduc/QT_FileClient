#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "filesys.h"

class FileHandle
{
public:
    FileHandle();
    ~FileHandle();
    bool openFile(std::string fileName);
    char* readFileBlock(unsigned long &sizeInBytes);

private:
    std::ofstream writeStream; //stream to read data from socket and write to file
    std::ifstream readStream; //stream to read from file and send to socket
};

#endif // FILEHANDLE_H
