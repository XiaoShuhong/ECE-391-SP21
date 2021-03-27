#ifndef _SYSTEMFILE_H
#define _SYSTEMFILE_H

#include "types.h"
#define RESERVE_boot_block 52
#define RESERVE_dentry 24
#define FILENAME_LEN 32
#define MAX_DENTRY 63
#define MAX_DATA_BLOCK_NUM 1023
#define BLOCK_SIZE 4096
#define MAX_FILE_NAME_LEN 32
#define FAIL -1



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


uint32_t init_sysfile(void);
int32_t sysfile_open(const uint8_t* filename);
int32_t sysfile_close(int32_t fd);
int32_t sysfile_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t sysfile_read(int32_t fd, uint32_t offset, void* buf,uint32_t length);
int32_t read_dentry_by_name(const uint8_t* fname, dentry* tar_dentry);
void __create_buffer__( uint8_t* const fname_buffer, uint8_t* const cur_filename_buffer,uint8_t* const fname,uint8_t* const cur_filename );
int32_t read_dentry_by_index(uint32_t index, dentry* tar_dentry);
int32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf , uint32_t length);
int32_t sysdir_read(int32_t fd, uint32_t offset, void* buf, int32_t nbytes);
int32_t sysdir_open(const uint8_t* filename);
int32_t sysdir_close(int32_t fd);
int32_t sysdir_write(int32_t fd, const void* buf, int32_t nbytes);















#endif