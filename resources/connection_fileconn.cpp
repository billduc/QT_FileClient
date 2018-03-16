#include "../header/connection.h"
#include "../header/wrap.h"

bool
Connection::send_Requset_Upload(std::string filepatch)
{
    Packet*     pk;
    int         bytes;
    int         cmd;
    int         rc;
    std::string filename        = this->_file->get_File_Name(filepatch);

    this->_isFileConnection     = true;
    this->_isMainConnection     = false;
    this->handle_Classify_Connection();
    std::cout << "#log Connecion: before set file patch "<< filepatch << "inner class: " << this->_file->get_File_Patch() << std::endl;
    this->_file->format_File_Patch(filepatch);
    this->_file->set_File(filepatch);
    std::cout << "#log Connecion: before set file patch "<< filepatch << "inner class: " << this->_file->get_File_Patch() << std::endl;
    //build and send login request CMD_UPLOAD_FILE
    pk = new Packet();
    pk->appendData(CMD_UPLOAD_FILE);
    pk->appendData(this->_session);
    pk->appendData(filename);
    pk->appendData(this->_file->get_Size_stdString());

    SSL_write(this->_ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;

    std::cout << "send CMD request upload finished" << std::endl;

    bzero(this->buffer, sizeof(this->buffer));
    struct timeval time = this->_timeout;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->_socketFd, &fdset);

    rc = select(this->_socketFd+1, &fdset, NULL, NULL, &time);

    std::cerr << "log before select " << SSL_get_fd(this->_ssl) << " " << rc << std::endl;

    if (rc == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bytes   = SSL_read(this->_ssl, this->buffer, sizeof(this->buffer));
    pk      = new Packet(std::string(buffer,bytes));
    if (pk->IsAvailableData())
        cmd = pk->getCMDHeader();

    std::cout << "read CMD reponse upload finished " << bytes << std::endl;

    std::cout << "cmd respond: " << cmd << std::endl;

    if (cmd == CMD_UPLOAD_READY) {
        std::cout << "ready to upload file" << std::endl;
        if (pk->IsAvailableData()){
            this->_urlFileServer = pk->getContent();
            std::cout << "file url: " << this->_urlFileServer << std::endl;
        }

        this->send_File(filepatch);
    } else {
        std::cout << "request upload fail" << std::endl;
        return false;
    }

    return true;
}

bool
Connection::send_Requset_Download(std::string _fileURL, long long _fileSize)
{
    Packet*     _pk;
    int         _bytes;
    int         _cmd;
    int         _rc;

    this->_isFileConnection     = true;
    this->_isMainConnection     = false;
    this->handle_Classify_Connection();

    _pk = new Packet();
    _pk->appendData(CMD_DOWNLOAD_FILE);
    _pk->appendData(this->_session);
    _pk->appendData(_fileURL);

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());

    delete _pk;

    _cmd = this->get_CMD_HEADER();

    if (_cmd == CMD_DOWNLOAD_READY_SEND){

        this->_file->begin_Write_File(_fileURL);
        //this->send_CMD_HEADER(CMD_DOWNLOAD_READY_SAVE);
        std::cout <<"begin save data form server "<<std::endl;
        this->write_Data(_fileURL, _fileSize);
        std::cout <<"end save data form server "<<std::endl;
    }
}

void
Connection::write_Data(std::string _fileURL, long long _fileSize)
{
    char            _buffer[BUFFSIZE];
    int             _bytes;
    std::string     _data;
    long long       _totalData, _recievedData;

    _totalData          = _fileSize;
    _recievedData       = 0;

    while(1){
        std::cout << "#log conn: Write file " << _recievedData << " " << _totalData<< std::endl;
        if (_totalData == _recievedData){
            this->_dataWriteDoneState = true;
            return;
        } else {
            if (_recievedData + sizeof(_buffer) <= _totalData)
            {
                //_bytes   = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
                _bytes   = SF_SSL_READ(this->_socketFd,  this->_ssl, _buffer, sizeof(_buffer));

                if (_bytes > 0){
                    _data = std::string(_buffer, _bytes);
                    std::cout << "#log conn: Write block" << std::endl;
                    // Previous (upload) command continuation, store incoming data to the file
                    std::cout << "#log conn: Part " << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                    this->_file->write_File_Block(_data);
                    _recievedData += _bytes;
                } else {
                    //this->_closureRequested = true;
                    std::cerr << "#log conn: 1 read zero data" << std::endl;
                }
            } else {
                if ((_totalData - _recievedData < sizeof(_buffer)) && (_totalData > _recievedData))
                    {
                        //_bytes   = SSL_read(this->_ssl, _buffer, (_totalData - _recievedData));
                        _bytes   = SF_SSL_READ(this->_socketFd, this->_ssl, _buffer, (_totalData - _recievedData));
                        _recievedData += _bytes;
                        if (_bytes > 0){
                            _data = std::string(_buffer, _bytes);
                            std::cout << "#log conn: Write block" << std::endl;
                            // Previous (upload) command continuation, store incoming data to the file
                            std::cout << "#log conn: Part" << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                            this->_file->write_File_Block(_data);
                        } else {
                            //this->_closureRequested = true;
                            std::cerr << "#log conn: 2 read zero data" << std::endl;
                        }
                    }
            }
        }
    }
}


bool
Connection::share_File(std::string _sender, std::string _receiver, std::string _filepatch){
    if (this->send_Requset_Upload(_filepatch)){
        std::cout << "send file to server done";
        this->send_CMD_UPLOAD_FINISH();
        if (this->check_Respond_CMD_SAVE_FILE_FINISH()){
            std::cout << "server save file finish. this connectin can be close"     << std::endl;
            this->_sender                   = _sender;
            this->_receiver                 = _receiver;
            this->_dataSizeSend_stdString   = this->_file->get_Size_stdString();
            this->_dataSizeSend_int         = this->_file->get_Size();
            return true;
        } else {
            std::cerr << "some this wrong when save file to server!!! check again"  << std::endl;
            return false;
        }
    }
    else{
        std::cerr << "some this wrong when send file to server! check again"        << std::endl;
        return false;
    }
}

bool
Connection::receive_File(std::string _fileURL, long long _fileSize)
{
    if (this->send_Requset_Download(_fileURL, _fileSize)){
        std::cout << "recieved file from server" << std::endl;;
        if (this->check_Respond_CMD_DOWNLOAD_FINISH()){
            std::cout << "server save file finish. this connectin can be close"     << std::endl;
            return true;
        } else {
            std::cerr << "some this wrong when save file to server!!! check again"  << std::endl;
            return false;
        }
    }
    else{
        std::cerr << "some this wrong when send file to server! check again"        << std::endl;
        return false;
    }
}

/*
 * send file to server
 * @_filepatch patch of file to send to server. (fomated according operation)
 */
bool
Connection::send_File(std::string _filepatch){
    long long   _size;
    long long   _dataSend   = 0;
    int         _count      = 1;
    char        _buffer[BUFFSIZE];

    this->_file->set_File(_filepatch);
    this->_file->open_File_To_Read();
    _size = this->_file->get_Size();

    size_t _totalChunks     =   _size / BUFFSIZE;
    size_t _sizeLastChunk   =   _size % BUFFSIZE;


    rep(i,_totalChunks){
        bzero(_buffer, BUFFSIZE);
        this->_file->read_File_Block(_buffer, BUFFSIZE);
        //int si = SSL_write(this->_ssl, this->buffer, BUFFSIZE);
        int si = SF_SSL_WRITE(this->_socketFd, this->_ssl, _buffer, BUFFSIZE);
        _dataSend += si;
        std::cout << " ssl send ok " << _count  << ": " << si <<  " - " << sizeof(_buffer) << std::endl;
        ++_count;
    }

    if (_sizeLastChunk > 0){
        bzero(_buffer, BUFFSIZE);
        this->_file->read_File_Block(_buffer, _sizeLastChunk);
        //int si = SSL_write(this->_ssl, this->buffer, _sizeLastChunk);
        int si = SF_SSL_WRITE(this->_socketFd, this->_ssl, _buffer, _sizeLastChunk);
        _dataSend += si;
        std::cout << " ssl send ok " << _count  << ": " << si <<  " - " << _sizeLastChunk << std::endl;
        ++_count;
    }

    std::cout << "data sended: " << _dataSend  << " of  Datasize: " << _size << std::endl;
    this->_file->close_Read_Stream();

    return true;
}

bool
Connection::send_CMD_UPLOAD_FINISH()
{
    //send CMD_UPLOAD_FINISH
    Packet*     _pk;
    _pk = new Packet();

    _pk->appendData(CMD_UPLOAD_FINISH);

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
    return true;
}

bool
Connection::check_Respond_CMD_DOWNLOAD_FINISH()
{
    std::cout << "Log Connection: check_Respond_CMD_SAVE_FILE_FINISH" << std::endl;
    int _cmd = this->get_CMD_HEADER();
    if (_cmd == CMD_DOWNLOAD_FINISH)
        return true;
    else
        return false;
}

bool
Connection::check_Respond_CMD_SAVE_FILE_FINISH()
{
    std::cout << "Log Connection: check_Respond_CMD_SAVE_FILE_FINISH" << std::endl;
    int _cmd = this->get_CMD_HEADER();
    if (_cmd == CMD_SAVE_FILE_FINISH)
        return true;
    else
        return false;
}

std::string
Connection::get_Data_Size_Send_stdString()
{
    return this->_dataSizeSend_stdString;
}

long long
Connection::get_Data_Size_Send_Int()
{
    return this->_dataSizeSend_int;
}


std::string
Connection::get_Sender()
{
    return this->_sender;
}

std::string
Connection::get_Receiver()
{
    return this->_receiver;
}


bool
Connection::get_Status_Send_File_Finished()
{
    return this->_statusSendFileFinished;
}


void
Connection::set_Status_Send_File_Finished(bool _state)
{
    this->_statusSendFileFinished = _state;
}
