#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int cd=channel_create();
  if(cd<0){
    printf("failed to create channel\n");
    exit(-1);
  }
  else{
    if(fork()==0){//child
      int x=channel_put(cd,50);
      if(x<0){
        printf("child couldnt put 50 into channel\n");
        exit(-1);
      }
      else{
        printf("child put 50 into channel\n");
      }
      int data;
      int z=channel_take(cd,&data);
      if(z<0){
        printf("child couldnt take value\n");
        exit(-1);
      }
      else{
        printf("child took value and its %d\n",data);
      }
    }
    else{ //parent
      int y=channel_put(cd,52);
      if(y<0){
        printf("parent couldnt put 52 into channel\n");
      }
      else{
        printf("parent put 52 into channel\n");
      }  
      int dataa;
      int k=channel_take(cd,&dataa);
      if(k<0){
        printf("parent couldnt take value\n");
        exit(-1);
      }
      else{
        printf("parent took value and its %d\n",dataa);
      }     
    }
  }
  exit(0);  
  }