#include "packet.h"

Packet::Packet()
{
    this->data.clear();
}

Packet::Packet(const PACKET & pk)
{
    this->data.clear();
    this->data = pk;
}

Packet::Packet(const std::string & str)
{
    this->data.clear();
    std::copy(str.begin(), str.end(), std::back_inserter(this->data) );
}

Packet::~Packet()
{
    this->data.clear();
}

PACKET
Packet::buildIntField(int cmd)
{
    PACKET vc (4,0);
    vc.clear();
    /*
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    */
    vc.push_back( (char) 0xFF & (cmd >> 24) );
    vc.push_back( (char) 0xFF & (cmd >> 16) );
    vc.push_back( (char) 0xFF & (cmd >> 8 ) );
    vc.push_back( (char) 0xFF &  cmd        );


    return vc;
}

PACKET
Packet::buildStringField(std::string sField)
{
    int leng = sField.length();

    PACKET sData;
    sData.clear();

    PACKET vtlength;
    vtlength = this->buildIntField(leng);

    //sData.insert(sData.end(), vtlength.begin(), vtlength.end());
    //sData.insert(sData.end(), sField.begin(), sField.end());

    std::copy(vtlength.begin(), vtlength.end(), std::back_inserter(sData));
    std::copy(sField.begin(), sField.end(), std::back_inserter(sData));
    return sData;
}

PACKET
Packet::getData()
{
    return this->data;
}

bool
Packet::appendData(int cmd)
{
    std::cout << "%Log build paccket: buil Int" << std::endl;
    PACKET pk;
    pk.clear();
    pk = buildIntField(cmd);

    std::copy(pk.begin(), pk.end(), std::back_inserter(this->data));

    return true;
}

bool
Packet::appendData(std::string s)
{
    std::cout << "%Log build paccket: buil string" << std::endl;
    PACKET pk;
    pk.clear();
    pk = buildStringField(s);

    std::copy(pk.begin(), pk.end(), std::back_inserter(this->data) );
    return true;
}


int
Packet::getCMDHeader()
{
    int cmd = 0;
    //cmd+= (this->data[0] << 24) + (this->data[1] << 16) + (this->data[2] << 8) + (this->data[3]);
    for(int i = 0; i < 4; ++i)
        cmd+= ((int)this->data[i] << ( 24 - i * 8 ) );

    this->data.erase(this->data.begin(), this->data.begin() + 4);
    return cmd;
}


bool
Packet::IsAvailableData()
{
    return (this->data.size() > 0) ? true : false;
}

std::string
Packet::getContent()
{
    int len = this->getCMDHeader();

    std::string res (this->data.begin(), this->data.begin() + len);

    this->data.erase(this->data.begin(), this->data.begin() + len);

    return res;
}

char*
Packet::getDataToArray()
{
    char arr [this->data.size()+1];
    memset(arr, 0, sizeof(arr));
    rep(i,this->data.size())
            arr[i] = this->data.at(i);
    rep(i, strlen(arr)){
        std::cout << arr[i];
    }
    std::cout << std::endl;
    //std::string s = std::string(std::begin(this->data), std::end(this->data));
    std::cout << "test: " << std::endl;
    return arr;
}

std::string
Packet::getData_stdString()
{
    std::string da(this->data.begin(), this->data.end());
    return da;
}
