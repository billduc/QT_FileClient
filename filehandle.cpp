#include "filehandle.h"

FileHandle::FileHandle()
{

}

FileHandle::~FileHandle()
{

}

bool
FileHandle::openFile(std::string fileName)
{
    // modes for binary file  |std::ios::ate
    this->readStream.open(fileName.c_str(), std::ios::in|std::ios::binary);
    if (this->readStream.fail()) {
        std::cout << "Reading file '" << fileName << "' failed!" << std::endl;
        return (false);
    }
    if (this->readStream.is_open()) {
        return (true);
    }
    std::cerr << "Unable to open file '" << fileName << " '" << std::endl;
    return (false);
}

void
FileHandle::format_FileName(std::string& filename)
{
    //check filepatch format QML
    //if (filename.find("file://")){
        filename.erase(filename.begin(), filename.begin()+7);
        std::cout << "!Log file: file path formated: " << filename << std::endl;
    //}
    return;
}


std::string
FileHandle::getFileName(std::string filepath)
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
        //cout << post  <<" " << filepath << " " << sizeof(filepath)<< " " << filepath.length() - post << endl;
        std::string filename = filepath.substr(post+1, filepath.length() - post);
        return filename;
    }
}

char*
FileHandle::readFileBlock(unsigned long &sizeInBytes)
{
    this->readStream.seekg(0, std::ios::end);
    std::ifstream::pos_type size = this->readStream.tellg();
    sizeInBytes = (unsigned long) size;

    this->readStream.seekg(0, std::ios::beg);

    char* memblock = new char[size];
}
