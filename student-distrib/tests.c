


/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0
#define KEYBOARD_IRQ 1

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S, idt.c
 * 
 * Changed by us, add the coverage for the 0x00-0x19 for out idt except 0x15
 */


int idt_test(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 20; i++){
		if(i == 15){
			continue;
		}
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}


/* division Test
 * 
 * Assert the exception of division will happen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S, idt.c
 * 
 */

int division_test(){
	TEST_HEADER;
	int i;
    int a=0;
    int b=4;
    for(i = 0;i<20;i++){
	b=b/a;
	}

	return FAIL; // if the exception happened, we can never get here
}

/* Any exception test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S, idt.c
 * 
 */
void test_any_excp(){
	/* change the following vector between 0-19(0x00-0x13) */ 
	/* to test each corresponding exception handlers */
	asm("int $0x08");
	return PASS
}

/* test_i8259_disable_irq_garbage
 * Inputs: None
 * Outputs: This test should not mask any interrupts when a garbage input is given to disable_irq 
 * Side Effects: None
 * Coverage: garbage input to disable_irq
 * Files: i8259.c
 */
void test_i8259_disable_irq_garbage(){
	/*send a invalid irq_num to disable_irq, nothing should happen*/
	/*The interrupt from keyboard and rtc should still be aceepted*/
	disable_irq(19);
	disable_irq(1234);
	return PASS
}


/* test_i8259_disable_irq
 * Inputs: None
 * Outputs: This test should mask interrupts from keyboard and rtc
 * Side Effects: None
 * Coverage: disable_irq
 * Files: i8259.c
 */
void test_i8259_disable_irq(){
	/*send irq_num of keyboard and rtc and check whether it is masked*/
	disable_irq(rtc_irq_number);
	disable_irq(KEYBOARD_IRQ);
	return PASS
}

/* test_i8259_enable_irq_garbage
 * Inputs: None
 * Outputs: This test should not enable any interrupts when a garbage input is given to enable_irq 
 * Side Effects: None
 * Coverage: garbage input to enable_irq
 * Files: i8259.c
 */
void test_i8259_enable_irq_garbage(){
	/*send garbage input to enable_irq nothing should happend*/
	enable_irq(19);
	enable_irq(1234);
	return PASS
}

/* test_i8259_enable_irq
 * Inputs: None
 * Outputs: This test should enable interrupts from keyboard and rtc
 * Side Effects: None
 * Coverage: enable_irq
 * Files: i8259.c
 */
void test_i8259_enable_irq(){
	/*it should enable the interrupt from the keyboard and rtc after disable_irq is called*/
	enable_irq(rtc_irq_number);
	enable_irq(KEYBOARD_IRQ);
}





/* page_test
 * Inputs: None
 * Outputs: This test should check if page mechanism is work or not.
 * Side Effects: None
 * Files: page.c
 */
int page_test(){
	TEST_HEADER;
	int i;
	int result = PASS;

	/* test margin of page */
	int test;
	int* ptr = (int*)0x400000;	/* the margin of PD */
	ptr[0] = 2;//just a random number
	test = ptr[0];
    int index= 0xB8000 >> 12;//  0xB8000 >> 12 is ithe index of page table 

	
	//check if the first two exist 
	if (PD[0].k.p == 0 || PD[1].M.p == 0 ){
		result = FAIL;
	}
	//check if the other present at the video memory
	for(i = 0; i < 1024; i++){// 1024 is the entry number
		if(PD[i].M.p == 1 && i > 1){
			result = FAIL;
		}
		if(PT[i].p == 1 && i !=index ){		
			result = FAIL;
		}
		if (PT[i].p == 0 && i == index){ 		 
			result = FAIL;
		} 
	}
	return result;
}






/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("division_test", division_test());
	// test_one_excp();
	// test_i8259_disable_irq_garbage();
	//test_i8259_disable_irq();
	// test_i8259_enable_irq_garbage();
	// test_i8259_enable_irq()
    //TEST_OUTPUT("page test", page_test());

}
