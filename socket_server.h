#ifndef SOCKET_SERVER_HEADER
#define SOCKET_SERVER_HEADER

int socket_init();
void socket_read(int sfd, void * shm, int nos, int sem);
void socket_close(int sfd);

#endif