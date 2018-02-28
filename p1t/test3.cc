#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests wait and broadcast-- both child threads wait for signal that parent sends out once; 
both of them must go on the ready queue and run when the previous thread yields
Expected output:
parent called with arg 1
Wait called on parent

child called with id child thread 1
Wait is called on child thread 1
child called with id child thread 2
Wait is called on child thread 2
Parent resumes running

Parent broadcasts

Wait called on parent

child thread 1 resumes running

Wait is called on child thread 1
child thread 2 resumes running

Wait is called on child thread 2
Thread library exiting.

**/
using namespace std;

void child(void *a) {
 char *id;
  int i;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
  thread_lock(1);
  thread_signal(1,1);
  cout << "Wait is called on "<< (char *) id << endl;
   if(thread_wait(1,2)){
     cout << "Failed to execute wait\n"<< endl;
  }
  
 cout << (char*) id << " resumes running\n"<< endl;
 cout << "Wait is called on "<< (char *) id << endl;
 if(thread_wait(1,1)){
     cout << "Failed to execute wait\n"<< endl;
  }

  }


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  thread_lock(1);

  if (thread_create((thread_startfunc_t) child, (void *) "child thread 1")) {
    cout << "thread_create failed\n";
    exit(1);
  }
  if (thread_create((thread_startfunc_t) child, (void *) "child thread 2")) {
    cout << "thread_create failed\n";
    exit(1);
  }

  cout << "Wait called on parent \n"<< endl;
  if(thread_wait(1,1)){
     cout << "Failed to execute parent yield\n"<< endl;
  }

  cout << "Parent resumes running\n"<< endl;

    if(thread_broadcast(1,2)){
     cout << "Failed to execute broadcast\n"<< endl;
  }
  else{
    cout << "Parent broadcasts\n"<< endl;
  }

  cout << "Wait called on parent\n"<< endl;
  if(thread_wait(1,1)){
     cout << "Failed to execute wait\n"<< endl;
  }

}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
