#include "../header/filehandle.h"

FileHandle::FileHandle()
{
    this->_fileSize     = -1;
    this->_filePatch    = "";
}

FileHandle::~FileHandle()
{
    if (this->_readStream.is_open())
        this->_readStream.close();
    if (this->_writeStream.is_open())
        this->_writeStream.close();
}

FileHandle::FileHandle(std::string _patch){
    this->_filePatch    = _patch;
    this->_fileSize     = -1;
}

bool
FileHandle::open_File_To_Read(std::string _fileName)
{
    // modes for binary file  |std::ios::ate
    this->_readStream.open(_fileName.c_str(), std::ios::in|std::ios::binary);
    if (this->_readStream.fail()) {
        std::cout   << "Reading file '"     << _fileName
                    << "' failed!"          << std::endl;

        return (false);
    }
    if (this->_readStream.is_open()) {
        this->_readStream.seekg(0, this->_readStream.beg);
        return (true);
    }
    std::cerr   << "Unable to open file '"  << _fileName
                << " '"                     << std::endl;
    return (false);
}

bool
FileHandle::open_File_To_Read(){
    this->_readStream.open(this->_filePatch.c_str(), std::ios::in|std::ios::binary);
    if (this->_readStream.fail()) {
        std::cout   << "Reading file '"     << this->_filePatch
                    << "' failed!"          << std::endl;

        return (false);
    }
    if (this->_readStream.is_open()) {
        this->_readStream.seekg(0, this->_readStream.beg);
        return (true);
    }
    std::cerr   << "Unable to open file '"  << this->_filePatch
                << " '"                     << std::endl;
    return (false);
}

void
FileHandle::close_Read_Stream(){
    this->_readStream.close();
}

std::string
FileHandle::get_File_Patch(){
    return this->_filePatch;
}

void
FileHandle::format_File_Patch(std::string& filename)
{
    //check filepatch format QML
    //if (filename.find("file://")){
        filename.erase(filename.begin(), filename.begin()+7);
        std::cout   << "!Log file: file path formated: " << filename
                    << std::endl;
    //}
    return;
}

std::string
FileHandle::get_File_Name(std::string filepath)
{
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
        /*
        std::cout   << post
                    << " " << filepath
                    << " " << sizeof(filepath)
                    << " " << filepath.length() - post
                    << std::endl;
        */
        std::string filename = filepath.substr(post+1, filepath.length() - post);
        return filename;
    }
}

void
FileHandle::read_File_Block(char* buffer, int sizeblock)
{
    this->_readStream.read(buffer, sizeblock);
}


int
FileHandle::write_File_Block(std::string content) {
    if(!this->_writeStream) {
        std::cerr   << "Cannot write to output file"
                    << std::endl;
        return (EXIT_FAILURE);
    }
    std::cout   << "$Log FileHandle: Appending to file"
                << std::endl;

    //this->_dataReceived += content.length();
    (this->_writeStream) << content;
    return (EXIT_SUCCESS);
}

int
FileHandle::begin_Write_File(std::string _fileName) {
    this->_writeStream.open(_fileName.c_str(), std::ios::out|std::ios::binary); // output file
    if(!this->_writeStream) {
        std::cerr   << "$Log FileHandle: Cannot open output file '"   << _fileName
                    << "'"
                    << std::endl;

        return (EXIT_FAILURE);
    }
    std::cout   << "$Log FileHandle: Beginning writing to file '"   << _fileName
                << "'"
                << std::endl;

    return (EXIT_SUCCESS);
}
/*
 *get size from of file
 */
long long
FileHandle::get_Size()
{
    std::streampos  _begin, _end;
    std::ifstream   _myfile(this->_filePatch.c_str(), std::ios::binary);
    _begin  = _myfile.tellg();
    _myfile.seekg (0, std::ios::end);
    _end    = _myfile.tellg();
    _myfile.close();
    return (_end - _begin);
}

void
FileHandle::set_File(std::string file){
    this->_filePatch = file;
    this->compute_Size();
}

void
FileHandle::compute_Size()
{
    std::streampos  _begin, _end;
    std::ifstream   _myfile(this->_filePatch.c_str(), std::ios::binary);
    _begin  = _myfile.tellg();
    _myfile.seekg (0, std::ios::end);
    _end    = _myfile.tellg();
    _myfile.close();
    this->_fileSize = _end - _begin;
}

std::string
FileHandle::get_Size_stdString(){
    return std::to_string(this->_fileSize);
}


















