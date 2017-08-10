#include "stacka.h"

static char * loc_head;
static int loc_stacksize;
static int loc_stackused;

int stacka_init (char * head, int size)
{
  loc_head = head;
  loc_stacksize = size;
  loc_stackused = 0;
  return 0;
}  

char * stacka_head ()
{
  return loc_head;
}

int stacka_malloc (char ** out_addr, int size)
{
  if (size <= loc_stacksize - loc_stackused)
    {
      *out_addr = loc_head + loc_stackused;
      loc_stackused += size;
      return 0;
    }
  else
    {
      return STACKA_STACK_OVERFLOW;
    }
}  

int stacka_free ()
{
  loc_stackused = 0;
  return 0;
}  
