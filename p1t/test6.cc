#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests that nothing will run without a call to thread_libinit 

Expected output:

thread_create failed
Main method failed to obtain lock

Wait(1,1) called on main

Main failed to execute wait

Yield() called on main

Main failed to execute yield

Main failed to broadcast

Main method failed to unlock

parent called with arg 1
Parent locked with Lock 1

Wait(1,1) called on parent

Thread library exiting.


**/
using namespace std;

void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

   if(thread_lock(1)){
     cout << "Parent failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Parent locked with Lock 1\n"<< endl;
  }

  cout << "Wait(1,1) called on parent \n"<< endl;

  if(thread_wait(1,1)){
     cout << "Parent failed to execute wait\n"<< endl;
  }

}

int main() {

  if (thread_create((thread_startfunc_t) parent, (void *) "parent thread")) {
    cout << "thread_create failed\n";
  }

  if(thread_lock(1)){
     cout << "Main method failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Main locked with Lock 1\n"<< endl;
  }

  cout << "Wait(1,1) called on main \n"<< endl;

  if(thread_wait(1,1)){
     cout << "Main failed to execute wait\n"<< endl;
  }

  cout << "Yield() called on main \n"<< endl;

  if(thread_yield()){
     cout << "Main failed to execute yield\n"<< endl;
  }

  if(thread_broadcast(1,1)){
     cout << "Main failed to broadcast\n"<< endl;
  }

  if(thread_unlock(1)){
     cout << "Main method failed to unlock\n"<< endl;
  }
  else{
    cout << "Main unlocked\n"<< endl;
  }

 
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}