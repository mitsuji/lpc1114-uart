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


int main ( int argc, char** argv )
{
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

