// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// //still not added to compile
// int
// main(int argc, char *argv[])
// {
//   int cd=channel_create();
//   if(cd<0){
//     printf("Failed to create channel\n");
//     exit(1);
//   }
//   if(fork()==0){ //child process
//     if(channel_put(cd,42)<0){
//         printf("Failed to put data n channel\n");
//         exit(1);
//     }
//     channel_put(cd,43); //supposed to sleep until cleared
//     //handle error (no idea why they put this so far)
//     channel_destroy(cd);
//     //handle error (no idea why they put this so far)
//   }
//   else{ //parent process
//   int data;
//   if(channel_take(cd,&data)<0){//42
//     printf("Failed to take data from channel\n");
//     exit(1);
//   }
//   data=channel_take(cd,&data); //43
//   //handle error(what do they mean by this?)

//   data=channel_take(cd,&data); //uppoed to sleep until child destroys channel(ie exits?)
//   }
//   exit(0);
// }
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int cd = channel_create();
    if (cd < 0)
    {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0) //Child 
    {
        if (channel_put(cd, 42) < 0)
        {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_put(cd, 43) < 0)
        {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_destroy(cd) < 0)
        {
            printf("Failed to destroy the channel\n");
            exit(1);
        }
    }
    else //Parent
    {

        int data;
        if (channel_take(cd, &data) < 0)
        { // 42
            
            printf("SAD %d\n",data);
            printf("Failed to take data from channel\n");
            exit(1);
        }
        else
         printf("Data is %d\n", data);
        if (channel_take(cd, &data) < 0)
        { // 43
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("Data is %d\n",data);
        if (channel_take(cd,&data) < 0)
        { // Sleep until child destroys channel
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("Data is %d\n",data);
    }
    exit(0);
}
/*flow of this program goes something like this:
-channel is created (for sure)
then 2 scenarios happen:
1.child executes first then puts 42 into channel, goes to put 43 and sleeps!
2.parent executes first tries to take, sleeps, then wakes up when child puts 42
then parent takes 42, sleeps, child puts 43 goes on to destroy channel as parent 
tries to take 43
*/