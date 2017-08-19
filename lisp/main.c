#include <stdlib.h>

#include "io.h"
#include "stacka.h"
#include "cell.h"
#include "lexia.h"


// define, set! したもの以外は eval,print後 消してOK
// cons sell をたどって free ?


void myexit(int code);


void test_cell();
void test_lexia();




struct s1
{
  cell_type type;
  char data[1];  
};


typedef struct _cell_atom_sym
{
  cell_type type;
  char name[7];
} cell_atom_sym;

typedef struct _cell_atom_int
{
  cell_type type;
  int16_t val;
} cell_atom_int;

typedef struct _cell_cons
{
  cell_type type;
  cell_addr car; // uint16_t
  cell_addr cdr; // uint16_t
} cell_cons;

typedef struct _cell_lambda
{
  cell_type type;
  cell_addr args; // uint16_t
  cell_addr code; // uint16_t
  cell_addr env;  // uint16_t
} cell_lambda;

typedef struct _cell_primop
{
  cell_type type;
  void * p;  
} cell_primop;

/*
typedef struct _data_cons
{
  cell_addr car; // uint16_t
  cell_addr cdr; // uint16_t
} data_cons;

typedef struct _data_lambda
{
  cell_addr args; // uint16_t
  cell_addr code; // uint16_t
  cell_addr env;  // uint16_t
} data_lambda;
*/





int main ( int argc, char** argv )
{
  /*
  io_printf("%d\n",sizeof(struct s1));
  io_printf("%d\n",sizeof(cell_atom_sym));
  io_printf("%d\n",sizeof(cell_atom_int));
  io_printf("%d\n",sizeof(cell_cons));
  io_printf("%d\n",sizeof(cell_lambda));
  io_printf("%d\n",sizeof(cell_primop));
  */
  //  io_printf("%d\n",sizeof(data_cons));
  //  io_printf("%d\n",sizeof(data_lambda));
  test_cell();
  //  test_lexia();
}


void myexit(int code) {
  io_printf("%d bytes left hanging\n", 0);
  exit(code);
}


void test_cell()
{
  char * head = malloc(4096);
  stacka_init(head,4096);
  
  cell_init();
  while(1){
    cell * c, * e;
    cell_read(&c);
    cell_eval(&e,c);
    cell_print(e);
    io_printf("\n");
  }
  
  stacka_free();
  free(head);
}


void test_lexia()
{
  while(1)
    {
      int ecode;
      char * token;
      if ( ecode = lexia_get_token(&token) == 0 )
	{
	  io_printf("%s\n",token);
	}
      else
	{
	  myexit(ecode);
	}
    }
}

