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
struct spinlock channel_wait_lock; //do we need this?
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
    initlock(&channel_wait_lock,"channel_wait_lock");
    for(c=channel; c<&channel[NCHANNEL];c++){
        initlock(&c->lock,"channel");
        c->state=FULL;
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
    return -1; //went through all and couldnt create channel
    found:
        c->cd=allocd();
        c->state=FULL;
        c->creator=myproc();
        c->references=0; //this is the correct initial val?
        int cd=c->cd;
        release(&c->lock); //should i release it?
        return cd;
}
int
channel_put(int cd,int data){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->cd==cd){
            if(c->state==BUSY){
                sleep(c,&c->lock);//placeholder until I can think of smth
            }
            else{
                goto found;
            }
        }
        else{
            release(&c->lock);
        }  
    }
    return -1; 
    found:
        c->state=BUSY; //when can i put  date here? busy is for taking it no?
        c->data=data;
        return 0;
}
int
channel_take(int cd,uint64 data){
    struct channel *c;
    for(c=channel; c<&channel[NCHANNEL];c++){
        acquire(&c->lock);
        if(c->cd==cd){
            if(c->state==BUSY){
                sleep(c,&c->lock);//placeholder until I can think of smth
            }
            else{
                goto found;
            }
        }
        else{
            release(&c->lock);
        }  
    }
    return -1; 
    found:
        c->state=BUSY;
        if(data!=0 && copyout(myproc()->pagetable,data,(char *)&c->data,sizeof(c->data))<0){
            c->state=FREE;
            release(&c->lock);
            return -1; //could not perform copyout
        }
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
        c->cd=0;
        c->creator=0;
        c->data=0;
        c->references=0;
        c->state=EMPTY;
        return 0;
}


