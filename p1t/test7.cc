#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>


/**
Tests if garbage dump method successfully works, i.e., if threads are actually deleted after they finish running

Expected output:
parent called with arg 1
Threads successfully deleted
Thread library exiting.


**/
using namespace std;

void child(void *a) {
  char *id;
  int i;

  id = (char *) a;

  if(thread_signal(1,1)){
     cout <<"child signal failed" << endl;
  }

 
}


void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;

  thread_lock(1);

  for(int i = 0; i<50000 ; i++){
    //cout << "printing stuff" << endl;
  if (thread_create((thread_startfunc_t) child, (void *) "child thread")) {
    //cout << "thread_create failed\n";
    exit(1);
  }
// cout << "Wait called " << endl;
  thread_wait(1,1);
  }

  cout << "Threads successfully deleted\n";
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 1 )) {
    cout << "thread_libinit failed\n"<< endl;
    exit(1);
  }
}
