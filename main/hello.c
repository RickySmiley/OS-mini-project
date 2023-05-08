#include <stdint.h>
#include "ide.h"
#include "fat.h"
#include "putc.h"
#include "page.h"

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot")))= {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

void init_pfa_list(void);
struct ppage *allocate_physical_pages(unsigned int npages);
void free_physical_pages(struct ppage *page_list);


void main() {
    unsigned short *vram = (unsigned short*)0xb8000; // Base address of video mem
    const unsigned char color = 7; // gray text on black background
 
    //Homework 1: terminal driver
   
    for(int i=0; i<300; i++) {
            esp_printf(putc,"hello world");
    }


    // Homework 2: Page frame initializer and page frame allocator
    init_pfa_list();
    struct ppage *allo_list=allocate_physical_pages(4);
    free_physical_pages(allo_list);

    //Homework 3: MMU 
    void *vaddr = (void *)0x200000; // NAK your map_pages function won't map any virt addr above 0x400000 because it's not capable of allocating a new second-level page table.
extern struct page_directory_entry page_directory[1024] __attribute__((aligned(4096)));
extern struct page page_table[1024] __attribute__((aligned(4096)));
    init_page_table(); // NAK calling init_page_table before map_pages to make sure that the page table is set up correctly
    map_pages(vaddr, allo_list, page_directory);

    
   //Homework 4: FAT FS Driver      

   fat_init();
   int read_enum = fat_open("TEST");
   char data_buf[4096];
   fat_read(read_enum,data_buf,16);




}

