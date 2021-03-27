#include "systemfile.h"
#include "lib.c"

inode* inode_men_start;
single_data_block* data_block_men_start;

/**/
uint32_t init_sysfile(void){
    inode_men_start=(inode*)(sysfile_mem_start+1);
    data_block_men_start=(single_data_block*)(sysfile_mem_start->inode_count+inode_men_start);

    return 0;

};

int32_t sysfile_open(const uint8_t* filename)
{

    return 0;
}

int32_t sysfile_close(int32_t fd){
    return 0;
}

int32_t sysfile_write(int32_t fd, const void* buf, int32_t nbytes){
    return FAIL;
}


int32_t sysfile_read(int32_t fd, uint32_t offset, void* buf,uint32_t length){
	dentry* tar_dentry;
	if(read_dentry_by_name((uint8_t*)fd, tar_dentry) == -1){
		return -1;
	}
	return read_data(tar_dentry->inode_num, offset, buf, length);    
}









int32_t read_dentry_by_name(const uint8_t* fname, dentry* tar_dentry){
    int length,dentry_num;
    int i,j;
    int is_match;
    int matched_index=-1;//-1 means we haven't find matched dentry
    uint8_t fname_buffer[33],cur_filename_buffer[33];
    dentry cur_entry;
    uint8_t* cur_filename;
    length=strlen(fname);
    dentry_num=sysfile_mem_start->dir_count;//number of dentry  
    
    
    //if length of input filename is larger than filename buffer, return fail
    if (length>MAX_FILE_NAME_LEN || tar_dentry==NULL){
        return FAIL;
    }

    //this loop itrates dentrys
    for(i=0;i<tar_dentry;i++){
        cur_entry=sysfile_mem_start->direntries[i];
        cur_filename=cur_entry.filename;//cur_filename is the current dentry's name. used to compare the fname.
        __create_buffer__(fname_buffer, cur_filename_buffer,fname, cur_filename );
        is_match=1;//just assume matching, if not, will change this later.
        matched_index=i;

        //loop the entire filename, check if match
        for(j=0;j<MAX_FILE_NAME_LEN;j++){
            if(fname_buffer[j]!=cur_filename_buffer[j]){
                is_match=0;      //means this
                matched_index=-1;//-1 means we haven't find matched dentry
                break;
            }
        }



    }

    //if matched, fill dentry
    if(is_match==1){
        strncpy((int8_t*)tar_dentry->filename,(int8_t*)sysfile_mem_start->direntries[matched_index].filename ,  sizeof(tar_dentry->filename));
        tar_dentry->filetype=sysfile_mem_start->direntries[matched_index].filetype;
        tar_dentry->inode_num=sysfile_mem_start->direntries[matched_index].inode_num;
        return 0; 
    }
    return FAIL;
    
}


void __create_buffer__( uint8_t* const fname_buffer, uint8_t* const cur_filename_buffer,uint8_t* const fname,uint8_t* const cur_filename ){
    int i;//index
    int fname_end=0;
    int cur_filename_end=0;
    for(i=0;i<(MAX_FILE_NAME_LEN+1);i++){
        if(fname[i]=='\0'){
            fname_end=1;
        }
        if(fname_end==0)
            fname_buffer[i]=fname[i];
        else
            fname_buffer[i]='\0';
    }


    for(i=0;i<(MAX_FILE_NAME_LEN+1);i++){
        if(cur_filename[i]=='\0'){
            cur_filename_end=1;
        }
        if(cur_filename_end==0)
            cur_filename_buffer[i]=cur_filename_buffer[i];
        else
            cur_filename_buffer[i]='\0';
    }

}


int32_t read_dentry_by_index(uint32_t index, dentry* tar_dentry){
    int dentry_num;
    
    dentry_num=sysfile_mem_start->dir_count;//number of dentry 

    if(index>=dentry_num|| index<0){
        return FAIL;
    }
    strncpy((int8_t*)tar_dentry->filename, (int8_t*)sysfile_mem_start->direntries[index].filename , sizeof(tar_dentry->filename));
    tar_dentry->filetype=sysfile_mem_start->direntries[index].filetype;
    tar_dentry->inode_num=sysfile_mem_start->direntries[index].inode_num;
    return 0; 

}

int32_t read_data(uint32_t inode_idx, uint32_t offset, uint8_t* buf , uint32_t length){
    inode* tar_node;
    uint32_t end;
    int num_full_block,i;
    uint32_t num_read=0;
    uint32_t start_block_off;
    int start_data_block_index,end_data_block_index;
    single_data_block* start_data_block;
    single_data_block* end_data_block;
    if(sysfile_mem_start->inode_count<=inode_idx){
        return FAIL;
    }
    tar_node=(inode*)(inode_men_start+inode_idx);
    if(offset>=tar_node->length){
        return 0;
    }
    if(offset+length>tar_node->length){
        end =tar_node->length;
    }
    else {
        end=offset+length;
    }
    start_data_block_index=tar_node->data_block_num[offset/BLOCK_SIZE];
    end_data_block_index=tar_node->data_block_num[end/BLOCK_SIZE];
    start_data_block = &data_block_men_start[start_data_block_index];
    end_data_block = &data_block_men_start[end_data_block_index];
    start_block_off=offset%BLOCK_SIZE;

    memcpy(buf,((uint32_t)start_data_block) +start_block_off,BLOCK_SIZE-start_block_off);
    num_read+=BLOCK_SIZE-start_block_off;
    if (end_data_block_index==start_data_block_index){
        return num_read;
    }
    if((end_data_block_index-start_data_block_index)==1){
        memcpy(((uint32_t)buf)+num_read,end_data_block,length-num_read);
        num_read+=length-num_read;
        return num_read;
    }
    else{
        num_full_block=end_data_block_index-start_data_block_index-1;
        for(i=0;i<num_full_block;i++){
            memcpy(((uint32_t)buf)+num_read,&data_block_men_start[start_data_block_index+i],BLOCK_SIZE);
            num_read+=BLOCK_SIZE;
        }
        memcpy(((uint32_t)buf)+num_read,end_data_block,length-num_read);
        num_read+=length-num_read;
        return num_read;

    }
    return FAIL;

}


int32_t sysdir_read(int32_t fd, uint32_t offset, void* buf, int32_t nbytes){
    dentry* tar_dentry;
    int i;
	if(read_dentry_by_index(offset, tar_dentry) == -1){
		return FAIL;
	}
    for (i = 0; i < 33; i++){
        ((int8_t*)(buf))[i] = '\0';
    }

    strncpy((int8_t*)buf, (const int8_t*)tar_dentry->filename,strlen((int8_t*)tar_dentry->filename));
    return 0;



}



int32_t sysdir_open(const uint8_t* filename)
{

    return 0;
}

int32_t sysdir_close(int32_t fd){
    return 0;
}

int32_t sysdir_write(int32_t fd, const void* buf, int32_t nbytes){
    return FAIL;
}

