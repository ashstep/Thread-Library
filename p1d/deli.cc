//Deli Program
#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>
#include <fstream>
#include <string>
#include <vector>
#include <limits.h>
#include <sstream>
#include <algorithm>


using namespace std;

/*

1. One master sandwich maker, and variable number of cashier. Therefore, 1 master thread 
+ "argc-2" other threads, one for each cashier. #1+argc-2
2. list of 1,000 sandwiches, and each sandwich is assigned its own unique number between 0 and 999
3. Cashiers post orders on the board so that the sandwich maker knows (1) what to prepare, 
   and (2) which cashier handled the order
4. board can hold a maximum number of order requests (max_orders), and cashiers must wait if the board is full.

5. program should start by creating a specified number of cashier threads to read in 
   sandwich orders from a file and one thread for the sandwich maker
6. each cashier thread should receive and forward a series of sandwich requests, which are specified in its input file
7. A cashier thread must wait until the sandwich maker has finished the last request from 
   the cashier before submitting its next order to ensure that a cashier's orders are completed in FIFO order
8. A cashier thread finishes after all the sandwiches in its input file have been prepared by the sandwich maker. 
9. sandwich maker does NOT prepare sandwiches from the board in FIFO order
10. sandwich maker's thread chooses the next sandwich based on how similar it is to the one it just completed, i.e.
    next sandwich that the sandwich maker should make is the one with the number on the board that is closest to the last sandwich's
11. maker is initialized with her last sandwich as -1
12. your sandwich-maker thread should only handle a request when the cork board has the largest possible number of orders
13. When at least max_orders cashier threads are alive, the largest possible number of requests on the board is max_orders. 
    When fewer than max_orders cashier threads are alive, the largest number of orders on the board is equal to the number of living cashier threads
14. maintain the number of living cashier threads as shared state.    
15. input file for each cashier contains that cashier's series of sandwich orders. Each line of the input file specifies the requested sandwich (0 to 999)
16. Open each input file read-only (use ifstream rather than fstream). 
17. After issuing a request, a cashier thread should call (note the space characters in the strings):
  cout << "POSTED: cashier " << cashier << " sandwich " << sandwich << endl;
    An order is available to be made (i.e., has been posted to the board) when the cashier thread prints this line.
18. After making a sandwich, the maker thread should make the following call (note the space characters in the strings):
  cout << "READY: cashier " << cashier << " sandwich " << sandwich << endl;
  An order is considered to be complete and off the board when the sandwich-maker thread prints this line.
19. Your program should not generate any other output. Note that the console is shared between the different threads. Hence the couts in your program must
    be protected by a monitor lock to prevent interleaving output from multiple threads. 

*/



//Use a sorted vector to store the orders? 
  int max_orders;
  int num_of_cashiers;
  int active_cashiers;
  int last_sandwich = -1;
  int current_order_count;

  unsigned int cashier_lock = 8888;
  unsigned int chef_lock = 8888;
  unsigned int order_added_lock = 7777;
  unsigned int cashier_cond = 6666;
  unsigned int chef_cond = 5555;

  std::vector<int> my_board;
  std::vector<int> my_cashiers;
  std::vector<char*> cashier_filename; //vector of the file names


//not using rn
struct order {
    int cashier_identifier;
    int order_identifier;
};

//struct cashier_arg {
  //int cashier_identifier;
  //char* filename;
//};


//order object creator
//call this each time you go to a new cashier and CHECK that none of their orders is currently on the board
void order_creator(int cashier, int order_num) {
  order new_order;
  new_order.cashier_identifier = cashier;
  new_order.order_identifier = order_num;
}



//making all the cashier threads- called each time
void cashier_funct(void *i) {
  thread_lock(cashier_lock);
   //cout << "i in cashier_function" << i << endl;
  
 long int file_index = (long int) i;
  //cout << "file_index in cashier_function" << file_index << endl;
  //cout << file_index
  char* my_file_name = cashier_filename[(int)file_index];
  string line;
  int order_num;
  ifstream my_stream (my_file_name); //reading in file
  
   if (my_stream.is_open()) {
    while ( getline (my_stream,line) ) {
      stringstream file_input(line);
      //Information is in "line"
      file_input>>order_num;
     // cout << "order_num in cashier_function" << order_num << endl;
      //check if corkboard is full before posting an order (or if one of this thread's orders is on the board???) 
      //If so, wait
      //( my_cashiers.find(my_cashiers.begin(), my_cashiers.end(), file_index) != my_cashiers.end() 
      while(my_board.size() >= max_orders ){
        thread_wait(cashier_lock, cashier_cond);
      }
     
      my_board.push_back(order_num);
      my_cashiers.push_back(file_index);
      cout << "POSTED: cashier " << file_index << " sandwich " << order_num << endl;
      thread_signal(chef_lock, chef_cond);
      thread_wait(cashier_lock, order_num);
    
    }
     
    my_stream.close();
    active_cashiers = active_cashiers - 1;
   thread_signal(chef_lock, chef_cond); 
    thread_unlock(cashier_lock);
  }
  
}

//Selects the next sandwich to make
int chooseSandwich(int prev_sandwich){
  int current_sandwich_index;
  int calc_diff;
  int difference = INT_MAX;
 for (int i=0; i<my_board.size(); i++) {
      calc_diff = abs(my_board[i] - prev_sandwich);
      if (calc_diff < difference) {
        difference = calc_diff;
        current_sandwich_index = i;
      }
    }
 
  return  current_sandwich_index;
}



//board THREAD CREATION
void board(void *argv) {
  thread_lock(chef_lock);
  int prev_sandwich = -1;
  int current_sandwich_index;
  int current_sandwich;
  
  //char** arguments =  (char **)argv;
  
  for (long int i = 0; i<num_of_cashiers; i++) {
    //cout << "i in board for each cashier" << i << endl;
    //The following line produces a warning because of the way we are casting i from void to long
    if (thread_create((thread_startfunc_t) cashier_funct, (void *) (i))) {
      //cout << "thread_create failed for cashier thread\n";
      exit(1);
    }
  }   
    //while there are still cashiers
    while(active_cashiers > 0 || !my_board.empty()){
      if(active_cashiers < max_orders){
        max_orders = active_cashiers;
      }
      //If there is still board space, wait for more orders
      while (my_board.size() < max_orders){
        thread_signal(cashier_lock, cashier_cond); //TODO signal?
        thread_wait(chef_lock, chef_cond);
      }
    //Make sandwich
    current_sandwich_index = chooseSandwich(prev_sandwich);
    current_sandwich = my_board[current_sandwich_index];
    cout << "READY: cashier " << my_cashiers[current_sandwich_index] << " sandwich " << current_sandwich << endl;
    my_board.erase(my_board.begin() + current_sandwich_index); //.begin -> An iterator to the beginning of the sequence container.
    my_cashiers.erase(my_cashiers.begin() + current_sandwich_index);
    prev_sandwich = current_sandwich;
    thread_signal(cashier_lock,current_sandwich);//or signal?
    thread_wait(chef_lock, chef_cond);
    }
  
  thread_unlock(chef_lock);
  // cashier_funct( (void *) "parent thread");
}




int main (int argc, char **argv){
  if (argc<=2){
    cout << "Error: Less than or equal to two arguments" <<endl;
    exit(1);
  }
  
  else{
      max_orders = atoi(argv[1]);
      num_of_cashiers = argc -2; 
      active_cashiers = num_of_cashiers;
    //cout << "Enters main" <<endl;
    //Creating a vector with filenames
    for(int i=0; i<num_of_cashiers; i++){
    //  cout << "Creating Filename vector" <<endl;
      cashier_filename.push_back(argv[i+2]);
      
   //   cout << "i in main_function for each cashier filename being put into vector:" << i << endl;
    }
    
  if (thread_libinit( (thread_startfunc_t) board, (void *) argv)) { 
    cout << "thread_libinit failed\n";
    exit(1);
  }
}
}
  

  
//exit(0) means a success, exit(1) failure
