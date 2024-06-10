#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//still not added to compile
int
main(int argc, char *argv[])
{
  int cd=channel_create();
  if(cd<0){
    printf("Failed to create channel\n");
    exit(1);
  }
  if(fork()==0){ //child process
    if(channel_put(cd,42)<0){
        printf("Failed to put data n channel\n");
        exit(1);
    }
    channel_put(cd,43); //supposed to sleep until cleared
    //handle error (no idea why they put this so far)
    channel_destroy(cd);
    //handle error (no idea why they put this so far)
  }
  else{ //parent process
  int data;
  if(channel_take(cd,&data)<0){//42
    printf("Failed to take data from channel\n");
    exit(1);
  }
  data=channel_take(cd,&data); //43
  //handle error(what do they mean by this?)

  data=chaannel_take(cd,&data); //uppoed to sleep until child destroys channel(ie exits?)
  }
  exit(0);
}
