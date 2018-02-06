#ifndef PACKET_H
#define PACKET_H

#include "filesys.h"

class Packet
{
public:
    Packet();
    Packet(const PACKET & pk);
    Packet(const std::string & s);

    ~Packet();

    bool appendData(int cmd);
    bool appendData(std::string s);
    bool IsAvailableData();
    PACKET getData();
    char * getDataToArray();
    int getCMDHeader();
    std::string getContent();

private:
    PACKET data;

    PACKET buildIntField(int cmd);
    PACKET buildStringField(std::string sdata);
};

#endif // PACKET_H
