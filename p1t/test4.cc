#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests yields and waits-- parent first obtains a lock and calls yield; child should block on obtaining the lock.
The parent then releases the lock and calls yield; child should run and then block on thread_wait.
Expected output:
parent called with arg 1
Parent locked

Yield called on parent

child called with id child thread 1
child thread 1 attempts to obtain lock
Parent resumes running

Parent unlocked

Yield called on parent

Child runs after obtaining lock

Parent failed to wait

Thread library exiting.

**/
using namespace std;

void child(void *a) {
 char *id;
  int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  cout <<(char *) id << " attempts to obtain lock"<<endl;
  if(thread_lock(1)){
     cout << "Error in obtaining lock\n"<< endl;
  }

 cout << "Child runs after obtaining lock\n"<< endl;

 if(thread_wait(1,1)){
     cout << "Child failed to wait\n"<< endl;
  }

  }


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  if(thread_lock(1)){
     cout << "Parent failed to obtain lock\n"<< endl;
  }
  else{
    cout << "Parent locked\n"<< endl;
  }

  if (thread_create((thread_startfunc_t) child, (void *) "child thread 1")) {
    cout << "thread_create failed\n";
    exit(1);
  }
 
 cout << "Yield called on parent\n"<< endl;
  if(thread_yield()){
     cout << "Failed to execute parent yield\n"<< endl;
  }
  
 
 cout << "Parent resumes running\n"<< endl;


  if(thread_unlock(1)){
     cout << "Parent failed to unlock\n"<< endl;
  }
  else{
    cout << "Parent unlocked\n"<< endl;
  }

cout << "Yield called on parent\n"<< endl;
 if(thread_yield()){
     cout << "Failed to execute parent yield\n"<< endl;
  }

  if(thread_wait(1,1)){
     cout << "Parent failed to wait\n"<< endl;
  }

}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}