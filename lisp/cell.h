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
  /*
  union {
    char atom_sym_name[7];
    int16_t atom_int_val;
    struct {
      cell_addr car; // uint16_t
      cell_addr cdr; // uint16_t
    } cons_cell;
    struct {
      cell_addr args; // uint16_t
      cell_addr code; // uint16_t
      cell_addr env;  // uint16_t
    } lambda_cell;
    void * primop_cell;  
  };
  */
} cell;


int cell_init ();

int cell_print (cell * x);

int cell_read (cell ** out_y);

int cell_eval (cell ** out_y, cell * x);


#endif
