#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 

void print(const char* s){
  write(1, s, strlen(s));
}

void prioritytest(void) {
  fork();
  print("process started with normal priority\n");
  set_priority(0);  
  print("Priority changed to highest\n");
  for(int i = 0; i < 4; i++) {
    if(fork() == 0){
      set_priority(0);
      print("Child process running at highest priority\n");
    }
  }
  set_priority(3);
  print("priority Set to lowest\n");
  sleep(10);
  set_priority(0);
  print("priority Set to Higest\n");  
}

int main(void){
  prioritytest();
  exit(0);
}

