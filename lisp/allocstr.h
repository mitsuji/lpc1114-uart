#ifndef __LISP_ALLOCSTR_H_
#define __LISP_ALLOCSTR_H_

#include <stdint.h>

#define ALLOCSTR_STACK_OVERFLOW 0x00040001
#define ALLOCSTR_BAD_ADDRESS    0x00040002

int allocstr_init(char* head, int size);

int allocstr_alloc(uint16_t* out_addr, char * str);

int allocstr_free(uint16_t* out_shift, uint16_t addr);

char * allocstr_to_ptr(uint16_t addr);



#endif

