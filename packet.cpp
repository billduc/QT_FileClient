#include "packet.h"

Packet::Packet()
{
    this->data.clear();
}

Packet::~Packet(){
    this->data.clear();
}

PACKET Packet::buildIntField(int cmd){
    PACKET vc (4);
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

PACKET Packet::buildStringField(std::string sField){
    int leng = sField.length();

    PACKET sData;
    sData.clear();

    PACKET vtlength;
    vtlength = this->buildIntField(leng);

    sData.insert(sData.end(), vtlength.begin(), vtlength.end());
    sData.insert(sData.end(), sField.begin(), sField.end());

    return sData;
}

PACKET Packet::getData(){
    return this->data;
}

bool Packet::appendData(int cmd){
    PACKET pk;
    pk.clear();
    pk = buildIntField(cmd);

    this->data.insert(this->data.end(), pk.begin(), pk.end());

    return true;
}

bool Packet::appendData(std::string s){
    PACKET pk;
    pk.clear();
    pk = buildStringField(s);

    this->data.insert(this->data.begin(), pk.begin(), pk.end());
    return true;
}



