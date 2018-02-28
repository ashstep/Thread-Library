#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests that unlock can only unlock a thread that the lock is held on. Tests basic locking conventions.

Expected output:
parent called with arg 1
Correctly failed to unlock an thread with no lock held

Locks thread with lock 1

Locks thread with lock 2

Correctly failed to unlock an thread with different lock held

Parent exits
child called with id child thread
Correctly failed to unlock since other thread holds lock

Call to thread_lock(1)

Thread library exiting.



**/
using namespace std;

void child(void *a) {
  char *id;
  int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  if(thread_unlock(1)){
     cout << "Correctly failed to unlock since other thread holds lock\n"<< endl;
  }
  else{
    cout << "Incorrectly unlocked a thread\n"<< endl;
  }

  cout << "Call to thread_lock(1)\n"<< endl;
  if(thread_lock(1)){
     cout << "Error in acquiring lock 1\n"<< endl;
  }
  
  cout << "Thread runs\n"<< endl;
 
}


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  if(thread_unlock(42)){
     cout << "Correctly failed to unlock an thread with no lock held\n"<< endl;
  }
  else{
    cout << "Incorrectly unlocked a thread\n"<< endl;
  }

  if(thread_lock(1)){
     cout << "Failed to lock thread\n"<< endl;
  }
  else{
    cout << "Locks thread with lock 1\n"<< endl;
  }

  if(thread_lock(2)){
      cout << "Failed to lock thread\n"<< endl;
  }
  else{
     cout << "Locks thread with lock 2\n"<< endl;
  }

  if(thread_unlock(4)){
     cout << "Correctly failed to unlock an thread with different lock held\n"<< endl;
  }
  else{
    cout << "Incorrectly unlocked a thread\n"<< endl;
  }



  if (thread_create((thread_startfunc_t) child, (void *) "child thread")) {
    cout << "thread_create failed\n"; 
    exit(1);
  }

cout << "Parent exits\n"; 
  // if(thread_wait()){
  //    cout << "Correctly failed to unlock an thread\n"<< endl;
  // }
  // else{
  //   cout << "Incorrectly unlocked a thread\n"<< endl;
  // }



}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}