#include "../header/wrap.h"

int
SF_SSL_READ(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, _rc;
    fd_set      _fdSetReader;

    FD_ZERO(&_fdSetReader);
    FD_SET(_fd, &_fdSetReader);

    _rc = select(_fd + 1, &_fdSetReader, NULL, NULL, NULL);
    if (_rc <= 0)
        std::cerr << "%log wopenssl: select timeout or trouble" << std::endl;
    _bytes = SSL_read(_ssl, _buffer, _sizebuf);

    if (_bytes > 0)
        std::cout << "%log wopenssl: SSL read data ok"          << std::endl;
    else
        std::cerr << "%log wopenssl: SSL_read have problem"     << std::endl;

    return _bytes;
}

int
SF_SSL_WRITE(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, _rc;
    fd_set      _fdSetWriter;

    FD_ZERO(&_fdSetWriter);
    FD_SET(_fd, &_fdSetWriter);

    _rc = select(_fd + 1, NULL, &_fdSetWriter, NULL, NULL);

    if (_rc <= 0)
        std::cerr << "%log wopenssl: select timeout or trouble" << std::endl;

    _bytes = SSL_write(_ssl, _buffer, _sizebuf);

    if (_bytes > 0)
        std::cout << "%log wopenssl: ssl write data ok"         << std::endl;
    else
        std::cerr << "%log wopenssl: SSL_write have problem"    << std::endl;

    return _bytes;
}
