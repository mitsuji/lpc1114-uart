#ifndef __LISP_STACKA_H_
#define __LISP_STACKA_H_

#define STACKA_STACK_OVERFLOW 0x00010001

int stacka_init (char * head, int size);
char * stacka_head ();
int stacka_malloc (char ** out_addr, int size);
int stacka_free ();


#endif
