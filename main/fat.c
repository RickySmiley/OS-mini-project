#include <stdio.h>
#include "fat.h"
#include <stdlib.h>
#include "ide.h"


char boot_sector[512];
char fat_table_buff[8*1024];
char read_buff[514*4];
struct boot_sector *bs;
struct root_directory_entry *r = read_buff;




    int fat_init() {
	ata_lba_read(2048,boot_sector,1);
 
	bs = boot_sector;
	
//	if(bs->boot_signature != 0xaa55){
//	   return -1;
//	}
//	if(bs->fs_type[4] != '6'){
//	   return -1;
//	}
	int fat_table_start = 2048 + bs->num_reserved_sectors;
	ata_lba_read(fat_table_start,fat_table_buff,16);
	unsigned int root_sector = 2048 + bs->num_fat_tables * bs->num_sectors_per_fat + bs->num_reserved_sectors + bs->num_hidden_sectors; // NAK adding sector offset to beginning of partition. Also fixing calculation of root sector location
	ata_lba_read(root_sector,read_buff,4);
	return 1;
    }

    int fat_open(char *path) { 
	char directory_name[10];
	int i = 0;
	int j = 0;
	int read_enum;
	for (int r = 1;path[r] != '\0'; r++) { // NAK adding loop terminating condition
	    directory_name[i] =  path[r];
	    j=r;
	    i++;
	}

	directory_name[i] = ' ';
	for (read_enum = 0;; read_enum++) {
	    for(int i =0; i<10;i++) {
		if(r[read_enum].file_name[i] == directory_name[i]) {
		    if((r[read_enum].file_name[i] == ' ') || (directory_name[i] == '\0')){
			goto done;
		    }
		    continue;
		} else { // NAK if file name doesn't match what's in the RDE, move on to next RDE
            break;
        }
	    }
	}
	done:
	path = &path[j];
	return read_enum;
    }

    void fat_read(int read_enum, char *data_buf, int size_of_data_buff) {
	    char file_data[4096*16];
	    struct boot_sector *bs;
	    char super_block[512];
	    ata_lba_read(2048,super_block,1);
	    bs = super_block;
	    int fat_region_start = bs->num_reserved_sectors;
	    int root_directory_region_start = fat_region_start + (bs->num_fat_tables * bs->num_sectors_per_fat);
	    int data_region_start = root_directory_region_start + ((bs->num_root_dir_entries*32)/bs->bytes_per_sector);
	    int first_sector_of_cluster = data_region_start + ((r[read_enum].cluster-2)*bs->num_sectors_per_cluster);
	    ata_lba_read(first_sector_of_cluster,file_data,size_of_data_buff);
	    
    }
