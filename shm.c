
//#include <stdio.h>

#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

key_t shm_allocate(const char* key_name, int size, int project_id){
  key_t key = get_key(key_name, project_id);

  int shm_id = shmget(key, size, IPC_CREAT | 0666);
  if (shm_id < 0) {
     perror("shmget error");
     exit(1);
  }
  return key;
}

int shm_get(key_t key, int size){
  return shmget(key, size, IPC_CREAT | 0666);
}

void* shm_get_memory(key_t key, int size){
  int shm_id = shm_get(key, size);
  void* shm_addr;
  shm_addr = shmat(shm_id, NULL, 0);
  if (shm_addr == (void *)-1) {
    printf("shm_get failed\n");
    shm_release(shm_id);
    shm_addr = NULL;
  }
  return shm_addr;
}

int shm_release(const char* key_name, key_t key, int size){
  int shm_id = shm_get(key, size);

  if (shmctl(shm_id, IPC_RMID, NULL) == -1){
    printf("shmctl failed");
    return -1;
  }
  unlink(key_name); // delete key file
  return 0;
}
