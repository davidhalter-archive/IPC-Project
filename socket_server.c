
#include "defs.h"
#include "socket_server.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

int socket_init() {
  int sys_ret, i, sfd;
  char str_port[8];
  struct addrinfo hints, *ai_list, *ai_ptr;
  
  // setup
  memset(&hints, '\0', sizeof(hints));
  hints.ai_family   = AF_UNSPEC;        // allow ipv4 and ipv6
  hints.ai_socktype = SOCK_STREAM;      // stream socket
  sprintf(str_port, "%d", COMM_PORT);
  
  sys_ret = getaddrinfo(NULL, str_port, &hints, &ai_list);
  if(sys_ret != 0) {
    printf("error getting network address %s\n", gai_strerror(sys_ret));
    return (-1);
  }
  
  // search through list, bind socket
  ai_ptr = ai_list;
  while (ai_ptr != 0) {
    sfd = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
    if (sfd >= 0) {
      i = 1;
      sys_ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
      if (sys_ret < 0)
        perror("cannot set socket options");

      if (bind(sfd, ai_ptr->ai_addr, ai_ptr->ai_addrlen) < 0) {
        perror("bind failed ");
        close(sfd);
        exit(-1);
      }
      printf("Binding successful\n");
      printf("Connected to port #%s\n", str_port);
          
      if (listen(sfd, 5) < 0) {
        close(sfd);
        perror("listen failed ");
        exit(-1);
      }
      else          
        break;
    }

    ai_ptr = ai_ptr->ai_next;
  }
  freeaddrinfo(ai_list); // not longer used, free
  
  if (ai_ptr == NULL) {
    printf("could not set up a socket server\n");
    exit(-1);
  }

  return sfd;     // init successful, return server file descriptor
}

void socket_read(int sfd, SensorData * sd) {
  int addrlen;
  char buf[BUF_SIZE];
  struct sockaddr addr;
  int cfd;

  if((cfd = accept(sfd, &addr,(unsigned *) &addrlen)) >= 0)  {
    while((read(cfd, buf, BUF_SIZE)) > 0) {
      *sd = *((SensorData *) buf);      // cast char to SensorData struct
    }
    close(cfd);
  }
}

void socket_close(int sfd) {
  close(sfd);
}