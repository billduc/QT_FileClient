#ifndef SF_CONNECTION_H
#define SF_CONNECTION_H

#include <QSslSocket>
#include "../../header/filesys.h"

class SF_Connection
{
public:
    SF_Connection();

private:
    QSslSocket      _socket;
    bool            is_MainConnection;
    bool            is_FileConnection;

    QString         _sender;
    QString         _receiver;


};

#endif // SF_CONNECTION_H
