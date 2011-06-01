
#include "common.c"
#include "socket_server.h"

void signal_handler(int sig);
void stop();

#define MAX_CHILDREN 2
pid_t children[MAX_CHILDREN] = {0,0};
void* shm; 
key_t shm_key; 
int sem = 0; //temporary, will change

int main(int argc, char *argv[]) {
  int nos = atoi(argv[1]);      //number of sensors
  
  printf("IPC Project started\n"); 
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGINT, signal_handler);

  shm_key = shm_allocate(SHM_KEY_FILE, SHM_SIZE, PROJECT_ID);
  shm = shm_get_memory(shm_key, SHM_SIZE);

  children[0] = start_process("HSControl.e");
  children[1] = start_process("HSDisplay.e");

  // socket tests
  int sfd = socket_init(); 
  while(1) {
    socket_read(sfd, shm);
    SensorData * sd = (SensorData *) shm;
    printf("id: %d ->",sd[0].deviceID);
    printf("seq: %d\n",sd[0].sequenceNr);
    printf("id: %d ->",sd[1].deviceID);
    printf("seq: %d\n",sd[1].sequenceNr);
    printf("id: %d ->",sd[2].deviceID);
    printf("seq: %d\n",sd[2].sequenceNr);
    
    printf("id: %d ->",sd[3].deviceID);
    printf("seq: %d\n",sd[3].sequenceNr);
    printf("id: %d ->",sd[4].deviceID);
    printf("seq: %d\n",sd[4].sequenceNr);
    printf("id: %d ->",sd[5].deviceID);
    printf("seq: %d\n",sd[5].sequenceNr);
    
    printf("id: %d ->",sd[6].deviceID);
    printf("seq: %d\n",sd[6].sequenceNr);
    printf("id: %d ->",sd[7].deviceID);
    printf("seq: %d\n",sd[7].sequenceNr);
    printf("id: %d ->",sd[8].deviceID);
    printf("seq: %d\n",sd[8].sequenceNr);
  }
  
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
