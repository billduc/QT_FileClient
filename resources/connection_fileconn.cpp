#include "../header/connection.h"


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

        //this->fsend(filepatch);
        this->send_File(filepatch);
    } else {
        std::cout << "request upload fail" << std::endl;
        return false;
    }

    return true;
}

bool
Connection::share_File(std::string _sender, std::string _receiver, std::string _filepatch){
    if (this->send_Requset_Upload(_filepatch)){
        std::cout << "send file to server done";
        this->send_CMD_UPLOAD_FINISH();
        if (this->check_Respond_CMD_SAVE_FILE_FINISH()){
            std::cout << "server save file finish. this connectin can be close" << std::endl;
            return true;
        } else {
            std::cerr << "some this wrong when save file to server!!! check again" << std::endl;
            return false;
        }
    }
    else{
        std::cerr << "some this wrong when send file to server! check again" << std::endl;
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
    int         _count       = 1;
    this->_file->set_File(_filepatch);
    this->_file->open_File_To_Read();
    _size = this->_file->get_Size();

    size_t _totalChunks     =   _size / BUFFSIZE;
    size_t _sizeLastChunk   =   _size % BUFFSIZE;


    rep(i,_totalChunks){
        bzero(this->buffer, BUFFSIZE);
        this->_file->read_File_Block(this->buffer, BUFFSIZE);
        int si = SSL_write(this->_ssl, this->buffer, BUFFSIZE);
        _dataSend += si;
        std::cout << " ssl send ok " << _count  << ": " << si <<  " - " << sizeof(this->buffer) << std::endl;
        ++_count;
    }

    if (_sizeLastChunk > 0){
        bzero(this->buffer, BUFFSIZE);
        this->_file->read_File_Block(this->buffer, _sizeLastChunk);
        int si = SSL_write(this->_ssl, this->buffer, _sizeLastChunk);
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
Connection::check_Respond_CMD_SAVE_FILE_FINISH()
{
    std::cout << "Log Connection: check_Respond_CMD_SAVE_FILE_FINISH" << std::endl;
    int _cmd = this->get_CMD_HEADER();
    if (_cmd == CMD_SAVE_FILE_FINISH)
        return true;
    else
        return false;
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