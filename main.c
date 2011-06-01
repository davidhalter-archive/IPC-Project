
#include "common.c"
#include "socket_server.h"

void signal_handler(int sig);
void stop();

#define MAX_CHILDREN 2
pid_t children[MAX_CHILDREN] = {0,0};
void* shm; 
key_t shm_key; 
int sem = 0; //temporary, will change

int main(void) {
  printf("IPC Project started\n"); 
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGINT, signal_handler);

  shm_key = shm_allocate(SHM_KEY_FILE, SHM_SIZE, PROJECT_ID);
  shm = shm_get_memory(shm_key, SHM_SIZE);

  *((int*) shm) = 42;
  
  // socket tests
  int sfd = socket_init();
  socket_read(sfd);

  children[0] = start_process("HSControl.e");
  children[1] = start_process("HSDisplay.e");

  //sleep(60);
  stop();
  return 0;
}

void signal_handler(int sig){
  printf("main stop signal\n");
  stop();
}

void stop(){
  fflush(stdout);
  int i;
  for(i=0;i<MAX_CHILDREN;i++){
    if (children[i] != 0){
      kill (children[i], SIGUSR1);
      waitpid(children[i]);
    }
  }
  shm_release(SHM_KEY_FILE, shm_key, SHM_SIZE);
  exit(0);
}
