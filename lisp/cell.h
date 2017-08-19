#ifndef __LISP_CELL_H_
#define __LISP_CELL_H_

#include <stdint.h>

typedef uint8_t cell_type;
#define ATOM_SYM 1
#define ATOM_INT 2
#define CONS 4
#define LAMBDA 8
#define PRIMOP 16

typedef uint16_t cell_addr;

typedef struct _cell
{
  cell_type type;
  char data[1];
} cell;


int cell_init ();

int cell_print (cell * x);

int cell_read (cell ** out_y);

int cell_eval (cell ** out_y, cell * x);


#endif
