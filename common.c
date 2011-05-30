
#include <stdlib.h>
#include <signal.h>

#include "defs.h"

void add_signal_handler(int sig, void (*callback)(int)){
  struct sigaction action;
  action.sa_handler = callback;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);
  sigaction (SIGINT, &action, NULL);
}

//returns pid
pid_t start_process(char* name){
  pid_t pid;
  if ((pid = fork()) == 0) {
    execl(name, NULL); 
    printf("!!! panic !!!\n");
  }else if (pid < 0) {
    printf("fork failed\n");
    end();
  }else{
    return pid;
  }
}

