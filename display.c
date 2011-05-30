
#include "common.c"

void signal_handler(int sig);

int main(void) {
  add_signal_handler(SIGUSR1, signal_handler);
  signal(SIGINT, SIG_IGN);

  key_t shm_key = shm_get_key(SHM_KEY_FILE, PROJECT_ID, 0);
  void* shm = shm_get_memory(shm_key, SHM_SIZE);

  printf("display: %d\n", *(int*)shm);
  sleep(2000);
  return 0;
}

void signal_handler(int sig){
  printf("display terminates with sig %d\n", sig);
  exit(0);
}
