
#include "common.c"

void signal_handler(int sig);
void stop();

#define MAX_CHILDREN 2
pid_t children[MAX_CHILDREN] = {0,0};

int main(void) {
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGINT, signal_handler);

  children[0] = start_process("HSControl.e");
  children[1] = start_process("HSDisplay.e");

  sleep(60);
  stop();
  return 0;
}

void signal_handler(int sig){
  printf("main stop signal\n");
  stop();
}

void stop(){
  int i;
  for(i=1;i<MAX_CHILDREN;i++){
    if (i != 0){
      waitpid(children[i]);
    }
  }
  exit(0);
}
