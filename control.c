
#include "common.c"

void signal_handler(int sig);

int main(void) {
  add_signal_handler(SIGUSR1, signal_handler);
  signal(SIGINT, SIG_IGN);

  Message sensor_msg;
  sensor_msg.msgType = MSG_TYPE;
  Msg control_msg;
  control_msg.msgType = MSG_TYPE1;

  message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  int send_data_tx = 0;
  while(1){
    //roman: send right data (read the manual)
    sensor_msg.mdata.sequenceNr = 42;
    sensor_msg.mdata.deviceID   = 2;
    sensor_msg.mdata.delta      = 41;
    sensor_msg.mdata.statusText = "43";
    message_send(message_id, sensor_msg, MSG_LENGTH, 0);
    if (send_data_tx){
      send_data_tx = 0;
      control_msg = 
      message_send(message_id, control_msg, MSG_LENGTH1, 0);
    }else{
      send_data_tx = 1;
    }
    sleep(1);
  }

  return 0;
}

void signal_handler(int sig){
  printf("control terminates\n");
  message_release();
  exit(0);
}
