
#include "common.c"

int message_id = 0; 
void signal_handler(int sig);
int pid_display = 0;

int main(int argc, char *argv[]) {
  add_signal_handler(SIGUSR1, signal_handler);
  add_signal_handler(SIGINT,  signal_handler);
  add_signal_handler(SIGTERM, signal_handler);
  //signal(SIGINT, SIG_IGN);

  int nos = atoi(argv[1]);      //number of sensors
  pid_t display_pid = atoi(argv[2]);
  
  Message sensor_msg;
  sensor_msg.msgType = MSG_TYPE;
  Msg control_msg;
  control_msg.msgType = MSG_TYPE1;

  key_t shm_key = get_key(SHM_KEY_FILE, PROJECT_ID);
  void* shm = shm_get_memory(shm_key, SHM_SIZE);
  SensorData * sd = (SensorData *) shm;
  
  int message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  int send_data_tx = 0;
  while(1){
    int i, device_id, seq;
    float delta[nos], max_delta = 0, sum = 0;
    for(i=0; i<nos; i++) {
      if(sd[i].status > (-1)) {
        delta[i] = sd[i].valREF - sd[i].valIS;
        sum = sum + delta[i];
        
        if(abs(delta[i]) > abs(max_delta)) {
          max_delta = delta[i];
          device_id = sd[i].deviceID;
          seq = sd[i].sequenceNr;
        }
      } else {
        kill(display_pid, SIGALRM);
        delta[i] = 0;
      }
    }
    
    if(sum < (-5)) {
      strcpy(sensor_msg.mdata.statusText, "+++");
    } else if(sum > 5) {
      strcpy(sensor_msg.mdata.statusText, "---");
    } else {
      strcpy(sensor_msg.mdata.statusText, "+/-");
    }
    sensor_msg.mdata.sequenceNr = seq;
    sensor_msg.mdata.deviceID   = device_id;
    sensor_msg.mdata.delta      = max_delta;
    message_send(message_id, &sensor_msg, MSG_LENGTH, 0);
    
    if (send_data_tx){
      send_data_tx = 0;
      control_msg.numOfSensors = nos;
      for(i=0; i<nos; i++) {
        control_msg.ctrl[i] = 0.3 * delta[i];
      }
      message_send(message_id, &control_msg, MSG_LENGTH1, 0);
    }else{
      send_data_tx = 1;
    }
    sleep(1);
  }

  return 0;
}

void signal_handler(int sig){
  printf("control terminates\n");
  kill(pid_display, SIGINT);
  exit(0);
}
