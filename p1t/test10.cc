#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
More complicated test case with signals and loops, simulating consumers and producers
Expected output:
child 0 ready
child 1 ready
child 2 ready
Yield called on parent
child called with id child 0
child 0 attempts to obtain lock
child 0 runs
Yield called on child 0
child called with id child 1
child 1 attempts to obtain lock
child called with id child 2
child 2 attempts to obtain lock
child called with id parent thread
parent thread attempts to obtain lock
Wait called on child 0
child 1 runs
Yield called on child 1
Wait called on child 1
child 2 runs
child 2 signals
Yield called on child 2
parent thread runs
Yield called on parent thread
Wait called on parent thread
child 0 signals
Yield called on child 0
Yield called on child 1
Wait called on child 1
Thread library exiting.


**/
using namespace std;


int count = 0;

void child_one(void *a){
  char *id;
    int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  cout <<(char *) id << " attempts to obtain lock "<< endl;
  thread_lock(1);
  cout <<(char *) id << " runs"<< endl;
  while(count==1){
    cout <<"Yield called on "<<(char *) id << endl;
    if(thread_yield()){
      cout <<"Issue with yield" << endl;
    };
    cout <<"Wait called on "<<(char *) id << endl;
    thread_wait(1,2);
  }
count++;
thread_signal(1,2);
cout <<(char *) id << " signals"<< endl;
cout <<"Yield called on "<<(char *) id << endl;
if(thread_yield()){
      cout <<"Issue with yield" << endl;
    };
thread_unlock(1);
}

void child_zero(void *a){
  char *id;
  int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  cout <<(char *) id << " attempts to obtain lock "<< endl;
  thread_lock(1);
  cout <<(char *) id << " runs"<< endl;
  while(count==0){
    cout <<"Yield called on "<<(char *) id << endl;
    if(thread_yield()){
      cout <<"Issue with yield" << endl;
    };
    cout <<"Wait called on "<<(char *) id << endl;
    thread_wait(1,2);
  }
count--;
thread_signal(1,2);
cout <<(char *) id << " signals"<< endl;
cout <<"Yield called on "<<(char *) id << endl;
if(thread_yield()){
      cout <<"Issue with yield" << endl;
    };
thread_unlock(1);
}

void parent(void *a){
    if (thread_create((thread_startfunc_t) child_zero, (void *) "child 0")) {
    cout << "thread_create failed\n";
    exit(1);
  }
   cout <<"child 0 ready "<< endl;
 
  if (thread_create((thread_startfunc_t) child_zero, (void *) "child 1")) {
    cout << "thread_create failed\n";
    exit(1);
  }
  cout <<"child 1 ready "<< endl;

  if (thread_create((thread_startfunc_t) child_one, (void *) "child 2")) {
    cout << "thread_create failed\n";
    exit(1);
  }
 cout <<"child 2 ready "<< endl;
 cout <<"Yield called on parent "<< endl;
  if(thread_yield()){
      cout <<"Issue with yield" << endl;
    };
  child_one((void*) "parent thread");
    

}


int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
