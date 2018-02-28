#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Expected output:
parent called with arg 1
Locks thread with lock 1

Wait called on parent
child called with id child thread 1
Locks thread with lock 1

Signals

Wait called onchild thread 1
child called with id child thread 2
Locks thread with lock 1

Signals

Wait called onchild thread 2
Parent broadcast
Unlocks parent

Thread library exiting.


**/
using namespace std;
int counter =0;

void child(void *a) {
  char *id;
  int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  if(thread_lock(1)){
      cout << "Failed to lock thread\n"<< endl;
  }
  else{
     cout << "Locks thread with lock 1\n"<< endl;
  }
  if (counter == 0){
   if(thread_signal(1,1)){
      cout << "Failed to signal\n"<< endl;
  }
  else{
     cout << "Signals\n"<< endl;
  }
}

 cout << "Wait called on" << (char *) id << endl;
  if (thread_wait(1,2)) {
    cout << "Child wait failed\n"; 
  }

   if (thread_broadcast(1,2)) {
    cout <<(char *) id << " broadcast failed\n"; 
  }
  else{
    cout <<(char *) id << " broadcast\n"; 
  }

  if(thread_unlock(1)){
      cout << "Failed to unlock "<< (char *) id << endl;
  }
  else{
     cout << "Unlocks child\n"<< endl;
  }
 
  
 
}


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  if(thread_lock(1)){
      cout << "Failed to lock thread\n"<< endl;
  }
  else{
     cout << "Locks thread with lock 1\n"<< endl;
  }

 if (thread_create((thread_startfunc_t) child, (void *) "child thread 1")) {
    cout << "thread_create failed\n"; 
    exit(1);
  }
  if (thread_create((thread_startfunc_t) child, (void *) "child thread 2")) {
    cout << "thread_create failed\n"; 
    exit(1);
  }
   cout << "Wait called on parent\n"; 
  if (thread_wait(1,1)) {
    cout << "Parent broadcast failed\n"; 
  }

  if (thread_broadcast(1,1)) {
    cout << "Parent broadcast failed\n"; 
  }
  else{
    cout << "Parent broadcast\n"; 
  }

  if(thread_unlock(1)){
      cout << "Failed to unlock parent\n"<< endl;
  }
  else{
     cout << "Unlocks parent\n"<< endl;
  }

  
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
