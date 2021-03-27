#ifndef _SYSTEMFILE_H
#define _SYSTEMFILE_H

#include "types.h"
#define RESERVE_boot_block 52
#define RESERVE_dentry 24
#define FILENAME_LEN 32
#define MAX_DENTRY 63
#define MAX_DATA_BLOCK_NUM 1023
#define BLOCK_SIZE 4096

typedef struct dentry
{
    uint8_t filename[FILENAME_LEN];
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[RESERVE_dentry]
}dentry;

typedef struct boot_block{
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[RESERVE_boot_block];
    dentry direntries[MAX_DENTRY]

}boot_block;

typedef struct inode{
    uint32_t length;
    uint32_t data_block_num[MAX_DATA_BLOCK_NUM];
}inode;

typedef struct single_data_block{
    uint8_t data[BLOCK_SIZE];
}single_data_block;


extern boot_block* sysfile_mem_start;


















#endif