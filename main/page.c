#include "page.h"
#include <stdint.h>
#include <stdio.h>

  extern int _end_kernel;
  unsigned long* kernelEnd = &_end_kernel;
  struct ppage physical_page_array[128];
  struct ppage *head;
  int physical_page_index_count=1;

  struct ppage *free_list = NULL;
 struct page_directory_entry page_directory[1024] __attribute__((aligned(4096)));
 struct page page_table[1024] __attribute__((aligned(4096)));
  


  void init_pfa_list(void) {
     head =  &physical_page_array[0];

     unsigned long start_addr = (((unsigned long)kernelEnd) & 0xFFFFF000) + 4096; // NAK enclosing the AND op in parens.
     void* current_addr = (void *)start_addr;
     physical_page_array[0].physical_addr = current_addr;
     physical_page_array[0].next = &physical_page_array[1];
     physical_page_array[0].prev = head;
     

     for (int i =1; i< 127; i++) {
	physical_page_array[i].physical_addr = physical_page_array[i-1].physical_addr+4096;      
	
	physical_page_array[i].next = &physical_page_array[i+1];
	physical_page_array[i+1].prev = &physical_page_array[i-1];
     } 
     physical_page_array[127].physical_addr=physical_page_array[126].physical_addr+4096;
     physical_page_array[127].next = 0;
     physical_page_array[127].prev = &physical_page_array[126];
   }  

   struct ppage *allocate_physical_pages(unsigned int npages) {
       struct ppage *allocd_list = head;
       struct ppage *curr_page = head;
       for (int i=0; i<npages-1; i++) {
	  curr_page  = curr_page->next;
       }
       head = curr_page->next;
       head->prev=0;
       curr_page->next =0;
       return allocd_list;
   }
    
   void free_physical_pages(struct ppage *ppage_list) {
      struct ppage *curr_ppage = ppage_list;
      while (curr_ppage->next != 0) {
	 curr_ppage = curr_ppage->next;
      }
      curr_ppage -> next = head;
      head -> prev = curr_ppage;
      head = ppage_list;
    }


   void init_page_table() {
      page_directory[0].present = 1;
      page_directory[0].rw = 1;
      page_directory[0].frame = (unsigned int)&page_table[0] >> 12;

      for (int i = 1; i < 1024; i++){

         page_directory[i].frame = 0;
         page_directory[i].present = 0;
         page_directory[i].rw = 0;

        //Identity mapping
         page_table[i].frame = i;
         page_table[i].rw = 1;
         page_table[i].present = 1;
      }

//  enabling paging
     asm("mov %0,%%cr3"
     :
     : "r"(page_directory)
     :);
    // NAK: Changing the literal you were ORing with EAX in the second asm line below. you were using the wrong literal (missing a 0), and the MMU wansn't turning on.
     asm("mov %cr0, %eax\n"
        "or $0x80000001,%eax\n"
        "mov %eax, %cr0");
}

   int map_pages(void *vaddr, struct ppage* physical_page, struct page_directory_entry *pd) {
      struct ppage *current = physical_page;
      while(current != NULL) {
	unsigned int vpn =((unsigned long int)vaddr) >> 12;
	uint32_t p_d = (unsigned int)vaddr >> 22; // NAK fixing p_d and p_t indexes
	uint32_t p_t = (vpn&0x3ff);
	struct page *temp = pd[p_d].frame<<12;
	temp[p_t].frame = (unsigned int) current->physical_addr>>12;
	temp[p_t].present = 1;
	temp[p_t].rw = 1;
	vaddr += 0x1000;
	current = current->next;
      }
      return vaddr;
}
