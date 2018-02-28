#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Checks that locking and unlocking twice properly return errors
Expected output:
parent called with arg 1
Parent locked

Wait called

Correctly failed to execute wait

Parent resumes

Wait called

Child locked

Signal(2,1) attempted
Signal(1,4) attempted
Signal(3,3) attempted
Broadcast(2,1) attempted
Broadcast(1,4) attempted
Broadcast(3,3) attempted
Wait called on child
Signal(1,1) attempted by child2
unlocked parent thread

Thread library exiting.


**/
using namespace std;

void child2(void *a) {
   cout << "Signal(1,1) attempted by child2" << endl;
if(thread_signal(1,1)){
     cout << "Signal error\n"<< endl;
  }

}

void child(void *a) {
   if(thread_lock(1)){
     cout << "Child error in acquiring lock\n"<< endl;
  }
  else{
    cout << "Child locked\n"<< endl;
  }

  cout << "Signal(2,1) attempted" << endl;
if(thread_signal(2,1)){
     cout << "Signal error\n"<< endl;
  }

  cout << "Signal(1,4) attempted" << endl;
if(thread_signal(1,4)){
     cout << "Signal error\n"<< endl;
  }
  cout << "Signal(3,3) attempted" << endl;
if(thread_signal(3,3)){
     cout << "Signal error\n"<< endl;
  }

    cout << "Broadcast(2,1) attempted" << endl;
if(thread_broadcast(2,1)){
     cout << "Broadcast error\n"<< endl;
  }

  cout << "Broadcast(1,4) attempted" << endl;
if(thread_broadcast(1,4)){
     cout << "Broadcast error\n"<< endl;
  }
  cout << "Broadcast(3,3) attempted" << endl;
if(thread_broadcast(3,3)){
     cout << "Broadcast error\n"<< endl;
  }


if (thread_create((thread_startfunc_t) child2, (void *) "child thread 2")) {
    cout << "thread_create failed\n";
    exit(1);
  }

   cout << "Wait called on child" << endl;
  if(thread_wait(1,2)){
     cout << "Child failed to execute wait\n"<< endl;
  

//    cout << "Signal attempted" << endl;
// if(thread_signal(1,1)){
//      cout << "Signal error\n"<< endl;
//   }
//   cout << "Wait called on child" << endl;
//   if(thread_wait(1,1)){
//      cout << "Child failed to execute wait\n"<< endl;
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

  if (thread_create((thread_startfunc_t) child, (void *) "child thread")) {
    cout << "thread_create failed\n";
    exit(1);
  }

  cout << "Wait called\n"<< endl;
if(thread_wait(2,1)){
     cout << "Correctly failed to execute wait\n"<< endl;
  }

 cout << "Parent resumes\n"<< endl;
   cout << "Wait called\n"<< endl;
if(thread_wait(1,1)){
     cout << "failed to execute wait\n"<< endl;
  }

  if(thread_unlock(1)){
     cout << " failed to unlock an unlocked thread\n"<< endl;
  }
  else{
    cout << "unlocked parent thread\n"<< endl;
  }


  

  
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}