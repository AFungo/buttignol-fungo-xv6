#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 

void print(const char* s){
  write(1, s, strlen(s));
}

void prioritytest(void){
    if(fork()==0){
      //set_priority(3);
      //sleep(4);
      for(;;){
				set_priority(3);
				print("este mensaje lo deberias ver una vez\n");
				//sleep(4);
			}       
    }
		for(;;){
    	//if(fork()==0)break;
      //intf("\n %d soy el pid",getpid());
    	set_priority(0);
      //print("\n soy un pid con prioridad 0\n");
		}
}

int main(void){
    prioritytest();
    exit(0);
}
