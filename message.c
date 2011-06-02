
#include <sys/msg.h>
#include <sys/ipc.h>

int message_init(const char* key_name, int project_id){
  key_t key = get_key(key_name, project_id);
  int flags;

  int message_id = msgget (key, 0777 | IPC_CREAT); 
  if (message_id < 0) {
    printf("message error\n");
    exit(1);
  }
  return message_id;
}

int message_receive(int message, void* data, size_t size, long type, int flag){
  return msgrcv(message, data, size, type, flag);
}

int message_send(int message, void* data, size_t size, int flag){
  return msgsnd(message, data, size, flag);
}

void message_release(const char* key_name, int message){
  msgctl(message, IPC_RMID, NULL);
  unlink(key_name); 
}
