
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "defs.h"
#include "shm.c"

// SensorData struct is 20 bytes
#define SHM_SIZE 20 * SENSOR_MAX_NUM

void add_signal_handler(int sig, void (*callback)(int)){
  struct sigaction action;
  action.sa_handler = callback;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);
  sigaction (sig, &action, NULL);
}

//returns pid
pid_t start_process(char* name, char* argv[]){
  pid_t pid;
  if ((pid = fork()) == 0) {
    execv(name, argv); 
    printf("!!! panic !!!\n");
  }else if (pid < 0) {
    printf("fork failed\n");
    end();
  }else{
    return pid;
  }
}

key_t get_key(const char* key_name, int project_id){
  int fd = open(key_name, O_RDWR | O_CREAT, 0770);
  close(fd);
  return ftok(key_name, project_id);
}
