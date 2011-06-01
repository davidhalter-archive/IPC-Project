
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "defs.h"
#include "shm.c"

// SensorData struct is 20 bytes
// shm stores max. 3 strucs for each sensor
#define SHM_SIZE 3 * 20 * SENSOR_MAX_NUM

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

