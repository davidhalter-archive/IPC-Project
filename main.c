
#include "common.c"
#include "socket_server.h"

void signal_handler(int sig);
void stop();

#define MAX_CHILDREN 2
pid_t children[MAX_CHILDREN] = {0,0};
void* shm; 
key_t shm_key; 
int sem = 0; //temporary, will change
int message_id = 0;

int main(int argc, char *argv[]) {
  int nos = atoi(argv[1]);      //number of sensors
  int offset;
  
  printf("IPC Project started\n"); 
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGINT, signal_handler);

  shm_key = shm_allocate(SHM_KEY_FILE, SHM_SIZE, PROJECT_ID);
  shm = shm_get_memory(shm_key, SHM_SIZE);

  sem = sem_init(SEM_KEY_FILE, PROJECT_ID, 1); //semaphor new -> 1

  children[0] = start_process("HSControl.e", NULL);
  children[1] = start_process("HSDisplay.e", argv);

  // socket tests
  int sfd = socket_init();
  SensorData sd[nos];
  unsigned old_sequence_nr = 0;
  SensorData sd_buf[3*SENSOR_MAX_NUM];   //buffer for sensor data 
  while(1) {
    socket_read(sfd, sd);  // write sensor data where sd points
    offset = nos*(sd->sequenceNr%3) + (sd->deviceID); // offset
    sd_buf[offset] = *sd;     // buffer sensor data
    
    /*
    printf("offset: %d\n", offset);
    printf("seq: %d -> id: %d\n",sd_buf[0].sequenceNr, sd_buf[0].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[1].sequenceNr, sd_buf[1].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[2].sequenceNr, sd_buf[2].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[3].sequenceNr, sd_buf[3].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[4].sequenceNr, sd_buf[4].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[5].sequenceNr, sd_buf[5].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[6].sequenceNr, sd_buf[6].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[7].sequenceNr, sd_buf[7].deviceID);
    printf("seq: %d -> id: %d\n",sd_buf[8].sequenceNr, sd_buf[8].deviceID);
    printf("current seq: %d -> id: %d\n",sd->sequenceNr, sd->deviceID);
    */
    
    if(sd->sequenceNr > old_sequence_nr) {
      int i,j;
      for(i=0;i<3*nos;i=i+nos) {
        if((sd->sequenceNr == sd_buf[i].sequenceNr+2)) {
          for(j=0;j<nos;j++) {
            ((SensorData*) shm)[j]= sd_buf[i+j];
            //printf(">>>>>>>>>>>>>>>>>>>>> write id %d\n\n", i+j);
          }
        }
      }
    }
    
    old_sequence_nr = sd->sequenceNr;
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
  sem_release(SEM_KEY_FILE, sem);
  exit(0);
}
