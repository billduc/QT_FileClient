#ifndef WRAP_H
#define WRAP_H

#include "filesys.h".h"

void    Listen(int fd, int backlog);


void	 err_dump(const char *, ...);
void	 err_msg(const char *, ...);
void	 err_quit(const char *, ...);
void	 err_ret(const char *, ...);
void	 err_sys(const char *, ...);

int      SF_SSL_READ(int, SSL*, char*, int );
int      SF_SSL_WRITE(int, SSL*, char*, int );

#endif // WRAP_H
