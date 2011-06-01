#ifndef SOCKET_SERVER_HEADER
#define SOCKET_SERVER_HEADER

int socket_init();
void socket_read(int sfd, SensorData* sd);
void socket_close(int sfd);

#endif