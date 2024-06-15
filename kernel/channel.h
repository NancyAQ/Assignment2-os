enum channel_status { EMPTY,FULL,OCCUPIED,UNOCCUPIED}; //will see if i need more
struct channel {
  struct spinlock lock;
  enum channel_status state; //channel state
  int data;
  int cd;
  int references;            //for the bonus, to impl later
  struct proc *creator;      //pointer to creator process
  void* put_lock;
  void* take_lock;
  int parent_pid; //added for now
};
extern struct channel channel[NCHANNEL]; //global to destroy when parent dies or exits