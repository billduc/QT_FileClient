#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "filesys.h"

class FileHandle
{
public:
    FileHandle();
    FileHandle(std::string patch);
    ~FileHandle();

    int             write_File_Block(std::string _content);
    int             begin_Write_File(std::string _fileName);

    bool            open_File_To_Read(std::string _fileName);
    bool            open_File_To_Read();
    void            close_Read_Stream();
    void            read_File_Block(char* _buffer, int _sizeblock);
    std::string     get_File_Name(std::string);
    void            format_File_Patch(std::string&);
    long long       get_Size();
    std::string     get_Size_stdString();
    void            set_File(std::string);
    std::string     get_File_Patch();
private:
    std::string     _filePatch;
    long long       _fileSize;



    std::ofstream   _writeStream; //stream to read data from socket and write to file
    std::ifstream   _readStream;  //stream to read from file and send to socket
    char            _buffer[BUFFSIZE];

    void            compute_Size();
};

#endif // FILEHANDLE_H
