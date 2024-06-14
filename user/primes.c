#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int isPrime(int num){ //naive checking approach
  for(int i=2;i<num;i++){
    if(((i!=num) &&(num%i==0))||(num==0))
      return 0;
  }
  return 1;
}
int reboot=1;
int
main(int argc, char *argv[])
{
  while(reboot){
    int counter=0; //counts number of found primes
    int checkers=3;
    if(argc>1){
      checkers=atoi(argv[1]);
    }
    int channel_one=channel_create();
    // CREATING CHANNELS IN PARENT PROC
    if(channel_one<=0){
        exit(-1);
      } 
    int channel_two=channel_create();
    if(channel_two<=0){
        exit(-1);
      } 
    if(fork()==0){ //generator
      int generated_num=2;
      while (counter<100)
      {
        
        if(channel_put(channel_one,generated_num)<0){
          printf("Shutting off generator pid %d\n",getpid());
            exit(-1);
        }
        generated_num=generated_num+1;
      }
      exit(0);
    }
    else{ //parent process
      for(int i=0;i<checkers;i++){
        if(fork()==0){
          while(counter<100){
            int number_to_check;
            if(channel_take(channel_one,&number_to_check)<0){
              exit(-1);          
            }
            if(isPrime(number_to_check)){
              if(channel_put(channel_two,number_to_check)<0){
                channel_destroy(channel_one);
                printf("Shutting off checker pid %d\n",getpid());
                exit(-1); 
              }
            }
        }
        }

      }
      if(fork()==0){ //printer child
        while(counter<100){
          int prime;
          if(channel_take(channel_two,&prime)==0){
              printf("Prime:%d Count: %d\n",prime,counter+1);
              counter=counter+1;
              }
          else{
            exit(-1);
          }
        }
          channel_destroy(channel_two);
          printf("Shutting off printer pid %d\n",getpid());
          exit(0);
      }
    }
    //wait for other processes to end
  for(int p=0;p<1+checkers;p++){
    wait(0);
      }
  char user_reboot[100];
  printf("Type 1 to reboot and 0 to exit:\n");
  reboot=atoi(gets(user_reboot,100));
    }
  exit(0);
}



