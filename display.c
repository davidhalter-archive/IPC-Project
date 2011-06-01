
#include "common.c"

void signal_handler(int sig);

int main(void) {
  add_signal_handler(SIGUSR1, signal_handler);
  signal(SIGINT, SIG_IGN);

  key_t shm_key = get_key(SHM_KEY_FILE, PROJECT_ID);
  void* shm = shm_get_memory(shm_key, SHM_SIZE);

  while(1) {
    SensorData * sd = (SensorData *) shm;
    printf("id: %d -> seq: %d -> is: %f -> ref: %f\n",sd[0].deviceID, sd[0].sequenceNr, sd[0].valIS, sd[0].valREF);

    usleep(500000);
  }
  return 0;
}

void signal_handler(int sig){
  printf("display terminates with sig %d\n", sig);
  exit(0);
}
