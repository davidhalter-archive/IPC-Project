
#include "common.c"
#include "sem.c"

void signal_handler(int sig);

char sig_alrm = 0;
char rcv_sig_alrm = 0;

int main(int argc, char *argv[]) {
  int nos = atoi(argv[1]);      // number of sensors
  int sem;
  
  int msg_device_buf, msg_sequence_buf;
  char msg_status_buf[4];
  float msg_delta_buf;
  
  add_signal_handler(SIGUSR1, signal_handler);
  add_signal_handler(SIGINT,  signal_handler);
  add_signal_handler(SIGTERM, signal_handler);
  add_signal_handler(SIGALRM, signal_handler);
  //signal(SIGINT, SIG_IGN);

  key_t shm_key = get_key(SHM_KEY_FILE, PROJECT_ID);
  void* shm = shm_get_memory(shm_key, SHM_SIZE);

  Message sensor_msg;
  int message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  
  sem = sem_init(SEM_KEY_FILE, PROJECT_ID, 0);
  while(1) {
    //HomeScreen();
    ClearScreen();
    
    // alrm signal
    if(rcv_sig_alrm > 0) {
      sig_alrm = 1;
      rcv_sig_alrm--;
    }else{
      sig_alrm = 0;
    }
    
    //print header
    printf("#####################################################################################\n");
    printf(COMPANY);
    printf("Data display read | Number of sensors: %d | ", nos);
    if(sig_alrm == 1) {
      printf("System health: Alarm!\n");
    } else {
      printf("System health: Ok\n");
    }
    printf("#####################################################################################\n\n");
    
    // print data
    sem_down(sem);
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
    sem_up(sem);
    
    
    if (message_receive(message_id, &sensor_msg, MSG_LENGTH, MSG_TYPE, IPC_NOWAIT) >= 0){
      msg_sequence_buf = sensor_msg.mdata.sequenceNr;
      msg_device_buf = sensor_msg.mdata.deviceID;
      msg_delta_buf = sensor_msg.mdata.delta;
      strcpy(msg_status_buf, sensor_msg.mdata.statusText);
    }
    
    printf("Control messages:\n");
    printf("Sequence number: %i\n", msg_sequence_buf);
    printf("Device ID:       %i\n", msg_device_buf);
    printf("Max Delta:       %f\n", msg_delta_buf);
    printf("Status:          %s\n", msg_status_buf);

    usleep(500000);
  }
  return 0;
}

void signal_handler(int sig){
  if (sig == SIGALRM){
    if(rcv_sig_alrm == 0){
      rcv_sig_alrm = 4; // 2 seconds 
      sig_alrm = 1;
    }   
  }else{
    sleep(1);
    printf("display terminates with sig %d\n", sig);
    exit(0);
  }
}
