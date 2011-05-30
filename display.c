
#include "common.c"

void signal_handler(int sig);

int main(void) {
  add_signal_handler(SIGUSR1, signal_handler);

  signal(SIGCHLD, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  sleep(2000);
  return 0;
}

void signal_handler(int sig){
  printf("display terminates\n");
  //exit(0);
}
