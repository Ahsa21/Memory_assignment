#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"
#include <pthread.h>


typedef struct MemoryBlock {
   void * pnt; //pointer to part of memorypool in the memory
   struct MemoryBlock * Next; // pointer ton the next MemoryBlock
   int size; // size of the alocated memory
   bool free;  // if a piece of memory is free to alocate of not
} MemoryBlock;




void* memory_pool = NULL;
MemoryBlock* Block_pool = NULL; //name changed


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int number;
int number_alocated;




void mem_init(size_t size) {
   pthread_mutex_lock(&lock);


   memory_pool = malloc(size);
   Block_pool = malloc(sizeof(MemoryBlock));


   if (memory_pool == NULL || Block_pool == NULL) {
       printf("memory_pool or Block was not allocated\n");
       pthread_mutex_unlock(&lock);
       return;
   }


   Block_pool -> pnt = memory_pool;
   Block_pool -> Next = NULL;
   Block_pool -> size = size;
   Block_pool -> free = true;


   pthread_mutex_unlock(&lock);


   number = size;
   number_alocated = 0;




}






void* mem_alloc(size_t size) {
   pthread_mutex_lock(&lock);
   MemoryBlock* current = Block_pool;


   if (number_alocated + size > number) {
       printf("there is no memoryleft to allocate so much");
       pthread_mutex_unlock(&lock);
       return NULL;
   }


   while (current != NULL) {
      
       if (current -> free == true && current -> size >= size) {
           if (current->size == size) { // Exact size match
               current->free = false;
               number_alocated += size;
               pthread_mutex_unlock(&lock);
               return current->pnt;
           }


           MemoryBlock* New_block = malloc(sizeof(MemoryBlock));


           if (New_block == NULL) {
               printf("No new block was allocated!\n");
               pthread_mutex_unlock(&lock);
               return NULL;
           }


           New_block -> pnt = current -> pnt + size;


           New_block -> Next = current -> Next;
           current -> Next = New_block;


           New_block -> size = current -> size - size;
           current -> size = size;


           number_alocated += size;


           New_block -> free = true;
           current -> free = false;
           pthread_mutex_unlock(&lock);
           return current -> pnt;




       } else {
           current = current -> Next;
       }


   }


   pthread_mutex_unlock(&lock);
   return NULL;


}






void mem_free(void* block) {
   if (!block) {
       printf("You want to free a null pointer\n");
       return;
   }
   pthread_mutex_lock(&lock);


   MemoryBlock * current = Block_pool;


   while (current != NULL)
   {
       if (current ->pnt == block) {
           current ->free = true;
           number_alocated -= current ->size;


           MemoryBlock* Next_Block = current ->Next;
           if (current ->free && Next_Block !=NULL && Next_Block ->free) {
               current -> size += Next_Block -> size;
               current -> Next = Next_Block -> Next;
               free(Next_Block);
               }
               pthread_mutex_unlock(&lock);
               return;


       } else {
           current = current ->Next;
       }
   }


   pthread_mutex_unlock(&lock);
  
  
}




void* mem_resize(void* block, size_t size) {
   if (block ==NULL) {
       printf("It is not possible to resize a block that is NULL\n");


   } else {
       pthread_mutex_lock(&lock);
       MemoryBlock* current = Block_pool;


       while(current != NULL) {
           if(current->pnt != block) {
               current = current ->Next;


           }
          
           else {


               if (current ->size >= size) {
                   printf("No need to resize\n");
                   pthread_mutex_unlock(&lock);
                   return block;
               } else {
                   void* new_pointer = mem_alloc(size);
                   memcpy(new_pointer, block, current ->size);
                   mem_free(block);
                   pthread_mutex_unlock(&lock);
                   return new_pointer;
               }
           }
       }
   }


   pthread_mutex_unlock(&lock);
   return NULL;
}








void mem_deinit() {
   free(memory_pool);
   memory_pool = NULL;


   pthread_mutex_lock(&lock);
   MemoryBlock*  current = Block_pool;
   while (current != NULL) {
       MemoryBlock *Next_Block = current->Next;
       free(current);
       current = Next_Block;
   }


   Block_pool = NULL;
   pthread_mutex_unlock(&lock);
   pthread_mutex_destroy(&lock);


   number_alocated = 0;
   number = 0;


}
