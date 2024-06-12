#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int isPrime(int num){ //naive checking approach
  for(int i=2;i<num;i++){
    if((i!=num) &&(num%i==0))
      return 0;
  }
  return 1;
}

int
main(int argc, char *argv[])
{
  int counter=0; //counts number of found primes
  char user_input[100];
  int checkers=3; //default val is 3
  int channel_one=channel_create();
  int generated_num=2;
  // CREATING CHANNELS IN PARENT PROC
  if(channel_one<=0){
      printf("error creating first channel\n");
      exit(-1);
    } 
  int channel_two=channel_create();
  if(channel_two<=0){
      printf("error creating second channel\n");
      exit(-1);
    } 
  printf("Hi there, how many prime checkers do you want?\n");
  checkers=atoi(gets(user_input,100));
  printf("number of inputed checkers is %d\n",checkers);
  if(fork()==0){ //generator
    while (counter<100)
    {
      // printf("GENERATED %d\n",generated_num);
      if(channel_put(channel_one,generated_num)<0){
        printf("error putting generated numbers in first channel\n");
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
            printf("error taking generated numbers from first channel\n");
            exit(-1);          
          }
          if(isPrime(number_to_check)){
            if(channel_put(channel_two,number_to_check)<0){
              printf("error putting prime numbers in second channel\n");
              exit(-1); 
            }
          }
      }
      }

    }
    if(fork()==0){ //printer child
      while(counter<100){
        int prime;
        channel_take(channel_two,&prime);
        if(prime<0){
          printf("error taking prime numbers from second channel\n");
          exit(-1);
        }
        else{
          printf("Prime:%d Count: %d\n",prime,counter);
          counter=counter+1;
        }
      }
        exit(0);
    }
  }
  if(counter<100){
      wait(0);
  }
  exit(0);
}

