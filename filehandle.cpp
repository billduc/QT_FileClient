#include "filehandle.h"

FileHandle::FileHandle()
{

}

FileHandle::~FileHandle(){

}

int FileHandle::openFile(std::string fileName){
    this->readStream.open(fileName.c_str(), std::ios::in|std::ios::binary); // modes for binary file  |std::ios::ate
    if (this->readStream.fail()) {
        std::cout << "Reading file '" << fileName << "' failed!" << std::endl; //  strerror(errno) <<
        return (false);
    }
    if (this->readStream.is_open()) {
        return (true);
    }
    std::cerr << "Unable to open file '" << fileName << " '" << std::endl; // << strerror(errno)
    return (false);
}

char* FileHandle::readFileBlock(unsigned long &sizeInBytes){
    this->readStream.seekg(0, std::ios::end);
    std::ifstream::pos_type size = this->readStream.tellg();
    sizeInBytes = (unsigned long) size;

    this->readStream.seekg(0, std::ios::beg);

    char * memblock = new char[size];
}
