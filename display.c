
#include "common.c"

void signal_handler(int sig);

int main(int argc, char *argv[]) {
  int nos = atoi(argv[1]);      // number of sensors
  add_signal_handler(SIGUSR1, signal_handler);
  signal(SIGINT, SIG_IGN);

  key_t shm_key = get_key(SHM_KEY_FILE, PROJECT_ID);
  void* shm = shm_get_memory(shm_key, SHM_SIZE);

  Message sensor_msg;
  int message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  
  while(1) {
    //HomeScreen();
    ClearScreen();
    
    SensorData * sd = (SensorData *) shm;
    int i, j;
    for(i=0; i<nos; i++) {
      
      int act_val_size = (int)sd[i].valIS+1;
      char act_val[act_val_size];
      for(j=0; j<act_val_size; j++) {
        act_val[j] = '.';
      }
      act_val[j]='\0';

      int ref_val_size = (int)sd[i].valREF+1;
      char ref_val[ref_val_size];
      for(j=0; j<ref_val_size; j++) {
        ref_val[j] = '-';
      }
      ref_val[j]='\0';

      int line_size;
      line_size = printf("Device %d @ %d: %d ",sd[i].deviceID, sd[i].sequenceNr, sd[i].status, act_val);
      printf("V act %s\n", act_val);
      
      char line_spacer[line_size+1];
      for(j=0; j<line_size; j++) {
        line_spacer[j] = ' ';
      }
      line_spacer[j] = '\0';
      printf("%sV ref %s\n\n", line_spacer, ref_val);
    }
    message_receive(message_id, &sensor_msg, MSG_LENGTH, MSG_TYPE, 0);
    printf("data: %i", sensor_msg.mdata.sequenceNr);

    usleep(500000);
  }
  return 0;
}

void signal_handler(int sig){
  printf("display terminates with sig %d\n", sig);
  exit(0);
}
