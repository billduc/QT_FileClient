#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "filesys.h"

class FileHandle
{
public:
    FileHandle();
    FileHandle(std::string patch);
    ~FileHandle();

    bool            open_File_To_Read(std::string fileName);
    bool            open_File_To_Read();
    void            close_Read_Stream();
    void            read_File_Block(char* buffer, int sizeblock);
    std::string     get_File_Name(std::string);
    void            format_FileName(std::string&);
    long long       get_Size();
    std::string     get_Size_stdString();
    void            set_File(std::string);

private:
    std::string     _filePatch;
    long long       _fileSize;

    std::ofstream   _writeStream; //stream to read data from socket and write to file
    std::ifstream   _readStream;  //stream to read from file and send to socket
    char            _buffer[BUFFSIZE];

    void            compute_Size();
};

#endif // FILEHANDLE_H
