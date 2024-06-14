#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "channel.h"
struct channel channel[NCHANNEL];
struct channel *initchannel;
int nextcd = 1;
struct spinlock cd_lock;
int
allocd()
{
  int cd;
  acquire(&cd_lock);
  cd=nextcd;
  nextcd = nextcd + 1;
  release(&cd_lock);
  return cd;
}

//initializing channel table
void
channelinit(void){
    struct channel *c;
    initlock(&cd_lock,"nextcd");
    for(c=channel; c<&channel[NCHANNEL];c++){
        initlock(&c->lock,"channel");
        c->state=EMPTY;
    }
    printf("initializing channels\n"); //small check
}

int
channel_create(void){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->state==EMPTY){
            goto found;
        }
        else{
            release(&c->lock);
        }  
    }
    return -1; //went through all couldnt create channel
    found:
        c->cd=allocd();
        c->state=FULL;
        c->creator=myproc();
        c->parent_pid=myproc()->pid;
        c->references=0; //this is the correct initial val?
        int cd=c->cd;
        c->put_lock=0;
        c->take_lock=0;
        release(&c->lock); 
        return cd;
}
int
channel_put(int cd,int data){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->cd==cd){
            if(c->state==OCCUPIED){
              while(c->state==OCCUPIED){
                sleep(c->put_lock,&c->lock);
                }
                if(c->state==EMPTY){
                    release(&c->lock);
                    return -1;
                }
                goto found;

            }
            else{
             if((c->state==FULL)||(c->state==UNOCCUPIED)){
                goto found;
                }
             else{
                release(&c->lock);
                return -1;
             }
            }
        }
        else{
            release(&c->lock);
        }  
    }
    return -1;
    found: 
        c->data=data;
        c->state=OCCUPIED;
        release(&c->lock); //i put data and release
        wakeup(c->take_lock);
        return 0;
}
int
channel_take(int cd,uint64 data){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->cd==cd){
            if((c->state==UNOCCUPIED)||(c->state==FULL)){
             while((c->state==UNOCCUPIED)||(c->state==FULL)){
                sleep(c->take_lock,&c->lock);
                }
                 if(c->state==EMPTY){
                    release(&c->lock);
                    return -1;
                }
                goto found;
            }
            else{
                if(c->state==OCCUPIED)
                    goto found;
                else {
                    release(&c->lock);
                    return -1;
                }
            }
        }
        else{
            release(&c->lock);
        }  
    }
    return -1; 
    found:
        if(data!=0 && copyout(myproc()->pagetable,data,(char *)&c->data,sizeof(c->data))<0){
            release(&c->lock);
            return -1; //could not perform copyout
        }
        c->state=UNOCCUPIED;
        release(&c->lock);
        wakeup(c->put_lock);
        return 0;
}
int 
channel_destroy(int cd){ //need to wake up processes sleeping on this with -1
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->cd==cd){
            goto delete;
        }
        else{
            release(&c->lock);
        }  
    } 
    return -1; //error an invalid cd didnt find any
    delete:  // no need to free lock 
    //no need to update status to deleted, cd wont be found in case of take or put and
    //-1 will be returned
        c->cd=0; 
        c->creator=0;
        c->data=0;
        c->references=0;
        c->state=EMPTY;
        release(&c->lock);
        wakeup(c->take_lock);
        wakeup(c->put_lock);
        return 0;
}
void channel_destroy_by_pid(int pid){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->parent_pid==pid){
            c->cd=0; 
            c->creator=0;
            c->data=0;
            c->references=0;
            c->state=EMPTY;
            release(&c->lock);
            wakeup(c->take_lock);
            wakeup(c->put_lock);
        }
        else{
            release(&c->lock);
        }  
    } 
}


