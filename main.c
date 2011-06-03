
#include "common.c"
#include "socket_server.h"
#include "sem.c"
#include <sys/socket.h>
#include <sys/wait.h>

void signal_handler(int sig);
void stop();

#define MAX_CHILDREN 2
pid_t children[MAX_CHILDREN] = {0,0};
void* shm; 
key_t shm_key; 
int message_id = 0;
int sem, sfd;
int terminator_is_coming = 0;

char** main_argv;

int main(int argc, char *argv[]) {
  main_argv = argv;
  int nos = atoi(argv[1]);      //number of sensors
  int offset;
  
  printf("IPC Project started\n"); 
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGINT, signal_handler);
  add_signal_handler(SIGCHLD, signal_handler);

  shm_key = shm_allocate(SHM_KEY_FILE, SHM_SIZE, PROJECT_ID);
  shm = shm_get_memory(shm_key, SHM_SIZE);

  sem = sem_init(SEM_KEY_FILE, PROJECT_ID, 1); //semaphor new -> 1

  message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
 
  children[0] = init_display(argv);
  children[1] = init_control(children[0], argv);

  // socket tests
  sfd = socket_init();
  if(sfd < 0)
    stop();
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
      int i,j,seq;
      char write = 1;
      for(i=0;i<3*nos;i=i+nos) {
        if((sd->sequenceNr == sd_buf[i].sequenceNr+2)) {
          
          // check for same sequence nr
          seq = sd_buf[i].sequenceNr;
          for(j=1;j<nos;j++) {
            if(sd_buf[i+j].sequenceNr != seq) {
              write = 0;        // do not write to shm
              break;
            }
            seq = sd_buf[i+j].sequenceNr;
          }
          
          if(write == 1) {
            sem_down(sem);
            for(j=0;j<nos;j++) {
              ((SensorData*) shm)[j]= sd_buf[i+j];
              //printf(">>>>>>>>>>>>>>>>>>>>> write id %d\n\n", i+j);
            }
            sem_up(sem);
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
  if (sig == SIGCHLD){
    if (terminator_is_coming == 0){
      int status;
      usleep(1100000); //make shure all processes are terminated
      int test = waitpid(children[0], &status, WNOHANG);
      //printf("status %i, %i\n", status, test);
      if (test < 0){
        children[0] = init_display(main_argv);
      }
      test = waitpid(children[1], &status, WNOHANG);
      //printf("status %i, %i\n", status, test);
      if (test < 0){
        children[1] = init_control(children[0], main_argv);
      }
      fflush(stdout);
    }
  }else{
    terminator_is_coming = 1;
    printf("main stop signal %i:\n", sig);
    stop();
  }
}

void stop(){
  fflush(stdout);
  int i;
  for(i=0;i<MAX_CHILDREN;i++){
    if (children[i] != 0){
      kill (children[i], SIGUSR1);
      int status;
      waitpid(children[i], &status, 0);
    }
  }
  shm_release(SHM_KEY_FILE, shm_key, SHM_SIZE);
  sem_release(SEM_KEY_FILE, sem);
  socket_close(sfd);  
  message_release(MBOX_KEY_FILE, message_id);
  exit(0);
}

pid_t init_display(char *argv[]){
  return start_process("HSDisplay.e", argv);
}

pid_t init_control(pid_t pid_display, char *argv[]){
  char* argv_control[4];
  char argv_buf[20];
  sprintf(argv_buf, "%d", pid_display);
  argv_control[0] = argv[0];
  argv_control[1] = argv[1];
  argv_control[2] = argv_buf;
  argv_control[3] = NULL;
  return start_process("HSControl.e", argv_control);
}
