#include "cell.h"

#include "io.h"
#include "stacka.h"
#include "lexia.h"
#include <string.h>
#include <stdlib.h>


/**

 * 例外発生場所
 * cell_car 
 * cell_cdr
 * cell_read で不正なトークン
 * stacka
 * lexia
 * undefined を定義すれば良さそう

 */


static cell * list_syms;
static cell * list_top_env;

static cell * sym_nil;
static cell * sym_t;
static cell * sym_quote;
static cell * sym_if;
static cell * sym_lambda;
static cell * sym_define;
static cell * sym_setb;
static cell * sym_begin;


static int alloc_sym (cell ** out_y, char * name);
static int alloc_int (cell ** out_y, int16_t v);
static int alloc_cons (cell ** out_y, cell * car, cell * cdr);
static int alloc_lambda (cell ** out_y, cell * args, cell * code, cell * env);
static int alloc_primop (cell ** out_y, void * p);

static int get_car (cell ** out_y, cell * x);
static int get_cdr (cell ** out_y, cell * x);


static cell * to_ptr (cell_addr a)
{
  return (cell *) (stacka_head() + a);
}  

static cell_addr from_ptr (cell * p)
{
  return (cell_addr) ((uint64_t)p - (uint64_t)stacka_head());
}


static int find_sym (cell ** out_y, char * name)
{
  cell * l = list_syms;
  while ( l != sym_nil )
    {
      cell * car;
      get_car(&car,l);
      if (!strcmp(name, car->atom_sym_name))
	{
	  *out_y = l;
	  return 0;
	}
      get_cdr(&l,l);
    }
  
  *out_y = sym_nil;
  return 0;
}

static int regist_sym (cell ** out_y, char * name)
{
  cell * x;
  find_sym (&x, name);
  if ( x != sym_nil )
    {
      get_car(out_y,x);
      return 0;
    }

  alloc_sym(&x,name);
  alloc_cons(&list_syms,x,list_syms);
  *out_y = x;
  return 0;
}


static int extend (cell ** out_y, cell * env, cell * sym, cell * val)
{
  cell * cons;
  alloc_cons(&cons, sym, val);
  return alloc_cons(out_y, cons, env);
}

static int extend_list (cell ** out_y, cell * env, cell * syms, cell * vals)
{
  if (syms == sym_nil )
    {
      *out_y = env;
    }
  else
    {
      cell * symscar, * valscar, * symscdr, * valscdr, * ext;
      get_car(&symscar,syms);
      get_car(&valscar,vals);
      get_cdr(&symscdr,syms);
      get_cdr(&valscdr,vals);
      extend(&ext,env,symscar,valscar);
      return extend_list(out_y, ext, symscdr, valscdr);
    }
}



static int extend_top_env (cell * sym, cell * val)
{
  cell * cons, * cdr, * env;
  alloc_cons (&cons,sym,val);
  get_cdr(&cdr,list_top_env);
  alloc_cons(&env,cons,cdr);
  list_top_env->cons_cell.cdr = from_ptr(env);
  return 0;
}


static int find (cell ** out_y, cell * key, cell * env)
{
  cell * car, * carcar, *cdr;
  
  if(env == sym_nil)
    {
      *out_y = sym_nil;
      return 0;
    }

  get_car(&car,env);
  get_car(&carcar,car);
  if (carcar == key)
    {
      *out_y = car;
      return 0;
    }

  get_cdr(&cdr,env);
  return find(out_y, key, cdr);
}



static int prim_cons (cell ** out_y, cell * args)
{
  cell * car, * cdr, * cdrcar;
  get_car(&car,args);
  get_cdr(&cdr,args);
  get_car(&cdrcar,cdr);
  return alloc_cons (out_y, car, cdrcar);
}

static int prim_car (cell ** out_y, cell * args)
{
  cell * car;
  get_car(&car,args);
  return get_car(out_y,car);
}

static int prim_cdr (cell ** out_y, cell * args)
{
  cell * car;
  get_car(&car,args);
  return get_cdr(out_y,car);
}

static int prim_print (cell ** out_y, cell * args)
{
  while(args != sym_nil)
    {
      cell * car;
      get_car(&car,args);
      cell_print (car);
      io_printf(" ");
      get_cdr(&args,args);
    }
  io_printf("\n");
  *out_y = sym_nil;
  return 0;
}



static int prim_sum(cell ** out_y, cell * args)
{
  int sum = 0;
  while (args != sym_nil)
    {
      cell * car;
      get_car(&car, args);
      sum += car->atom_int_val;
      get_cdr(&args,args);
    }
  alloc_int(out_y, sum);
  return 0;
}

static int prim_sub(cell ** out_y, cell * args)
{
  int sum;
  cell * car;
  get_car(&car,args);
  get_cdr(&args,args);
  sum = car->atom_int_val;
  while (args != sym_nil )
    {
      cell * car;
      get_car(&car, args);
      sum -= car->atom_int_val;
      get_cdr(&args,args);
    }
  alloc_int(out_y, sum);
  return 0;
}

static int prim_prod(cell ** out_y, cell * args)
{
  int prod = 1;
  while (args != sym_nil)
    {
      cell * car;
      get_car(&car, args);
      prod *= car->atom_int_val;
      get_cdr(&args,args);
    }
  alloc_int(out_y, prod);
  return 0;
}

static int prim_div(cell ** out_y, cell * args)
{
  int prod;
  cell * car;
  get_car(&car,args);
  get_cdr(&args,args);
  prod = car->atom_int_val;
  while (args != sym_nil )
    {
      cell * car;
      get_car(&car, args);
      prod /= car->atom_int_val;
      get_cdr(&args,args);
    }
  alloc_int(out_y, prod);
  return 0;
}


static int num_args (int16_t * out_x, int16_t * out_y, cell * args)
{
  cell * car, * cdr, * cdrcar;
  get_car(&car,args);
  get_cdr(&cdr,args);
  get_car(&cdrcar,cdr);

  *out_x = car->atom_int_val;
  *out_y = cdrcar->atom_int_val;
}

static int prim_eq(cell ** out_y, cell * args)
{
  int16_t x,y;
  num_args(&x,&y,args);
  *out_y = x == y ? sym_t : sym_nil;
  return 0;
}

static int prim_gt(cell ** out_y, cell * args)
{
  int16_t x,y;
  num_args(&x,&y,args);
  *out_y = x > y ? sym_t : sym_nil;
  return 0;
}

static int prim_lt(cell ** out_y, cell * args)
{
  int16_t x,y;
  num_args(&x,&y,args);
  *out_y = x < y ? sym_t : sym_nil;
  return 0;
}

static int prim_ge(cell ** out_y, cell * args)
{
  int16_t x,y;
  num_args(&x,&y,args);
  *out_y = x >= y ? sym_t : sym_nil;
  return 0;
}

static int prim_le(cell ** out_y, cell * args)
{
  int16_t x,y;
  num_args(&x,&y,args);
  *out_y = x <= y ? sym_t : sym_nil;
  return 0;
}


static int alloc_sym (cell ** out_y, char * name)
{
  cell * r;
  //  int cell_size = sizeof(sym_type) + 7; // 8 byte
  int cell_size = sizeof(cell);
  stacka_malloc((char **)&r, cell_size);

  r->type = ATOM_SYM;
  strcpy(r->atom_sym_name, name);
  *out_y = r;
  return 0;
}

static int alloc_int (cell ** out_y, int16_t v)
{
  cell * r;
  //  int cell_size = sizeof(sym_type) + sizeof(int16_t); // 3 byte
  int cell_size = sizeof(cell);
  stacka_malloc((char **)&r, cell_size);

  r->type = ATOM_INT;
  r->atom_int_val = v;
  *out_y = r;
  return 0;
}

static int alloc_cons (cell ** out_y, cell * car, cell * cdr)
{
  cell * r;
  //  int cell_size = sizeof(sym_type) + (sizeof(uint16_t) *2); // 5 byte
  int cell_size = sizeof(cell);
  stacka_malloc((char **)&r, cell_size);

  r->type = CONS;
  r->cons_cell.car = from_ptr(car);
  r->cons_cell.cdr = from_ptr(cdr);
  *out_y = r;
  return 0;
}

static int alloc_lambda (cell ** out_y, cell * args, cell * code, cell * env)
{
  cell * r;
  //  int cell_size = sizeof(sym_type) + (sizeof(uint16_t) *3); // 7 byte
  int cell_size = sizeof(cell);
  stacka_malloc((char **)&r, cell_size);

  r->type = LAMBDA;
  r->lambda_cell.args = from_ptr(args);
  r->lambda_cell.code = from_ptr(code);
  r->lambda_cell.env  = from_ptr(env);
  *out_y = r;
  return 0;
}

static int alloc_primop (cell ** out_y, void * p)
{
  cell * r;
  //  int cell_size = sizeof(sym_type) + (sizeof(void *)); // 9(amd64) or 5(arm32) byte
  int cell_size = sizeof(cell);
  stacka_malloc((char **)&r, cell_size);

  r->type = PRIMOP;
  r->primop_cell = p;
  *out_y = r;
  return 0;
}



static int get_car (cell ** out_y, cell * x)
{
  if(x == 0)
    {
      *out_y = sym_nil;
      //      io_printf("warning: car argument null on line %d\n", 0);
      return 0x00000001;
    }
  
  if(x == sym_nil)
    {
      *out_y = sym_nil;
      return 0;
    }
  
  if(x->type != CONS)
    {
      *out_y = sym_nil;
      //      io_printf("warning: car argument not a list on line %d\n", 0);
      return 0x00000002;
    }

  *out_y = to_ptr(x->cons_cell.car);
  return 0;
}

static int get_cdr (cell **out_y, cell * x)
{
  if(x == sym_nil)
    {
      *out_y = sym_nil;
      return 0;
    }
  
  if(x->type != CONS)
    {
      //      io_printf("warning: cdr argument not a list on line %d\n", 0); 
      *out_y = sym_nil;
      return 0x00000003;
    }
  
  if(to_ptr(x->cons_cell.cdr) == 0)
    {
      //      io_printf("error: cdr list element is zero-pointer at %d\n", 0);
      *out_y = sym_nil;
      return 0x00000004;
    }
  
  *out_y = to_ptr(x->cons_cell.cdr);
  return 0;
}


int cell_init ()
{
  // sym_nil
  alloc_sym(&sym_nil,"nil");
  
  // list_syms
  alloc_cons(&list_syms,sym_nil,sym_nil);

    
  { // list_top_env
    cell * cons;
    alloc_cons(&cons,sym_nil,sym_nil);
    alloc_cons(&list_top_env,cons,sym_nil);
  }
  
  { // sym_t
    regist_sym(&sym_t,"t");
    extend_top_env (sym_t, sym_t);
  }
  
  regist_sym(&sym_quote,"quote");
  regist_sym(&sym_if,"if");
  regist_sym(&sym_lambda,"lambda");
  regist_sym(&sym_define,"define");
  regist_sym(&sym_setb,"set!");
  regist_sym(&sym_begin,"begin");

  
  { // cons
    cell * sym, * pop;
    regist_sym(&sym, "cons");
    alloc_primop(&pop, prim_cons);
    extend_top_env(sym, pop);
  }
  
  { // car
    cell * sym, * pop;
    regist_sym(&sym, "car");
    alloc_primop(&pop, prim_car);
    extend_top_env(sym, pop);
  }
  
  { // cdr
    cell * sym, * pop;
    regist_sym(&sym, "cdr");
    alloc_primop(&pop, prim_cdr);
    extend_top_env(sym, pop);
  }
  
  { // print
    cell * sym, * pop;
    regist_sym(&sym, "print");
    alloc_primop(&pop, prim_print);
    extend_top_env(sym, pop);
  }
  
  { // +
    cell * sym, * pop;
    regist_sym(&sym, "+");
    alloc_primop(&pop, prim_sum);
    extend_top_env(sym, pop);
  }
  
  { // -
    cell * sym, * pop;
    regist_sym(&sym, "-");
    alloc_primop(&pop, prim_sub);
    extend_top_env(sym, pop);
  }
  
  { // *
    cell * sym, * pop;
    regist_sym(&sym, "*");
    alloc_primop(&pop, prim_prod);
    extend_top_env(sym, pop);
  }
  
  { // /
    cell * sym, * pop;
    regist_sym(&sym, "/");
    alloc_primop(&pop, prim_div);
    extend_top_env(sym, pop);
  }

  { // =
    cell * sym, * pop;
    regist_sym(&sym, "=");
    alloc_primop(&pop, prim_eq);
    extend_top_env(sym, pop);
  }

  { // >
    cell * sym, * pop;
    regist_sym(&sym, ">");
    alloc_primop(&pop, prim_gt);
    extend_top_env(sym, pop);
  }

  { // <
    cell * sym, * pop;
    regist_sym(&sym, "<");
    alloc_primop(&pop, prim_lt);
    extend_top_env(sym, pop);
  }

  { // >=
    cell * sym, * pop;
    regist_sym(&sym, ">=");
    alloc_primop(&pop, prim_ge);
    extend_top_env(sym, pop);
  }

  { // <=
    cell * sym, * pop;
    regist_sym(&sym, "<=");
    alloc_primop(&pop, prim_le);
    extend_top_env(sym, pop);
  }

  lexia_init();
}



static int read_raw(cell ** out_y, char * token);
static int read_list(cell ** out_y);

int cell_read(cell ** out_y)
{
  char * token;
  lexia_get_token(&token);
  return read_raw(out_y, token);
}

static int read_raw(cell ** out_y, char * token)
{
  if(!strcmp(token, "("))
    {
      read_list(out_y);
      return 0;
    }
  else if(!strcmp(token, "\'"))
    {
      cell * tmp;
      cell_read(&tmp);
      cell * cons;
      alloc_cons(&cons,tmp,sym_nil);
      alloc_cons(out_y,sym_quote,cons);
      return 0;
    }
  else if(0
	  || token[strspn(token, "0123456789")] == '\0'
	  || (token[0] == '-' && strlen(token) > 1) )
    {
      alloc_int(out_y,atoi(token));
      return 0;
    }
  else
    {
      regist_sym(out_y,token);
      return 0;
    }
}

static int read_list(cell ** out_y)
{
  char * token;
  lexia_get_token(&token);
  
  if(!strcmp(token, ")"))
    {
      *out_y = sym_nil;
      return 0;
    }
  else if(!strcmp(token, "."))
    {
      cell * tmp;
      char * end_token;
      cell_read(&tmp);
      lexia_get_token(&end_token);
      if(strcmp(end_token, ")"))
	{
	  return 0x00000001;
	}
      else
	{
	  *out_y = tmp;
	  return 0;
	}
    }
  else
    {
      cell * tmp;
      cell * cons;
      read_raw(&tmp,token);
      read_list(&cons);
      alloc_cons(out_y, tmp, cons);
      return 0;
    }
}


typedef int (*primop)(cell **, cell *);

static int eval_list (cell ** out_y, cell * exps, cell * env);

static int eval_raw (cell ** out_y, cell * exp, cell * env)
{
  eval_start:

  if(exp == sym_nil)
    {
      *out_y = sym_nil;
      return 0;
    }

  switch(exp->type) {
    case ATOM_INT:
      *out_y = exp;
      return 0;

    case ATOM_SYM:
      {
	cell * x;
	find(&x, exp, env);
	if (x == sym_nil)
	  {
	    io_printf ("Unbound symbol ");
	    cell_print (exp);
	    io_printf ("\n");
	    *out_y = sym_nil;
	  }
	return get_cdr(out_y,x);
      }

    case CONS:
      {
	cell * car;
	get_car (&car,exp);

	if (car == sym_if)
	  {
	    cell * cdr, * cdrcar, * ecdrcar;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar, cdr);
	    eval_raw(&ecdrcar, cdrcar, env);
	    if ( ecdrcar != sym_nil )
	      {
		cell * tmp = exp;
		get_cdr(&tmp,tmp);get_cdr(&tmp,tmp);get_car(&tmp,tmp);
		return eval_raw(out_y,tmp,env);
	      }
	    else
	      {
		cell * tmp = exp;
		get_cdr(&tmp,tmp);get_cdr(&tmp,tmp);get_cdr(&tmp,tmp);get_car(&tmp,tmp);
		return eval_raw(out_y,tmp,env);
	      }
	  }
	else if (car == sym_lambda)
	  {
	    cell * cdr, * cdrcar, * cdrcdr;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar,cdr);
	    get_cdr(&cdrcdr,cdr);
	    return alloc_lambda(out_y, cdrcar, cdrcdr, env);
	  }
	else if(car == sym_define)
	  {
	    cell * cdr, * cdrcar, * cdrcdr, * cdrcdrcar;
	    cell * ecdrcdrcar;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar,cdr);
	    get_cdr(&cdrcdr,cdr);
	    get_car(&cdrcdrcar,cdrcdr);
	    eval_raw(&ecdrcdrcar,cdrcdrcar,env);
	    extend_top_env(cdrcar,ecdrcdrcar);
	    *out_y = ecdrcdrcar;
	    return 0;
	  }
	else if(car == sym_setb)
	  {
	    cell * cdr, * cdrcar, * cdrcdr, * cdrcdrcar;
	    cell * x, * ecdrcdrcar;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar,cdr);
	    get_cdr(&cdrcdr,cdr);
	    get_car(&cdrcdrcar,cdrcdr);
	    find(&x,cdrcar,env);
	    eval_raw(&ecdrcdrcar,cdrcdrcar,env);
	    x->cons_cell.cdr = from_ptr(ecdrcdrcar);
	    *out_y = ecdrcdrcar;
	    return 0;
	  }
	else if (car == sym_quote)
	  {
	    cell * cdr;
	    get_cdr(&cdr,exp);
	    return get_car(out_y,cdr);
	  }
	else if (car == sym_begin)
	  {
	    get_cdr(&exp,exp);
	    if(exp == sym_nil)
	      {
		*out_y = sym_nil;
		return 0;
	      }
	    for(;;)
	      {
		cell * cdr;
		get_cdr(&cdr,exp);
		if (cdr == sym_nil)
		  {
		    get_car(&exp,exp);
		    goto eval_start;
		  }
		cell * car, * ecar;
		get_car(&car,exp);
		eval_raw(&ecar,car,env);
		get_cdr(&exp,exp);
	      }
	  }
	else
	  {
	    cell * cdr, * proc, * args;
	    get_cdr(&cdr,exp);
	    eval_raw(&proc, car, env);
	    eval_list(&args, cdr, env);

	    if (proc->type == LAMBDA )
	      {
		cell * lmdargs, * lmdcode, * lmdenv ;
		lmdargs = to_ptr(proc->lambda_cell.args);
		lmdcode = to_ptr(proc->lambda_cell.code);
		lmdenv  = to_ptr(proc->lambda_cell.env);
		extend_list(&env, lmdenv, lmdargs, args);
		alloc_cons(&exp,sym_begin,lmdcode);
		goto eval_start;
	      }
	    else if ( proc->type == PRIMOP )
	      {
		((primop) proc->primop_cell)(out_y, args);
		return 0;
	      }
	    else
	      {
		io_printf("Bad PROC type: %d\n", proc->type);
		*out_y = sym_nil;
		return 0;
	      }
	  }
      }

    case LAMBDA:
      *out_y = exp;
      return 0;
      
    case PRIMOP:
      *out_y = exp;
      return 0;
      
  }

  *out_y = exp;
  return 0;
}

static int eval_list (cell ** out_y, cell * exps, cell * env)
{
  if(exps == sym_nil)
    {
      *out_y = sym_nil;
      return 0;
    }

  cell * car, * cdr, * ecar, * ecdr;
  get_car(&car, exps);
  get_cdr(&cdr, exps);
  eval_raw(&ecar, car, env);
  eval_list(&ecdr, cdr, env);
  return alloc_cons(out_y, ecar,ecdr);
}

int cell_eval (cell ** out_y, cell * x)
{
  return eval_raw(out_y, x, list_top_env);
}


int cell_print (cell * x)
{
  switch(x->type)
    {
    case ATOM_SYM:
      if(x == sym_nil)
	{
	  io_printf("()");
	}
      else
	{
	  io_printf("%s", x->atom_sym_name);
	}
      break;
      
    case ATOM_INT:
      io_printf("%d", x->atom_int_val);
      break;
      
    case CONS: 
      io_printf("(");
      while(1)
	{
	  cell * car, * cdr;
	  get_car(&car,x);
	  get_cdr(&cdr,x);
	  cell_print(car);
	  if(cdr == sym_nil)
	    {
	      io_printf(")");
	      break;
	    }
	  x = cdr;
	  if(x->type != CONS)
	    {
	      io_printf(" . ");
	      cell_print(x);
	      io_printf(")");
	      break;
	    }
	  io_printf(" ");
	}
      break;
      
    case LAMBDA:
      io_printf("#<LAMBDA>");
      break;
      
    case PRIMOP:
      io_printf("#<PRIMOP>");
      break;

    default:
      break;
      
    }
  
  return 0;
}


