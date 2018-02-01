#ifndef PACKET_H
#define PACKET_H

#include "filesys.h"

class Packet
{
public:
    Packet();
    void build(int cmd);
    std::string readHeader();
    std::string readData();

private:
    std::vector<unsigned char> data;


    std::vector<unsigned char> buildIntField(int cmd);
    std::vector<unsigned char> buildStringField(std::string sdata);
};

#endif // PACKET_H
