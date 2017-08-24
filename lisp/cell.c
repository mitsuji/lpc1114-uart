#include "cell.h"

#include "io.h"
#include "stacka.h"
#include "lexia.h"
#include <string.h>
#include <stdlib.h>


/**

 ** 例外発生場所
 * cell_car 
 * cell_cdr
 * cell_read で不正なトークン
 * stacka
 * lexia
 * undefined を定義すれば良さそう

 ** メモリ確保場所
 * Init時
 ** list_syms, list_top_env
 ** 各symbol
 ** 各primop
 * Read時
 * Eval時
 ** lambda で始まるconsを評価するとき(lambda: 1)
 ** define で始まるconsを評価するとき(extend_top_env(cons:2))
 ** lambda を評価するとき(extend_list(cons:2) cons:1)
 ** primop を評価するとき(戻り値用に色々)
 ** list を評価するとき (cons:1)


 */

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

typedef int (*primop)(cell **, cell *);

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
static int alloc_primop (cell ** out_y, primop p);

static int get_car (cell ** out_y, cell * x);
static int get_cdr (cell ** out_y, cell * x);


static cell * to_ptr (cell_addr a)
{
  return (cell *) (stacka_head() + a);
}  

static cell_addr from_ptr (cell * p)
{
  return (cell_addr) ((uint32_t)p - (uint32_t)stacka_head());
}


static int find_sym (cell ** out_y, char * name)
{
  cell * l = list_syms;
  while ( l != sym_nil )
    {
      cell * car;
      get_car(&car,l);
      if (!strcmp(name, car->data))
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

static int extend_top_env (cell * sym, cell * val)
{
  cell * cons, * cdr, * env;
  data_cons * dp;
  alloc_cons (&cons,sym,val);
  get_cdr(&cdr,list_top_env);
  alloc_cons(&env,cons,cdr);
  dp = (data_cons *)(list_top_env->data);
  dp->cdr = from_ptr(env);
  return 0;
}

static int regist_primop (char * name, primop p)
{
  cell * sym, * pop;
  regist_sym(&sym, name);
  alloc_primop(&pop, p);
  extend_top_env(sym, pop);
}



static int alloc_sym (cell ** out_y, char * name)
{
  cell * r;
  //  int cell_size = sizeof(cell);
  int cell_size = sizeof(cell_type) + 7; // 8 byte
  stacka_malloc((char **)&r, cell_size);
#ifdef  __STACKA_DEBUG_
  io_printf ("__STACKA_DEBUG_[alloc_sym]: %dbyte\n", cell_size);
#endif

  r->type = ATOM_SYM;
  strcpy(r->data, name);
  *out_y = r;
  return 0;
}

static int alloc_int (cell ** out_y, int16_t v)
{
  cell * r;
  int16_t * dp;
  //  int cell_size = sizeof(cell);
  int cell_size = sizeof(cell_type) + sizeof(int16_t); // 3 byte
  stacka_malloc((char **)&r, cell_size);
#ifdef  __STACKA_DEBUG_
  io_printf ("__STACKA_DEBUG_[alloc_int]: %dbyte\n", cell_size);
#endif

  r->type = ATOM_INT;
  dp = (int16_t *)(r->data);
  *dp = v;
  *out_y = r;
  return 0;
}

static int alloc_cons (cell ** out_y, cell * car, cell * cdr)
{
  cell * r;
  data_cons * dp;
  //  int cell_size = sizeof(cell);
  int cell_size = sizeof(cell_type) + sizeof(data_cons); // 5 byte
  stacka_malloc((char **)&r, cell_size);
#ifdef  __STACKA_DEBUG_
  io_printf ("__STACKA_DEBUG_[alloc_cons]: %dbyte\n", cell_size);
#endif

  r->type = CONS;
  dp = (data_cons *) (r->data);
  dp->car = from_ptr(car);
  dp->cdr = from_ptr(cdr);
  *out_y = r;
  return 0;
}

static int alloc_lambda (cell ** out_y, cell * args, cell * code, cell * env)
{
  cell * r;
  data_lambda * dp;
  //  int cell_size = sizeof(cell);
  int cell_size = sizeof(cell_type) + sizeof(data_lambda); // 7 byte
  stacka_malloc((char **)&r, cell_size);
#ifdef  __STACKA_DEBUG_
  io_printf ("__STACKA_DEBUG_[alloc_lambda]: %dbyte\n", cell_size);
#endif

  r->type = LAMBDA;
  dp = (data_lambda *) (r->data);
  dp->args = from_ptr(args);
  dp->code = from_ptr(code);
  dp->env  = from_ptr(env);
  *out_y = r;
  return 0;
}

static int alloc_primop (cell ** out_y, primop p)
{
  cell * r;
  primop * dp;
  //  int cell_size = sizeof(cell);
  int cell_size = sizeof(cell_type) + (sizeof(primop)); // 5 byte
  stacka_malloc((char **)&r, cell_size);
#ifdef  __STACKA_DEBUG_
  io_printf ("__STACKA_DEBUG_[alloc_primop]: %dbyte\n", cell_size);
#endif

  r->type = PRIMOP;
  dp = (primop *) (r->data);
  *dp = p;
  *out_y = r;
  return 0;
}


static int get_car (cell ** out_y, cell * x)
{
  data_cons * dp;
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

  dp = (data_cons *)x->data;
  *out_y = to_ptr(dp->car);
  return 0;
}

static int get_cdr (cell **out_y, cell * x)
{
  data_cons * dp;
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
  
  dp = (data_cons *)x->data;
  
  if(to_ptr(dp->cdr) == 0)
    {
      //      io_printf("error: cdr list element is zero-pointer at %d\n", 0);
      *out_y = sym_nil;
      return 0x00000004;
    }
  
  *out_y = to_ptr(dp->cdr);
  return 0;
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



static int eval_raw (cell ** out_y, cell * exp, cell * env);
static int eval_list (cell ** out_y, cell * exps, cell * env);

int cell_eval (cell ** out_y, cell * x)
{
  return eval_raw(out_y, x, list_top_env);
}

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
#ifdef __CELL_DEBUG_
	    io_printf("__CELL_DEBUG_[lambda]: "); cell_print(exp); io_printf("\n");
#endif
	    cell * cdr, * cdrcar, * cdrcdr;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar,cdr);
	    get_cdr(&cdrcdr,cdr);
	    return alloc_lambda(out_y, cdrcar, cdrcdr, env);
	  }
	else if(car == sym_define)
	  {
#ifdef __CELL_DEBUG_
	    io_printf("__CELL_DEBUG_[define]: "); cell_print(exp); io_printf("\n");
#endif
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
	    data_cons * dp;
	    get_cdr(&cdr,exp);
	    get_car(&cdrcar,cdr);
	    get_cdr(&cdrcdr,cdr);
	    get_car(&cdrcdrcar,cdrcdr);
	    find(&x,cdrcar,env);
	    eval_raw(&ecdrcdrcar,cdrcdrcar,env);
	    dp = (data_cons *) x->data;
	    dp->cdr = from_ptr(ecdrcdrcar);
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
#ifdef __CELL_DEBUG_
	    io_printf("__CELL_DEBUG_[cons]: "); cell_print(exp); io_printf("\n");
#endif
	    cell * cdr, * proc, * args;
	    get_cdr(&cdr,exp);
	    eval_raw(&proc, car, env);
	    eval_list(&args, cdr, env);

	    if (proc->type == LAMBDA )
	      {
		cell * lmdargs, * lmdcode, * lmdenv ;
		data_lambda * dp;
		dp = (data_lambda *)proc->data;
		lmdargs = to_ptr(dp->args);
		lmdcode = to_ptr(dp->code);
		lmdenv  = to_ptr(dp->env);
		extend_list(&env, lmdenv, lmdargs, args);
		alloc_cons(&exp,sym_begin,lmdcode);
		goto eval_start;
	      }
	    else if ( proc->type == PRIMOP )
	      {
		((primop) *((void **)proc->data))(out_y, args);
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
#ifdef __CELL_DEBUG_
	    io_printf("__CELL_DEBUG_[list]: "); cell_print(exps); io_printf("\n");
#endif
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
	  io_printf("%s", x->data);
	}
      break;
      
    case ATOM_INT:
      io_printf("%d", *((int16_t *)x->data));
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


static int prim_cons (cell ** out_y, cell * args);
static int prim_car (cell ** out_y, cell * args);
static int prim_cdr (cell ** out_y, cell * args);
static int prim_atom (cell ** out_y, cell * args);
static int prim_not (cell ** out_y, cell * args);
static int prim_eq_sym (cell ** out_y, cell * args);

static int prim_print (cell ** out_y, cell * args);

static int prim_add(cell ** out_y, cell * args);
static int prim_sub(cell ** out_y, cell * args);
static int prim_mul(cell ** out_y, cell * args);
static int prim_div(cell ** out_y, cell * args);
static int prim_mod(cell ** out_y, cell * args);

static int prim_eq_int(cell ** out_y, cell * args);
static int prim_ne_int(cell ** out_y, cell * args);
static int prim_gt(cell ** out_y, cell * args);
static int prim_lt(cell ** out_y, cell * args);
static int prim_ge(cell ** out_y, cell * args);
static int prim_le(cell ** out_y, cell * args);


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

  regist_primop ("cons", prim_cons);
  regist_primop ("car", prim_car);
  regist_primop ("cdr", prim_cdr);
  regist_primop ("atom", prim_atom);
  regist_primop ("not", prim_not);
  regist_primop ("eq", prim_eq_sym);

  regist_primop ("print", prim_print);
  
  regist_primop ("+", prim_add);
  regist_primop ("-", prim_sub);
  regist_primop ("*", prim_mul);
  regist_primop ("/", prim_div);
  regist_primop ("%", prim_mod);
  regist_primop ("=", prim_eq_int);
  regist_primop ("!=", prim_ne_int);
  regist_primop (">", prim_gt);
  regist_primop ("<", prim_lt);
  regist_primop (">=", prim_ge);
  regist_primop ("<=", prim_le);
  
  lexia_init();
}


static int get_args (cell ** out_x1, cell ** out_x2, cell * args)
{
  cell * cdr;
  get_car(out_x1,args);
  get_cdr(&cdr,args);
  get_car(out_x2,cdr);
}

static int prim_cons (cell ** out_y, cell * args)
{
  cell * x1, * x2;
  get_args(&x1,&x2,args);

  return alloc_cons (out_y, x1, x2);
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

static int prim_atom (cell ** out_y, cell * args)
{
  cell * car;
  get_car(&car,args);
  
  *out_y =
    ( 0
      || car->type == ATOM_SYM
      || car->type == ATOM_INT
      )
    ? sym_t : sym_nil;
  return 0;
}

static int prim_not (cell ** out_y, cell * args)
{

  cell * car;
  get_car(&car,args);
  
  *out_y = (car != sym_t) ? sym_t : sym_nil;
  return 0;
}

static int prim_eq_sym (cell ** out_y, cell * args)
{
  cell * x1, * x2;
  get_args(&x1,&x2,args);
  
  *out_y = (x1 == x2) ? sym_t : sym_nil;
  return 0;
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


static int prim_add(cell ** out_y, cell * args)
{
  int16_t sum = 0;
  while (args != sym_nil)
    {
      cell * car;
      get_car(&car, args);
      sum += *((int16_t *)car->data);
      get_cdr(&args,args);
    }
  alloc_int(out_y, sum);
  return 0;
}

static int prim_sub(cell ** out_y, cell * args)
{
  int16_t sum;
  cell * car;
  get_car(&car,args);
  get_cdr(&args,args);
  sum = *((int16_t *)car->data);
  while (args != sym_nil )
    {
      cell * car;
      get_car(&car, args);
      sum -= *((int16_t *)car->data);
      get_cdr(&args,args);
    }
  alloc_int(out_y, sum);
  return 0;
}

static int prim_mul(cell ** out_y, cell * args)
{
  int16_t prod = 1;
  while (args != sym_nil)
    {
      cell * car;
      get_car(&car, args);
      prod *= *((int16_t *)car->data);
      get_cdr(&args,args);
    }
  alloc_int(out_y, prod);
  return 0;
}

static int prim_div(cell ** out_y, cell * args)
{
  int16_t prod;
  cell * car;
  get_car(&car,args);
  get_cdr(&args,args);
  prod = *((int16_t *)car->data);
  while (args != sym_nil )
    {
      cell * car;
      get_car(&car, args);
      prod /= *((int16_t *)car->data);
      get_cdr(&args,args);
    }
  alloc_int(out_y, prod);
  return 0;
}

static int prim_mod(cell ** out_y, cell * args)
{
  int16_t prod;
  cell * car;
  get_car(&car,args);
  get_cdr(&args,args);
  prod = *((int16_t *)car->data);
  while (args != sym_nil )
    {
      cell * car;
      get_car(&car, args);
      prod %= *((int16_t *)car->data);
      get_cdr(&args,args);
    }
  alloc_int(out_y, prod);
  return 0;
}


static int get_args_int (int16_t * out_x1, int16_t * out_x2, cell * args)
{
  cell * x1, * x2;
  get_args(&x1,&x2,args);
  
  *out_x1 = *((int16_t *)x1->data);
  *out_x2 = *((int16_t *)x2->data);
}

static int prim_eq_int(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 == x2 ? sym_t : sym_nil;
  return 0;
}

static int prim_ne_int(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 != x2 ? sym_t : sym_nil;
  return 0;
}

static int prim_gt(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 > x2 ? sym_t : sym_nil;
  return 0;
}

static int prim_lt(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 < x2 ? sym_t : sym_nil;
  return 0;
}

static int prim_ge(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 >= x2 ? sym_t : sym_nil;
  return 0;
}

static int prim_le(cell ** out_y, cell * args)
{
  int16_t x1, x2;
  get_args_int(&x1,&x2,args);
  *out_y = x1 <= x2 ? sym_t : sym_nil;
  return 0;
}
