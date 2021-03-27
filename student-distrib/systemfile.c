#include "systemfile.h"


inode* inode_men_start;
single_data_block* data_block_men_start;

/**/
uint32_t init_sysfile(void){
    inode_men_start=(inode*)sysfile_mem_start+1;
    data_block_men_start=(single_data_block*)(sysfile_mem_start->inode_count+inode_men_start);

    return 0;

};

uint8_t sysfile_open()
{



    
}