#include <stdlib.h>

#include "io.h"
#include "stacka.h"
#include "cell.h"



void test_cell();

int main ( int argc, char** argv )
{
  test_cell();
}

void test_cell()
{
  char * head = malloc(4096);
  stacka_init(head,4096);
  
  cell_init();
#ifdef  __STACKA_DEBUG_
  io_printf("__STACKA_DEBUG_[init]: stackused %dbyte\n", stacka_stackused());
#endif

  while(1){
    cell * r, * e;
    if ( cell_read(&r) == CELL_READ_EOF )
      {
	exit(0);
      }

#ifdef  __STACKA_DEBUG_
    io_printf("__STACKA_DEBUG_[read]: stackused %dbyte\n", stacka_stackused());
#endif

    cell_eval(&e,r);
#ifdef  __STACKA_DEBUG_
    io_printf("__STACKA_DEBUG_[eval]: stackused %dbyte\n", stacka_stackused());
#endif

    cell_print(e); io_printf("\n");
#ifdef  __STACKA_DEBUG_
    io_printf("__STACKA_DEBUG_[print]: stackused %dbyte\n", stacka_stackused());
#endif

  }
  
  stacka_free();
  free(head);
}


