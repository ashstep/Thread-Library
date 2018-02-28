#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests wait and signal-- parent waits for child thread's signal and resumes running
Expected output:

parent called with arg 1
Parent locked

Wait is called on parent

Child locked

Parent alerted

Parent correctly resumes running

Thread library exiting.

**/
using namespace std;

void child(void *a) {
   if(thread_lock(1)){
     cout << "Child error in acquiring lock\n"<< endl;
  }
  else{
    cout << "Child locked\n"<< endl;
  }
if(thread_signal(1,1)){
     cout << "Failed to signal parent\n"<< endl;
  }
  else{
    cout << "Parent alerted\n"<< endl;
  }
  if(thread_wait(1,1)){
     cout << "Child failed to execute wait\n"<< endl;
  }
 
  }


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  if(thread_lock(1)){
     cout << "Error in acquiring lock\n"<< endl;
  }
  else{
    cout << "Parent locked\n"<< endl;
  }

if (thread_create((thread_startfunc_t) child, (void *) "child thread")) {
    cout << "thread_create failed\n";
    exit(1);
  }
  
cout << "Wait is called on parent\n"<< endl;

  if(thread_wait(1,1)){
     cout << "Failed to execute wait\n"<< endl;
  }

  cout << "Parent correctly resumes running\n"<< endl;

  thread_wait(1,1);
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
