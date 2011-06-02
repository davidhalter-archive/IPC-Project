
#include "defs.h"
#include "common.c"

int main(void) {
  Msg control_msg;
  int i;

  int message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  while(1){
    if (message_receive(message_id, &control_msg, MSG_LENGTH1, MSG_TYPE1, 0) >= 0){
      printf("Received Message %li:\nNumber of Sensors - %i\n", control_msg.msgType, control_msg.numOfSensors);
      for(i=0;i<control_msg.numOfSensors;i++){
        printf("Sensor %i - %f\n", i, control_msg.ctrl[i]);
      }
      printf("\n");
    }
    fflush(stdout);
  }
  return 0;
}
