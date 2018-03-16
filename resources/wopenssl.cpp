#include "../header/wrap.h"

int
SF_SSL_READ(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, _rc;
    fd_set      _fdSet;

    FD_ZERO(&_fdSet);
    FD_SET(_fd, &_fdSet);

    _rc = select(_fd + 1, &_fdSet, NULL, NULL, NULL);
    if (_rc <= 0)
        std::cerr << "select timeout or trouble" << std::endl;
    _bytes = SSL_read(_ssl, _buffer, _sizebuf);

    if (_bytes > 0)
        std::cout << "send ok" << std::endl;
    else
        std::cerr << "SSL_read have problem" << std::endl;

    return _bytes;
}

int
SF_SSL_WRITE(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, _rc;
    fd_set      _fdSet;

    FD_ZERO(&_fdSet);
    FD_SET(_fd, &_fdSet);

    _rc = select(_fd + 1, NULL, &_fdSet, NULL, NULL);

    if (_rc <= 0)
        std::cerr << "select timeout or trouble" << std::endl;

    _bytes = SSL_write(_ssl, _buffer, _sizebuf);

    if (_bytes <= 0)
        std::cout << "send ok" << std::endl;
    else
        std::cerr << "SSL_write have problem" << std::endl;

    return _bytes;
}
