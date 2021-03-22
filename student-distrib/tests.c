#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

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
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}


    

	return result;
}

// add more tests here

int division_test(){
	TEST_HEADER;
	int i;
    int a=0;
    int b=4;
    for(i = 0;i<20;i++){
	b=b/a;
	}

	return FAIL;
}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */





int page_test(){
	TEST_HEADER;
	int i;
	int result = PASS;

	/* test margin of page -- should pass without page fault */
	int val;
	int* ptr = (int*)0x400000;	/* the margin of PD */
	ptr[0] = 1;
	val = ptr[0];

	/* check the value of the entry of the tables */
	// first, check whether the first two exist 
	if (PD[0].k.p == 0 || PD[1].M.p == 0 ){
		printf ("PDE 1 OR 0 NOT PRESENT\n");
		result = FAIL;
	}

	// then, check whether the other not present and present at the video memory
	for(i = 0; i < 1024; i++){				// 1024 is the entry number
		if(PD[i].M.p == 1 && i > 1){
			printf ("PDE wrongly PRESENT\n");
			result = FAIL;
		}
		if(PT[i].p == 1 && i != 0xB8000 >> 12 ){		//  0xB8000 >> 12 is ithe index where the page table should present
			printf ("PTE wrongly PRESENT\n");
			result = FAIL;
		}
		if (PT[i].p == 0 && i == 0xB8000 >> 12){ 		// 0xB8000 >> 12 is ithe index where the page table should present
			printf ("PTE no PRESENT\n");
			result = FAIL;
		} 
	}
	printf ("refresh\n");
	return result;
}













/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
    //  TEST_OUTPUT("division_test", division_test());
    TEST_OUTPUT("page test", page_test());
   
	// launch your tests here
}
