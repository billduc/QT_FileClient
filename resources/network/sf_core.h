#ifndef SF_CORE_H
#define SF_CORE_H

#include <QObject>
#include "../../header/filesys.h"

class SF_Core : public QObject
{
    Q_OBJECT
public:
    explicit SF_Core(QObject *parent = nullptr);
    SF_Core();
    ~SF_Core();



private:

};

#endif // SF_CORE_H
