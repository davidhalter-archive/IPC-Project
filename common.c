
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "defs.h"
#include "shm.c"

#define SHM_SIZE 32

void add_signal_handler(int sig, void (*callback)(int)){
  struct sigaction action;
  action.sa_handler = callback;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);
  sigaction (sig, &action, NULL);
}

//returns pid
pid_t start_process(char* name){
  pid_t pid;
  if ((pid = fork()) == 0) {
    execl(name, name, NULL); 
    printf("!!! panic !!!\n");
  }else if (pid < 0) {
    printf("fork failed\n");
    end();
  }else{
    return pid;
  }
}

key_t get_key(const char* key_name, int project_id, int new){
  if (new == 1){
    int fd = open(key_name, O_RDWR | O_CREAT, 0770);
    close(fd);
  }
  return ftok(key_name, project_id);
}
