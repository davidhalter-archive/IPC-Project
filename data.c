
#include "defs.h"
#include "common.c"

int main(void) {
  Msg control_msg;

  int message_id = message_init(MBOX_KEY_FILE, PROJECT_ID);
  while(1){
    message_receive(message_id, &control_msg, MSG_LENGTH1, MSG_TYPE1, 0);
    printf("data: %i", control_msg.numOfSensors);
  }
  return 0;
}
