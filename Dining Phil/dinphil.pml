#define N 5
mtype = {THINKING, HUNGRY, EATING};

mtype phil[N]; // state of each philosopher
int eatCount[N]; // number of times each philosopher ate

typedef mutex
{
    bool mstate; // 0 = unlocked. 1 = locked.
    byte mid;
}

mutex forks[N]; // one mutex for each fork

inline initprob() {
    int i;
    for (i : 1 .. N) {
        forks[i-1].mstate = 0;
        phil[i-1] = THINKING;
    }
}

// pthread_mutex_lock(&m);
inline lock(m){
  
  atomic{ 
          m.mstate == 0 -> m.mstate = 1; m.mid = _pid 
        };
  
}


// pthread_mutex_unlock(&m);
inline unlock(m){
  
  atomic{ 
           assert(m.mid == _pid); m.mstate = 0; m.mid = 0 
        };
  
}

proctype philosopher(int id) {
    do
    :: phil[id] == THINKING ->
        // philosopher is thinking
        printf("Philosopher %d is thinking\n", id+1);
        phil[id] = HUNGRY;

    :: phil[id] == HUNGRY ->
        // philosopher is hungry and trying to grab forks
        printf("Philosopher %d is hungry\n", id+1);
        lock(forks[id]); // acquire left fork
        lock(forks[(id+1)%N]); // acquire right fork
        phil[id] = EATING;
        eatCount[id] = eatCount[id] + 1;
        printf("Philosopher %d is eating\n", id+1);

    :: phil[id] == EATING ->
        // philosopher is eating and releases forks
        printf("Philosopher %d is done eating\n", id+1);
        unlock(forks[id]); // release left fork
        unlock(forks[(id+1)%N]); // release right fork
        phil[id] = THINKING;
    od
}

init {
  int z;
  printf("\n Dining Philosophers \n")
  initprob();
  for (z : 1 .. N) {
    run philosopher(z-1);
  }
}


