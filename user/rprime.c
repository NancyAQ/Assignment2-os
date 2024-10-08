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
  int gen_cd=channel_create();
  channel_put(gen_cd,2);
  while(reboot){
    int generate_start;
    if(channel_take(gen_cd,&generate_start)<0){
        printf("couldnt take generated starting number'\n");
        exit(-1);
    }
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
      int generated_num=generate_start;
      int gen=1;
      while (gen)
      {
        
        if(channel_put(channel_one,generated_num)<0){
          gen=0;
        }
        generated_num=generated_num+1;
      }
      printf("GENERATOR %d OFF\n",getpid());
      channel_put(gen_cd,generated_num);
      exit(0);
    }
    else{ //parent process
      for(int i=0;i<checkers;i++){
        if(fork()==0){
          int on=1;
          while(on){
            int number_to_check;
            if(channel_take(channel_one,&number_to_check)<0){
              exit(-1);          
            }
            if(isPrime(number_to_check)){
              if(channel_put(channel_two,number_to_check)<0){
                on=0;
                printf("CHECKER: %d OFF\n",getpid());
                channel_destroy(channel_one);
                exit(0); 
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
          printf("PRINTER OFF:%d\n",getpid());
          channel_destroy(channel_two);
          exit(0);
      }
    }
    //wait for all children processes to end
  for(int p=0;p<2+checkers;p++){
    wait(0);
      }
  char user_reboot[100];
  printf("Type 1 to reboot and 0 to exit:\n");
  reboot=atoi(gets(user_reboot,100));
    }
  exit(0);
  
}



