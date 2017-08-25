#include "allocstr.h"

#include <string.h>

static char * loc_head;
static int loc_stacksize;
static int loc_stackused;


static uint16_t from_ptr(char * ptr)
{
  return ((uint32_t)ptr) - ((uint32_t)loc_head);
}

char * allocstr_to_ptr(uint16_t addr)
{
  return (char *) (loc_head + addr);
}

int allocstr_init(char* head, int size)
{
  head [0] = '\0';
  loc_head = head +1;
  loc_stacksize = size -1;
  loc_stackused = 0;
  return 0;
}

int allocstr_alloc(uint16_t* out_addr, char * str)
{
  int len;
  len = strlen(str);
  
  if ((len +1) <= loc_stacksize - loc_stackused)
    {
      char * ptr = loc_head + loc_stackused;
      strcpy(ptr,str);
      *out_addr = from_ptr(ptr);
      loc_stackused += (len +1);
      return 0;
    }
  else
    {
      return ALLOCSTR_STACK_OVERFLOW;
    }

}

int allocstr_free(uint16_t* out_shift, uint16_t addr)
{
  char * ptr = allocstr_to_ptr(addr);
  if ( *(ptr-1) != '\0' )
    {
      return ALLOCSTR_BAD_ADDRESS;
    }

  *out_shift = strlen(ptr) +1;
  
  int movelen = loc_stackused - (addr + *out_shift);
  memmove (ptr, ptr+(*out_shift), movelen);
  loc_stackused -= *out_shift;

  return 0;
}


