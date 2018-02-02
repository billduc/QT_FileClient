#ifndef PACKET_H
#define PACKET_H

#include "filesys.h"

class Packet
{
public:
    Packet();

    ~Packet();

    bool appendData(int cmd);
    bool appendData(std::string s);
    std::string readHeader();
    std::string readData();
    PACKET getData();
private:
    PACKET data;

    PACKET buildIntField(int cmd);
    PACKET buildStringField(std::string sdata);
};

#endif // PACKET_H
