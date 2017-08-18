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
  char data[7];  
};

struct cons
{
  cell_addr car; // uint16_t
  cell_addr cdr; // uint16_t
};

struct lambda
{
  cell_addr args; // uint16_t
  cell_addr code; // uint16_t
  cell_addr env;  // uint16_t
};







int main ( int argc, char** argv )
{
  io_printf("%d\n",sizeof(struct s1));
  io_printf("%d\n",sizeof(int16_t));
  io_printf("%d\n",sizeof(struct cons));
  io_printf("%d\n",sizeof(struct lambda));
  io_printf("%d\n",sizeof(void *));
  //  test_cell();
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

