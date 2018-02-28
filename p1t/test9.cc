#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>
#include <stdio.h>
/**
Tests multiple running threads with waits, signals, and yields. 

Expected output:
child called with id child 1
child 1 locked

child 1 waiting

child called with id child 2
child 2 locked

child 2 yields

child called with id child 3
child 2 runs

child 2 unlocked

child 1 unlocked

Thread library exiting.
**/


using namespace std;

void child_three(void *a){
    char *id;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
    thread_signal(1,1);
}

void child_two(void *a){
    char *id;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
    if(thread_lock(1)){
     cout <<  (char *) id <<" failed to obtain lock\n"<< endl;
  }
  else{
    cout <<(char *) id <<" locked\n"<< endl;
  }
  cout <<  (char *) id <<" yields\n"<< endl;
  if(thread_yield()){
     cout << (char *) id <<" failed to yield\n"<< endl;
  }
  cout <<  (char *) id <<" runs\n"<< endl;
   if(thread_unlock(1)){
     cout <<  (char *) id <<" failed to unlock\n"<< endl;
  }
  else{
    cout << (char *) id <<" unlocked\n"<< endl;
  }
   
}

void child_one(void *a){
char *id;

  id = (char *) a;
  cout <<"child called with id " << (char *) id << endl;
    if(thread_lock(1)){
     cout <<  (char *) id <<" failed to obtain lock\n"<< endl;
  }
  else{
    cout <<  (char *) id <<" locked\n"<< endl;
  }
    cout << (char *) id <<" waiting\n"<< endl;
    if(thread_wait(1,1)){
         cout <<"Wait failed to execute\n"<< endl;
    }
    
    if(thread_unlock(1)){
     cout <<  (char *) id <<" failed to unlock\n"<< endl;
  }
  else{
    cout << (char *) id <<" unlocked\n"<< endl;
  }

}
void parent(){
    if (thread_create((thread_startfunc_t) child_one, (void *) "child 1")) {
    cout << "thread_create failed\n";
    exit(1);
  }
  if (thread_create((thread_startfunc_t) child_two, (void *) "child 2")) {
    cout << "thread_create failed\n";
    exit(1);
  }
  if (thread_create((thread_startfunc_t) child_three, (void *) "child 3")) {
    cout << "thread_create failed\n";
    exit(1);
  }
    

}
int main() {
    thread_libinit( (thread_startfunc_t) parent, (void *) 1);
}
