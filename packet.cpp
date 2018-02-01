#include "packet.h"

Packet::Packet()
{
    this->data.clear();
}


std::vector<unsigned char> Packet::buildIntField(int cmd){
    std::vector<unsigned char> vc (4);
    /*
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    */
    vc[0] = (cmd >> 24) & 0xFF;
    vc[1] = (cmd >> 16) & 0xFF;
    vc[2] = (cmd >> 8 ) & 0xFF;
    vc[3] =  cmd   		& 0xFF;
    return vc;
}

std::vector<unsigned char> Packet::buildStringField(std::string sField){
    int leng = sField.length();

    std::vector<unsigned char> sData;
    sData.clear();

    std::vector<unsigned char> vtlength;
    vtlength = this->buildIntField(leng);

    sData.insert(sData.end(), vtlength.begin(), vtlength.end());
    sData.insert(sData.end(), sField.begin(), sField.end());

    return sData;
}

PACKET Packet::getData(){
    return this->data;
}





