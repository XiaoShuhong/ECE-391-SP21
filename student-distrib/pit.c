/*pit.c used to init periodic interval timer device*/
/*Version 1 ML 2021/4/25 9:50*/

#include "pit.h"
#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "system_call.h"
#include "x86_desc.h"
#include "systemfile.h"
#include "terminal.h"


 uint32_t scheduled_index=2;


/* void init_pit(void)
 * Description: init the periodic interval timer device and set the frequency to 100Hz
 * Inputs: None
 * Outputs: None
 * Return value: None
 * Side effects: Allow the pit interrupts with the frequency at 100Hz
 */

void init_pit(void){
    outb(PIT_MODE, PIT_COMMAND_PORT);
    outb((uint8_t)(LATCH && 0xff), PIT_DATA_PORT_CHANNEL0);
    outb((uint8_t)(LATCH >> 8), PIT_DATA_PORT_CHANNEL0);

    enable_irq(pit_irq_number);
   
}


/* void pit_handler(void)
 * Description: handle the pit interrupt, and 
 * Inputs: None
 * Outputs: None
 * Return value: None
 * Side effects: 
 */
void pit_handler(void){
    cli();
    send_eoi(pit_irq_number);
    // printf("a");
    scheduling();
    sti();

}


void scheduling(void){
    // if(PCB_array[0] == NULL && PCB_array[1] == NULL && PCB_array[2] == NULL ){
    //     init_shells((uint8_t*)"shell"); 
    // }
    process_video_switch();
    process_switch();


}

void flush_TLB(void){
    asm volatile(
        "movl %%cr3,%%eax   ;"
        "movl %%eax,%%cr3   ;"
        : : : "eax","cc"
    );
}




void process_video_switch(void){
    uint32_t next_running_terminal = (scheduled_index + 1)%3; 
    if(current_terminal_number!=next_running_terminal){
        PT[video_memory>>shift_twelve].ptb_add=(video_memory+four_k*(1+next_running_terminal))>>shift_twelve;
        flush_TLB();
        PT_for_video[user_PT_index].ptb_add=(video_memory+four_k*(1+scheduled_index))>>shift_twelve;
        flush_TLB();
    }
    if(current_terminal_number==next_running_terminal){
        PT_for_video[user_PT_index].ptb_add=video_memory>>shift_twelve;
        flush_TLB();
        PT[video_memory>>shift_twelve].ptb_add=video_memory>>shift_twelve;
        flush_TLB();
        memcpy(( void *)video_memory,(const void *)(video_memory+four_k*(1+next_running_terminal)),(uint32_t)four_k);
    }
}


int32_t init_shells(const uint8_t* command){  
    cli(); //unable interupt
    int8_t filename[MAX_FILE_LEN];
    int8_t arg[BUF_SIZE];
    uint8_t ELF_buf[ELF_buf_size];

    dentry  tar_dentry;
    int32_t new_pid=-1; //the idx of pcb_array
    int32_t i=0;
    if(command==NULL){
        return FAIL;
    }
    if(split_argument(command,filename, arg)==FAIL){ //split the command into arg and filename
        return FAIL;
    }
    //check if shell>3
    int32_t g=0;//index
    uint8_t* SHELL=(uint8_t*)"shell";
    int32_t is_shell=1;
    for(g=0;g<5;g++){
        if (SHELL[g]!=filename[g]){ 
            is_shell = 0;
            break;}
    }
    if(shell_count >=3 && is_shell==1){
        return 0;
    }
    
    if (is_shell==1){
        shell_count = shell_count + 1;
    }

    

    if(read_dentry_by_name((uint8_t*)filename,&tar_dentry)==FAIL){//check if it is a file in file system
        return FAIL;
    }

    if(read_data(tar_dentry.inode_num, 0, ELF_buf , LEN_ELF)==FAIL){
        return FAIL;
    }
    if(ELF_buf[0]!=mgc_num0|| ELF_buf[1]!=mgc_num1||ELF_buf[2]!=mgc_num2||ELF_buf[3]!=mgc_num3){// check whether file is excutable
        return FAIL;
    }
    

    for(i=0;i<pcb_array_size;i++){   //map new process to a free pcb position
        if(PCB_array[i]==NULL){
            new_pid=i;//new_pid is the idx of the PCB_array
            break;
        }
    }

    if(new_pid==-1){
        return FAIL;
    }

    set_user_page(new_pid);

    //copy file data to newly allocated page 
    uint32_t length= ((inode*)(inode_men_start+tar_dentry.inode_num))->length;
    //uint8_t data_buf[length];
    if(read_data(tar_dentry.inode_num, 0, (uint8_t*)file_data_base_add ,length)==FAIL){
        return FAIL;
    }
    //memcpy((void*)file_data_base_add, (const void*)data_buf,length );

    //create new PCB
    create_new_PCB(arg,new_pid);


    //update current PCB;
    current_PCB=PCB_array[new_pid];



    //save process information to current_PCB
    current_PCB->pid=new_pid;
    current_PCB->parent_pid=terminals[new_pid].running_pid;
    //update terminal's running_pid
    terminals[new_pid].running_pid = new_pid;
    current_PCB->tss_esp0=(uint32_t)current_PCB +KERNAL_STACK_SIZE-avoid_page_fault_fence;
    if(is_shell == 1){
        current_PCB->shell_indicator=1;
    }
    else{
        current_PCB->shell_indicator=0;
    }
    


    //prepare for "context switch"
    uint32_t SS=USER_DS;
    uint32_t ESP=user_page_start_address + user_stack_size -avoid_page_fault_fence;//bottom of virtual address
    //EFLAG
    uint32_t CS=USER_CS;
    uint32_t return_address=*(uint32_t*)(((int8_t*)file_data_base_add) +24);

    //create fake "iret stack structure" and change tss
    tss.esp0 = (uint32_t)current_PCB +KERNAL_STACK_SIZE -avoid_page_fault_fence;
    tss.ss0 = KERNEL_DS;


    //save old esp,ebp
    asm volatile("movl %%esp,%%eax;"
        : "=a"(current_PCB->esp)
        :
        : "memory");

    asm volatile("movl %%ebp,%%eax;" 
        : "=a"(current_PCB->ebp)
        :
        : "memory");

        
    sti(); //enable interupt

    asm volatile(
            "movw  %%ax, %%ds;"
            "pushl %%eax;"
            "pushl %%ebx;"
            "pushfl  ;"
            "pushl %%ecx;"
            "pushl %%edx;"
            "IRET"
            :
            : "a"(SS), "b"(ESP), "c"(CS), "d"(return_address)
            : "cc", "memory"
        );

    return 0;
}







void process_switch(void){

    terminals[current_terminal_number].cursor_x = screen_x;
    terminals[current_terminal_number].cursor_y = screen_y;
    if(PCB_array[0] == NULL || PCB_array[1] == NULL || PCB_array[2] == NULL ){
        if(current_PCB!=NULL){
            asm volatile("movl %%esp, %0":"=r" (current_PCB->cur_esp));
            asm volatile("movl %%ebp, %0":"=r" (current_PCB->cur_ebp));    
        }
        init_shells((uint8_t*)"shell"); 
    }


    uint32_t next_running_terminal = (scheduled_index + 1)%3; 
    uint32_t next_pid = terminals[next_running_terminal].running_pid;
    uint32_t cur_pid = current_PCB->pid;
    PCB* cur_process = PCB_array[cur_pid];
    PCB* next_process = PCB_array[next_pid];

    scheduled_index = (scheduled_index + 1) % 3;
    current_PCB = PCB_array[next_pid];
    //save cur process's esp, ebp
    asm volatile("movl %%esp, %0":"=r" (cur_process->cur_esp));
    asm volatile("movl %%ebp, %0":"=r" (cur_process->cur_ebp));
  

    //change TSS's esp0
    tss.esp0 = (uint32_t)next_process +KERNAL_STACK_SIZE-avoid_page_fault_fence;
    set_user_page(next_pid);
    
    //load next_process's esp ebp
    asm volatile ("         \n\
        movl %0, %%esp      \n\
        movl %1, %%ebp      \n\
        "
        :
        : "r" (next_process->cur_esp), "r" (next_process->cur_ebp)
        : "memory"
    );  


//change EIP：
    asm volatile(
        "leave  ;"
        "ret    ;"
    );
    
}



