#ifndef FILESYS_H
#define FILESYS_H

#include <bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#define CMD_AUTHEN_LOGIN        1
#define CMD_AUTHEN_SUCCESS      2

#define CMD_ADD_CONTACT         3
#define CMD_ADD_SUCCESS         4

#define CMD_UPLOAD_FILE         5
#define CMD_UPLOAD_READY        6
#define CMD_MSG_FILE            7
#define CMD_DOWNLOAD_FILE       7
#define CMD_DOWNLOAD_FINISH     8

#define CMD_SHARE_FILE          10
#define CMD_SHARE_WAITING       11
#define CMD_RECEIVE_FILE        12
#define CMD_GET_FILE            13
#define CMD_SHARE_CONFIRM       14
#define CMF_SHARE_FINISH        15


#define rep(i,n) 				for(int i = 0; i < (int) n; ++i)
#define For(i,a,b) 				for(int i =(int) a; i <=(int) b; ++i)
#define PACKET 					std::vector<unsigned char>
#define PS 						push_back

#endif // FILESYS_H
