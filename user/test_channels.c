
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int cd=channel_create();
    if (cd<0)
    {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork()== 0)//child proc
    {
        if (channel_put(cd,42)< 0)
        {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_put(cd,43)<0)
        {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_destroy(cd)<0)
        {
            printf("Failed to destroy the channel\n");
            exit(1);
        }
    }
    else//parent proc
    {

        int data;
        if (channel_take(cd, &data) < 0)
        {
            printf("Failed to take data from channel\n");
            exit(1);
        }
        else
         printf("Data is %d\n", data);
        if (channel_take(cd, &data) < 0)
        {
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("Data is %d\n",data);
        if (channel_take(cd,&data) < 0)
        {
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("Data is: %d\n",data);
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