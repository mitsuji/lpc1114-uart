#include <stdlib.h>

#include "io.h"
#include "stacka.h"
#include "cell.h"

#include "allocstr.h"


void test_allocstr();
void test_cell();

int main ( int argc, char** argv )
{
  test_allocstr();
  //  test_cell();
}


void test_allocstr_print(uint16_t * strs, int strs_len);
void test_allocstr_free(uint16_t * strs, int strs_len, uint16_t str);
void test_allocstr()
{
  char * head = malloc(2048);
  allocstr_init(head,2048);

  uint16_t strs[5];
  uint16_t sf;
  
  allocstr_alloc(&strs[0], "Alpha");
  allocstr_alloc(&strs[1], "Bravo");
  allocstr_alloc(&strs[2], "Charlie");
  allocstr_alloc(&strs[3], "Echo");
  allocstr_alloc(&strs[4], "Foxtrot");
  test_allocstr_print(strs, (sizeof(strs)/sizeof(uint16_t)));
  
  test_allocstr_free(strs, (sizeof(strs)/sizeof(uint16_t)), strs[1]);
  test_allocstr_print(strs, (sizeof(strs)/sizeof(uint16_t)));
  
  allocstr_alloc(&strs[1], "November");
  test_allocstr_print(strs, (sizeof(strs)/sizeof(uint16_t)));
  
  test_allocstr_free(strs,(sizeof(strs)/sizeof(uint16_t)), strs[1]);
  test_allocstr_print(strs, (sizeof(strs)/sizeof(uint16_t)));

  allocstr_alloc(&strs[3], "Uniform");
  test_allocstr_print(strs, (sizeof(strs)/sizeof(uint16_t)));
  
}

void test_allocstr_print(uint16_t * strs, int strs_len)
{
  for (int i=0; i < strs_len; ++i)
    {
      io_printf("strs[%d]: %d, %s\n", i,strs[i],allocstr_to_ptr(strs[i]));
    }
}

void test_allocstr_free(uint16_t * strs, int strs_len, uint16_t str)
{
  uint16_t sf;
  allocstr_free(&sf, str);
  io_printf("free shift: %d\n",sf);
  for (int i=0; i < strs_len; ++i)
    {
      if (strs[i] > str)
	{
	  strs[i] -= sf ;
	}
    }
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


