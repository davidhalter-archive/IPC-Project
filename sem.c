
#include <sys/sem.h>

int sem_init(const char* key_name, int project_id){
  key_t key = get_key(key_name, project_id);
  int flags;

  if (1  > 0)
    flags = 0664 | IPC_CREAT;
  else
    flags = 0;

  int sem_id = semget(key, 1, flags); //2. argument: number of semaphors
  
  //sem_union.val = 0;
  semctl(sem_id, 0, SETVAL, 1); 
  int val = semctl(sem_id, 0, GETVAL);
  printf("test: %i\n", val);
  if (sem_id < 0) {
    printf("shm error\n");
    //exit(1);
  }
  return sem_id;
}

void sem_down(int sem){
  struct sembuf buf;
  buf.sem_num = 1;
  buf.sem_op = -1;
  buf.sem_flg = 0;
  semop(sem, &buf, 1); // do it
}

void sem_up(int sem){
  struct sembuf buf;
  buf.sem_num = 1;
  buf.sem_op  = 1;
  buf.sem_flg = 0;
  semop(sem, &buf, 1); // do it
}
