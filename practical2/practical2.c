#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cond.c"


int pthread_state = 1;
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t var = PTHREAD_COND_INITIALIZER;

int pnum;  // number updated when producer runs.
int csum;  // sum computed using pnum when consumer runs.

int (*pred)(int); // predicate indicating if pnum is to be consumed

int produceT() {
  scanf("%d",&pnum); // read a number from stdin
  return pnum;
}

void *Produce(void *a) {
  int p;
  // lock mutex
  pthread_mutex_lock(&mutex);

  p=1;
  while (p) {
    printf("@P-READY\n");
    p = produceT();
    printf("@PRODUCED %d\n",p);
    
    pthread_cond_signal(&var); // Signal the condition variable
    
    pthread_state = 0;
     while(!pthread_state)
    {
      pthread_cond_wait(&var , &mutex); // wait for the next signal
    }
     pthread_mutex_unlock(&mutex); // unlock the mutex
  }
  printf("@P-EXIT\n");
  pthread_exit(NULL);
}


int consumeT() {
  if ( pred(pnum) ) { csum += pnum; }
  return pnum;
}

void *Consume(void *a) {
  int p;

  p=1;
  pthread_mutex_lock(&mutex); // lock mutex
  while (p) {
     while(pthread_state)
    {
      pthread_cond_wait(&var , &mutex); // wait for signal
    }
    printf("@C-READY\n");
    p = consumeT();
    printf("@CONSUMED %d\n",csum);
    pthread_state = 1;
    pthread_cond_signal(&var); // signal pthread
    pthread_mutex_unlock(&mutex); // unlock mutex
  }
  printf("@C-EXIT\n");
  pthread_exit(NULL);
}


int main (int argc, const char * argv[]) {
  // the current number predicate
  static pthread_t prod,cons;
	long rc;

  pred = &cond1;
  if (argc>1) {
    if      (!strncmp(argv[1],"2",10)) { pred = &cond2; }
    else if (!strncmp(argv[1],"3",10)) { pred = &cond3; }
  }


  pnum = 999;
  csum=0;
  
  printf("@P-CREATE\n");
 	rc = pthread_create(&prod,NULL,Produce,(void *)0);
	if (rc) {
			printf("@P-ERROR %ld\n",rc);
			exit(-1);
		}
  printf("@C-CREATE\n");
 	rc = pthread_create(&cons,NULL,Consume,(void *)0);
	if (rc) {
			printf("@C-ERROR %ld\n",rc);
			exit(-1);
		}

  printf("@P-JOIN\n");
  pthread_join( prod, NULL);
  printf("@C-JOIN\n");
  pthread_join( cons, NULL);


  printf("@CSUM=%d.\n",csum);

  return 0;
}
