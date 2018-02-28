#include <stdio.h>  // needed for size_t
#include <unistd.h> // needed for sbrk
#include <assert.h> // needed for asserts
#include "dmm.h"

/* You can improve the below metadata structure using the concepts from Bryant
 * and OHallaron book (chapter 9).
 */

typedef struct metadata {
  /* size_t is the return type of the sizeof operator. Since the size of an
   * object depends on the architecture and its implementation, size_t is used
   * to represent the maximum size of any object in the particular
   * implementation. size contains the size of the data object or the number of
   * free bytes
   */
  size_t size;
  struct metadata* next;
  struct metadata* prev; 
} metadata_t;

/* freelist maintains all the blocks which are not in use; freelist is kept
 * sorted to improve coalescing efficiency 
 */

static metadata_t* freelist = NULL;

void* dmalloc(size_t numbytes) {
  /* initialize through sbrk call first time */

  if(freelist == NULL) { 			
    if(!dmalloc_init())
      return NULL;
  }

  assert(numbytes > 0);

  /* your code here */
  //Iterate through freelist until a large enough block is found 
  //if header specifies large enough block, split it off
  metadata_t* iter = freelist;
  //printf("Iter is here %p\n", (void*)iter);
  while(iter->next!=NULL){
   // printf("Iter size %lu\n", iter->size);
    if(iter->size >= numbytes){
      break;
    }
    iter = iter ->next;
  }
	
//Iter currently points at a large enough free block
//First consider not splitting the block
  if(iter->next!=NULL && iter->size < numbytes+ METADATA_T_ALIGNED &&  iter->size >= numbytes){
    iter->prev->next = iter->next;
    iter->next->prev = iter->prev;
    iter->next = NULL;
    iter ->prev = NULL;
   // printf("no split\n");
      return ((void*)iter) + METADATA_T_ALIGNED;
    }

//Initialize new block of size numbytes if iter is large enough 
// If previous statement didn't execute, the block is large enough
if(iter->next!=NULL && iter->size >= numbytes+ METADATA_T_ALIGNED){
metadata_t* iterSplit = (metadata_t *)(((void*) iter) + numbytes + METADATA_T_ALIGNED);
iterSplit->size = iter->size - numbytes - METADATA_T_ALIGNED;
// printf("New block size %lu\n", iterSplit->size);
//Adjust the size parameter
  iter -> size = numbytes;
  // printf("Modified block size %lu\n", iter->size);
//remove iter from freelist; keep iterSplit in the freelist
  iterSplit->prev = iter->prev;
  iterSplit->next = iter->next;
  if(iter->prev!=NULL){
  iter->prev->next = iterSplit;
}
if(iter->next!=NULL){
  iterSplit->next->prev = iterSplit;
}
  iter->next = NULL;
  iter ->prev = NULL;
  return ((void*)iter) + METADATA_T_ALIGNED;
 // return iter;
  }
return NULL;
}

void dfree(void* ptr) {
  /* your code here */
  //Find which block needs to be freed
  metadata_t* freeBlock = (metadata_t*) (ptr-METADATA_T_ALIGNED);
  // printf("Pointer to block is here %p\n", ptr-METADATA_T_ALIGNED);
 
  //add it back to linkedlist
  metadata_t* iter = freelist;
  //Place at beginning of freelist
  // if((void*) iter > ptr){
  //   freeBlock -> next = iter;
  //   iter -> prev = freeBlock;
  //   freelist = freeBlock;
  // }
  //Place in middle of list
 // else{
  while(iter->next!=NULL){
    // printf("Current iter %p\n", (void*) iter);
    // printf("Current iter-next %p\n", (void*) iter->next);
    // printf("Current iter-next-next %p\n", (void*) iter->next->next);


    if((void*) iter < ptr && (void*) iter->next > ptr){
      // printf("Found place for block\n");
      freeBlock->prev = iter;
      freeBlock->next = iter->next;
      iter->next->prev = freeBlock;
      iter->next = freeBlock;
      break;
    }
    // printf("okay...\n");
    iter = iter ->next;
    // printf("okay2...\n");
  }
  //Place at end of list 
  // if((void*) iter < ptr){
  //   iter->next = freeBlock;
  //   freeBlock->prev = iter;
  // }

//}

//Coalesce blocks
//merge freeBlock with the block before it and after
  // printf("Previous block plus crap: %p\n",(void*)freeBlock->prev + freeBlock->prev->size + METADATA_T_ALIGNED);
  // printf("Current block plus location: %p\n",(void*)freeBlock);
if(((void*)freeBlock->prev + freeBlock->prev->size + METADATA_T_ALIGNED == (void*) freeBlock) && 
  (void*)freeBlock + freeBlock ->size + METADATA_T_ALIGNED == (void*) (freeBlock->next) &&
  freeBlock->prev->prev!=NULL &&
  freeBlock->next->next!=NULL){
  // printf("three way coalesce\n");
  freeBlock->prev->size = freeBlock->prev->size + freeBlock->size + 2*METADATA_T_ALIGNED + freeBlock->next->size;
  freeBlock->prev->next = freeBlock->next->next;
  freeBlock->next->next->prev = freeBlock ->prev;
}
//merge freeBlock and freeBlock->next
else if((void*)freeBlock + freeBlock ->size + METADATA_T_ALIGNED == (void*) (freeBlock->next)
  && freeBlock->next->next!=NULL){
  freeBlock->size = freeBlock->size + METADATA_T_ALIGNED + freeBlock->next->size;
  freeBlock->next = freeBlock->next->next;
  freeBlock->next->prev = freeBlock;
}
else if(((void*)freeBlock->prev + freeBlock->prev->size + METADATA_T_ALIGNED == (void*) freeBlock) &&
  freeBlock->prev->prev!=NULL){
  freeBlock->prev->size = freeBlock->prev->size + freeBlock->size + METADATA_T_ALIGNED;
  freeBlock->prev->next = freeBlock->next;
  freeBlock->next->prev = freeBlock ->prev;
}



}

bool dmalloc_init() {

  /* Two choices: 
   * 1. Append prologue and epilogue blocks to the start and the
   * end of the freelist 
   *
   * 2. Initialize freelist pointers to NULL
   *
   * Note: We provide the code for 2. Using 1 will help you to tackle the 
   * corner cases succinctly.
   */

  size_t max_bytes = ALIGN(MAX_HEAP_SIZE);
  /* returns heap_region, which is initialized to freelist */
  freelist = (metadata_t*) sbrk(max_bytes); 
  /* Q: Why casting is used? i.e., why (void*)-1? */
  if (freelist == (void *)-1)
    return false;
  //IMPORTANT: 
  //Freelist is the prologue block; liststart is the actual freelist; epilogue is the epilogue block
  metadata_t* listStart = (metadata_t *)(((void*) freelist) + METADATA_T_ALIGNED);
  freelist->next = listStart;
  freelist->prev = NULL;
  freelist->size = 0;
  listStart->size = max_bytes-3*METADATA_T_ALIGNED;
  // printf("listStart->size is %lu \n", listStart->size);
  // printf("maxbytes is %lu \n", max_bytes); 
  listStart ->prev = freelist;
  metadata_t* epilogue = (metadata_t *)(((void*) listStart) + listStart->size + METADATA_T_ALIGNED);
  epilogue->size = 0;
  epilogue ->next = NULL;
  epilogue ->prev = listStart;
  listStart -> next = epilogue;
  // printf("epilogue here: %p\n",(void*)epilogue);
  return true;
}

/* for debugging; can be turned off through -NDEBUG flag*/
void print_freelist() {
  metadata_t *freelist_head = freelist;
  while(freelist_head != NULL) {
    DEBUG("\tFreelist Size:%zd, Head:%p, Prev:%p, Next:%p\t",
	  freelist_head->size,
	  freelist_head,
	  freelist_head->prev,
	  freelist_head->next);
    
    freelist_head = freelist_head->next;
   }

  DEBUG("\n");
}