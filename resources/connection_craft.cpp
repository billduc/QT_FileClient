#include "../header/connection.h"

bool Connection::fsend(std::string filepath)
{
    std::ifstream ifFile(filepath.c_str(), std::ios::in | std::ios::binary);

    ifFile.seekg(0, std::ios::end);
    long long fileSize = ifFile.tellg();

    std::cout << "fileSize : " << fileSize << std::endl;

    size_t total_chunks     = fileSize / sizeof(this->buffer);
    size_t size_last_chunk  = fileSize % sizeof(this->buffer);

    unsigned int dataSend = 0;

    int count = 1;
    ifFile.seekg (0, ifFile.beg);
    for(int i = 0; i < total_chunks; ++i){
        ifFile.read(this->buffer, sizeof(this->buffer));
        int si = SSL_write(this->_ssl, this->buffer, sizeof(this->buffer));
        if (si < 0){
            std::cerr << " ssl wirte data error !!!! " << si << std::endl;
            int ret = 333;
            SSL_get_error(this->_ssl,ret);
            switch ( ret) {
                case SSL_ERROR_WANT_READ:
                    std::cerr << "SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                    std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl;
                    break;
                case SSL_ERROR_WANT_X509_LOOKUP:
                    std::cerr << "SSL_ERROR_WANT_X509_LOOKUP" << std::endl;
                    break;
                case SSL_ERROR_SYSCALL:
                    std::cerr << "SSL_ERROR_SYSCALL" << std::endl;
                    break;
                case SSL_ERROR_NONE:
                    std::cerr << "SSL_ERROR_NONE" << std::endl;
                    break;
                case SSL_ERROR_WANT_CONNECT:
                    std::cerr << "SSL_ERROR_WANT_CONNECT" << std::endl;
                    break;
                default:
                    std::cerr << "SSL_ERROR... " << ret <<  std::endl;
                    break;
            }
        } else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << sizeof(this->buffer) << std::endl;
            ++count;
        }
    }

    if (size_last_chunk > 0){
        ifFile.read(this->buffer, size_last_chunk);
        int si = SSL_write(this->_ssl, this->buffer, size_last_chunk);
        if (si < 0)
            std::cerr <<" ssl wirte data error !!!!" << std::endl;
        else{
            dataSend += si;
            std::cout << " ssl send ok " << count  << ": " << si <<  " - " << size_last_chunk << std::endl;
            ++count;
        }
    }

    std::cout << "data sended: " << dataSend  << " of  Datasize: " << fileSize << std::endl;
    ///close(SSL_get_fd(this->_ssl));
    return true;
}
