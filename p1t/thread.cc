#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "thread.h"
#include "interrupt.h"
#include <ucontext.h>
#include <deque>
#include <queue>
#include <map>

using namespace std;

struct Thread {
	ucontext_t *ucontext;
	char *stack;
	int completed;
};

struct Lock_Data {
  Thread *curr_lock_holder; //who has the lock now
  queue<Thread*> lock_queue; //requested thread lock queue
};

static bool init_library = false; 

static map<unsigned int, Lock_Data*> all_locks; 
static map<unsigned int, map<unsigned int, queue<Thread*> > > cv_map_lock_map_thread;

static deque<Thread*> readyThreads;
static Thread *currentThread;

static std::vector<Thread*> completedThreads;
static Thread *next_on_queue;
static queue<Thread*> completedQ;


//0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
// void garbageDump(){
//    // cout << "Garbage dump running ..." <<endl;
// 	for(int i = 0; i<completedThreads.size(); i++){
// 		delete[] completedThreads[i]->stack;
// 		completedThreads[i]->ucontext->uc_stack.ss_sp = NULL;
// 		completedThreads[i]->ucontext->uc_stack.ss_size =0;
// 		completedThreads[i]->ucontext->uc_stack.ss_flags =0;
//     	delete (char*) completedThreads[i] -> ucontext->uc_stack.ss_sp; //idk if this is necessary
//     	delete completedThreads[i]->ucontext;
//     	delete completedThreads[i];
// 	}
// completedThreads.clear();
//   //cout << "Garbage dump completed..." <<endl;
// }
//0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

void garbageDump(){
    while (completedQ.size()>0) {
      Thread* removeThis = completedQ.front();
      completedQ.pop();
      delete removeThis->stack;
      removeThis->ucontext->uc_stack.ss_sp = NULL;
      removeThis->ucontext->uc_stack.ss_size = 0;
      removeThis->ucontext->uc_stack.ss_flags = 0;
      removeThis->ucontext->uc_link = NULL;
      delete (char*) removeThis-> ucontext->uc_stack.ss_sp; //idk if this is necessary
      delete removeThis->ucontext;
      delete removeThis;
    }
  }
//0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000





static int start (thread_startfunc_t func, void *arg){ //Stub
    //cout << "Beginning start function, enable will be called" <<endl;
	interrupt_enable();
	func(arg);
	interrupt_disable(); 
	garbageDump();
	currentThread->completed=1;
//	completedThreads.push_back(currentThread);
  completedQ.push(currentThread);

    //cout << "readyThreads size is " << readyThreads.size()<<endl;
	if (readyThreads.size() > 0){
		next_on_queue = readyThreads.front(); 
		readyThreads.pop_front(); 
		currentThread = next_on_queue;
		setcontext(next_on_queue->ucontext);
	} else {  
	//	cout<< "Returning from start function, enable will be called"<<endl;
		//cout<<"Messing up in start.\n";
		cout << "Thread library exiting.\n";
		interrupt_enable();
		exit(0);
	}
} 
//*******************************************************************************************************

int thread_libinit(thread_startfunc_t func, void *arg){
  //cout << "Entering thread_libinit, disable will be called" <<endl;
	interrupt_disable();

	if (init_library) {
		interrupt_enable();
		return -1;
    } else {                
  	init_library = true;
    }
  
  	Thread* parent;
  	try {

  		parent = new Thread; 
  		parent->ucontext = new ucontext_t;
  		char *stack = new char [STACK_SIZE];
  		parent->stack = stack;
  		parent->completed = 0;

  		getcontext(parent->ucontext);

  	parent->ucontext->uc_stack.ss_sp = stack;
  	parent->ucontext->uc_stack.ss_size = STACK_SIZE;
  	parent->ucontext->uc_stack.ss_flags = 0;

  	currentThread = parent;

  	makecontext((parent->ucontext), (void (*)()) start, 2, func, arg);
  	setcontext(parent->ucontext);

  	}
  catch(...){
    interrupt_enable();
    return -1; 
	}
}

//*******************************************************************************************************

int thread_create(thread_startfunc_t func, void *arg){
  	//cout << "Entering thread_create, will call disable" <<endl;
	interrupt_disable();
	garbageDump();
	Thread *new_thread;
try {

    if (!init_library){ 
    	interrupt_enable();
    	return -1;
    }
    //cout << "Initializing new child thread" <<endl;
    new_thread = new Thread; 
    new_thread->ucontext = new ucontext_t;
    char *new_stack = new char [STACK_SIZE];
    new_thread -> stack = new_stack;
    new_thread->completed = 0;

        
    getcontext(new_thread->ucontext);
    
    new_thread->ucontext->uc_stack.ss_sp =  new_thread -> stack;
    new_thread->ucontext->uc_stack.ss_size = STACK_SIZE;
    new_thread->ucontext->uc_stack.ss_flags = 0;    

    makecontext((new_thread->ucontext), (void (*)()) start, 2, func, arg);
    readyThreads.push_back(new_thread);
}
catch(...){
	interrupt_enable();
	return -1;
}
	//cout << "Exiting thread_create, will call enable" <<endl;
interrupt_enable();
return 0;
}

//*******************************************************************************************************
int thread_yield(void){
  //  cout << "  Entering thread_yield, will call disable" <<endl;
	interrupt_disable();
	try {
		if (!init_library){
			interrupt_enable();
			return -1;
		}

		readyThreads.push_back(currentThread);
		Thread* prev = currentThread;
    	Thread* nxt = readyThreads.front();//first element on the ready list
    	readyThreads.pop_front();  //remove it frmo the list
    	currentThread = nxt;
    	swapcontext(prev->ucontext,nxt->ucontext);
	} catch(...) {
		interrupt_enable();
		return -1;
	}
interrupt_enable();
  //cout << "Exiting thread_yield..." <<endl;
return 0;
}

//*******************************************************************************************************

int thread_lock(unsigned int lock){
interrupt_disable();
garbageDump();
try {
	if (!init_library){
		interrupt_enable();
		return -1;
	}

		if(all_locks.find(lock) ==all_locks.end()){
			Lock_Data* tempLockData = new Lock_Data;
			tempLockData->curr_lock_holder = NULL;
			tempLockData->lock_queue = *(new queue<Thread*>);
			all_locks.insert(make_pair(lock, tempLockData));
		}

		if (cv_map_lock_map_thread.find(lock) == cv_map_lock_map_thread.end()) {
			queue<Thread*> tempQueue = *(new queue<Thread*>);
			map<unsigned int, queue<Thread*> > tempMap = *(new map<unsigned int, queue<Thread*> >);
			tempMap.insert(make_pair(NULL,tempQueue));
			cv_map_lock_map_thread.insert(make_pair(lock, tempMap));
		}

		if(all_locks[lock]->curr_lock_holder != NULL && all_locks[lock]->curr_lock_holder == currentThread){
			interrupt_enable();
			return -1;
		}

	if(all_locks[lock]->curr_lock_holder == NULL){
		all_locks[lock]->curr_lock_holder = currentThread;
		interrupt_enable();
        return 0;
    }else{
      	all_locks[lock]->lock_queue.push(currentThread);
        /*	if (readyThreads.size()==0) {
      		cout << "Thread library exiting.\n";
      		interrupt_enable();
      		exit(0);
      	} */

      	Thread* prevThread = currentThread;
      	Thread* nextThread = readyThreads.front();
      	if(nextThread == NULL){
      		//cout<<"Messing up in lock.\n";
      		cout<<"Thread library exiting.\n";
      		interrupt_enable();
      		exit(0);
      	}

      	readyThreads.pop_front();
        currentThread = nextThread; 
        swapcontext(prevThread->ucontext,nextThread->ucontext);
        interrupt_enable();
        return 0;
    }

} catch(...) {
	interrupt_enable();
	return -1;
}
interrupt_enable();
return 0;
}

//*******************************************************************************************************

int thread_unlock(unsigned int lock){
interrupt_disable();
try {
    if (!init_library){ 
    	interrupt_enable();
    	return -1;
    }
    if (all_locks.find(lock)==all_locks.end()) {
    	interrupt_enable();
    	return -1;
    }
    
    if ((all_locks[lock]->curr_lock_holder == NULL) || (all_locks[lock]->curr_lock_holder != currentThread)) {  
    	interrupt_enable();
    	return -1;
    }

  	all_locks[lock]->curr_lock_holder = NULL; //release the lock

    if (all_locks[lock]->lock_queue.size()!=0) { 
    	Thread* nxt = all_locks[lock]->lock_queue.front();
    	all_locks[lock]->lock_queue.pop();
    	all_locks[lock]->curr_lock_holder=nxt;
    	readyThreads.push_back(nxt); 
    }
} catch(...) {
	interrupt_enable();
	return -1;
}
interrupt_enable();
      
return 0;
}

//*******************************************************************************************************
//TODO: If the condition variable is not in the map 

int thread_wait(unsigned int lock, unsigned int cond){
interrupt_disable();
garbageDump();
try {
    if (!init_library){ 
    	interrupt_enable();
    	return -1;
    }

//*****************************************************************
//Call unlock
    try {

    	if (all_locks.find(lock)==all_locks.end()) {
      		interrupt_enable();
    		return -1;
    	}
    	if ((all_locks[lock]->curr_lock_holder == NULL) || (all_locks[lock]->curr_lock_holder != currentThread)) {
 			interrupt_enable();
    		return -1;
    	}
  	all_locks[lock]->curr_lock_holder = NULL; //release the lock

    if (all_locks[lock]->lock_queue.size()!=0) { 
    	Thread* nxt = all_locks[lock]->lock_queue.front();
    	all_locks[lock]->lock_queue.pop();
    	all_locks[lock]->curr_lock_holder=nxt;
    	readyThreads.push_back(nxt);
    }
		} catch(...) {
		interrupt_enable();
		return -1;
	}
//*****************************************************************
try{
if (cv_map_lock_map_thread[lock].find(cond) == cv_map_lock_map_thread[lock].end()) {
	queue<Thread*> tempQueue = *(new queue<Thread*>);
	cv_map_lock_map_thread[lock].insert(make_pair(cond, tempQueue));
} 
    cv_map_lock_map_thread[lock][cond].push(currentThread);
} catch(...){
	interrupt_enable();
	//cout<<"Messing up in wait1.\n";
	cout<< "Thread library exiting."<<endl;
	exit(0);
}
if (readyThreads.size()==0) {
 	//cout<<"Messing up in wait2.\n";
      		cout << "Thread library exiting.\n";
      		interrupt_enable();
      		exit(0);
      	}
 Thread* prevThread = currentThread;
 Thread* nextThread = readyThreads.front();

 /*
 if(nextThread==NULL){
 	cout<<"Messing up in wait2.\n";
 	cout<<"Thread library exiting.\n";
 	interrupt_enable();
 	exit(0);
 }
 */
 readyThreads.pop_front();
    currentThread = nextThread; 
    swapcontext(prevThread->ucontext,nextThread->ucontext);
//000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
//Calling Lock
    try {
    	if (!init_library){
    		interrupt_enable();
    		return -1;
    	}
    	if(all_locks.find(lock) ==all_locks.end()){
    		Lock_Data* tempLockData = new Lock_Data;
    		tempLockData->curr_lock_holder = NULL;
    		tempLockData->lock_queue = *(new queue<Thread*>);
    		all_locks.insert(make_pair(lock, tempLockData));
    	}
    	if (cv_map_lock_map_thread.find(lock) == cv_map_lock_map_thread.end()) {
    		queue<Thread*> tempQueue = *(new queue<Thread*>);
    		map<unsigned int, queue<Thread*> > tempMap = *(new map<unsigned int, queue<Thread*> >);
    		tempMap.insert(make_pair(NULL,tempQueue));
    		cv_map_lock_map_thread.insert(make_pair(lock, tempMap));
    	}
    	if(all_locks[lock]->curr_lock_holder != NULL && all_locks[lock]->curr_lock_holder == currentThread){
    		interrupt_enable();
    		return -1;
    	}
    	if(all_locks[lock]->curr_lock_holder == NULL){  
    		all_locks[lock]->curr_lock_holder = currentThread;
    		interrupt_enable();
          return 0;
      }
      else{
      	all_locks[lock]->lock_queue.push(currentThread);
      	if (readyThreads.size()==0) {
      		//cout<<"Messing up in lock within wait.\n";
      		cout << "Thread library exiting.\n";
      		interrupt_enable();
      		exit(0);
      	}
      	Thread* prevThread = currentThread;
      	Thread* nextThread = readyThreads.front();
      	readyThreads.pop_front();
        currentThread = nextThread; 
        swapcontext(prevThread->ucontext,nextThread->ucontext); 
        interrupt_enable();
        return 0;
    }

} catch(...) {
	interrupt_enable();
	return -1;
}
//**********************************************************************

} catch(...){
	interrupt_enable();
	return -1;
}
interrupt_enable();
return 0;
}
int thread_signal(unsigned int lock, unsigned int cond){

	interrupt_disable();
	try {
		if (!init_library){
			interrupt_enable();
			return -1;
		}

		Thread* signling_this;
  if (cv_map_lock_map_thread[lock].size()>0 && cv_map_lock_map_thread[lock][cond].size()>0 && cv_map_lock_map_thread.count(lock)>0 && cv_map_lock_map_thread[lock].count(cond)>0 ) { 
    signling_this = cv_map_lock_map_thread[lock][cond].front(); //get the thread being signaled
    cv_map_lock_map_thread[lock][cond].pop(); //remove it 
    readyThreads.push_back(signling_this); //place on ready Q
}


} catch(...){
	interrupt_enable();
	return -1;
}
interrupt_enable();
return 0;
}

int thread_broadcast(unsigned int lock, unsigned int cond){
	interrupt_disable();
	//garbageDump();
	try {
		if (!init_library){
			interrupt_enable();
			return -1;
		}

while (cv_map_lock_map_thread[lock].size()>0 && cv_map_lock_map_thread[lock][cond].size()>0 && cv_map_lock_map_thread.count(lock)==1 && cv_map_lock_map_thread[lock].count(cond)==1 ) { //what is the check here- is this right?
	try {
		Thread* signling_this;

  if (cv_map_lock_map_thread[lock].size()>0 && cv_map_lock_map_thread[lock][cond].size()>0 && cv_map_lock_map_thread.count(lock)>0 && cv_map_lock_map_thread[lock].count(cond)>0 ) { 
    signling_this = cv_map_lock_map_thread[lock][cond].front(); 
    cv_map_lock_map_thread[lock][cond].pop(); 
    readyThreads.push_back(signling_this); 
}

} catch(...){
	interrupt_enable();
	return -1;
}
}

} catch (...) {
	interrupt_enable();
	return -1;
}

interrupt_enable();

return 0;
}
