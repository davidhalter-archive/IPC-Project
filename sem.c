
#include <sys/sem.h>
#include "common.c"

key_t sem_init(const char* key_name, int size, int project_id){
  key_t key = get_key(key_name, project_id, 1);
  int flags;

  if (numOfSems > 0)
    flags = 0664 | IPC_CREAT;
  else
    flags = 0;

  int sem_id = semget(key, 1, flags); //2. argument: number of semaphors
  int val = semctl(sem_id, 0, GETVAL);
  printf("test: %i\n", val);
  if (shm_id < 0) {
       printf("shmget error\n");
       exit(1);
  }
  return key;
}

void sem_down(int sem){
  
}

void sem_up(int sem){
  
}
