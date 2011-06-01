#ifndef SOCKET_SERVER_HEADER
#define SOCKET_SERVER_HEADER

int socket_init();
void socket_read(int sfd, void* shm);
void socket_close(int sfd);

#endif