#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Checks that locking and unlocking twice properly return errors
Expected output:
parent called with arg 1
Parent locked

Correctly failed to obtain lock

Parent unlocked

Correctly failed to unlock an unlocked thread

Thread library exiting.

**/
using namespace std;


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  if(thread_lock(42)){
     cout << "Parent failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Parent locked\n"<< endl;
  }
  if(thread_lock(42)){
     cout << "Correctly failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Incorrectly locked twice\n"<< endl;
  }
  if(thread_unlock(42)){
     cout << "Parent failed to unlock\n"<< endl;
  }
  else{
    cout << "Parent unlocked\n"<< endl;
  }
  if(thread_unlock(42)){
     cout << "Correctly failed to unlock an unlocked thread\n"<< endl;
  }
  else{
    cout << "Incorrectly unlocked an unlocked thread\n"<< endl;
  }


  

  
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}