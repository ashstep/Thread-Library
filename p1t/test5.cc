#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests that wait cannot be called on a thread that does not have the proper lock

Expected output:

parent called with arg 1
Parent locked with Lock 1

Wait(2,1) called on parent

Failed to execute wait

Wait(1,1) called on parent

Thread library exiting.

**/
using namespace std;

void parent(void *a) {
  int arg;
  arg = (long int) a;
  cout << "parent called with arg " << arg << endl;
  start_preemptions(0,1,333);
  if(thread_lock(1)){
     cout << "Parent failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Parent locked with Lock 1\n"<< endl;
  }

  cout << "Wait(2,1) called on parent \n"<< endl;

  if(thread_wait(2,1)){
     cout << "Failed to execute wait\n"<< endl;
  }

  cout << "Wait(1,1) called on parent \n"<< endl;

  if(thread_wait(1,1)){
     cout << "Failed to execute wait\n"<< endl;
  }

  thread_unlock(1);

  
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
