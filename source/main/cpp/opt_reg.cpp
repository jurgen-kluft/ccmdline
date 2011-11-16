/************************ register options ********************************
*
*    options are stored in an array,
*    each element of which is a structure
*
*    The members of the structure contain information about
*        1) the variable which is altered
*        2) the type of variable
*        3) a character by which the variable is invoked
*        4) a string by which the variable can be invoked
*        5) the positional value of the option if applicable
*        6) a brief description of the variable's role in the program
*        7) a longer help message
*        8) a flag indicating that an option has been invoked
*        9) a "hook", a function that is called when variable is invoked
*        10) a pointer to an array structure if the option is for an array
*            
*    The structure elements are not assigned directly, but are
*    "registered" with a call to the function optreg().
*        
*    In this file are the routines which access the structure of
*    registered options.
*
*          I) Register options 
*         II) Get number corresponding to name of option
*        III) Print options out
*             a) to a file which can be used as an input command line
*             b) to the screen as part of a usage message
*             c) to the screen as part of an interactive menu
*         IV) Process single delimited and positional options
*/
#include "xbase\x_target.h"
#include "xbase\x_types.h"
#include "xbase\x_string_std.h"

#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_proc.h"
#include "xcmdline\private\opt_util.h"
#include "xcmdline\private\opt_num.h"

namespace xcore
{
	namespace xcmdline
	{
		/* Default is to indicate whether a flag is true or false by using + or -
		* But, by #define'ing FLAG_ZERO_ONE, we will use 1 or 0 instead
		*/

		#ifdef FLAG_ZERO_ONE
			#define FLAG_TRUE '1'
			#define FLAG_FALSE '0'        
		#else
			#define FLAG_TRUE '+'
			#define FLAG_FALSE '-'        
		#endif


		Opt_Proc::Opt_Proc()
			: opt_exit_number(0)		//< if we have to exit(), then exit with this number
			, optlist(NULL)				//< array of options
			, opt_nreg(0)				//< number of registered opts
			, opt_array_delim(',')		//< the delimiter for arrays. (eads)*/
		{

		}

		void	Opt_Proc::opt_free()
		{
			s32 i;
			for (i=0; i<opt_nreg; ++i) 
			{
				OPT_FREE(optlist[i].longname);
				OPT_FREE(optlist[i].help);
				OPT_FREE(optlist[i].descript);
				if (optlist[i].array && optlist[i].type == OPT_STRING)
				{
					char *s = *((char **)(optlist[i].value));
					OPT_FREE(s);
					OPT_FREE(optlist[i].value);
					OPT_FREE(*(optlist[i].array->value));
				}
				OPT_FREE(optlist[i].array);
			}
			OPT_FREE(optlist);
			opt_freestrings();
			opt_nreg=0;
		}

		char*	Opt_Proc::optarray_getstrval( s32 size, void *val, opt_TYPE otype, char delim )
		{
			s32 n;
			s32 len=0;
			char *str_val = NULL; /* string with all values in array 
								  * will be re-allocated for each item
								  */
			/* Convert the array 'val' into a string 'str_val' */
			for ( n = 0; n < size; n++ )
			{
				/* this next line may look a little arcane...
				* 1. Recall val is in real life a pointer to an array, that is type **
				*    but we don't know the type, so we pass it through the argument list
				*    as a void *
				* 2. *(void **)val recasts val to a void ** and then dereferences it
				*    so that it is the pointer that is the array, but its type is
				*    `void *' instead of `type *'
				* 3. (char *)(*(void **)val) recasts this generic `void *' to be
				*    a `char *' because we know what the size of a char is (it's 1)
				*    and we can do arithmetic with it
				* 4. n*optsizeof(otype) is the offset of the n'th element of the array
				* 5. nth_val points to the n'th value in the array and if we were to
				*    recast it as *(type *)nth_val it would be the explicit value.
				*    Of course we don't know type, but we do know otype, and so
				*    we let opt_getstrval() take care of that
				*/
				char *nth_val = (char *)(*(void **)val) + n*optsizeof( otype );
				char *str_nth_val = opt_getstrval( nth_val, otype );
				// cdk:  was:  char str_delim[2] = { delim, '\0' };
				char str_delim[2];
				str_delim[0] = delim;
				str_delim[1] = 0; 

				if ( n == size-1 ) 
					str_delim[0]='\0';  /* no delimiter at the end */

				/* Make room in str_val for the next item */
				len += x_strlen(str_nth_val) + x_strlen(str_delim);
				if (str_val)
				{
					str_val = (char *)Opt_Allocator::get_opt_allocator()->reallocate(str_val, sizeof(char)*(len+1), 4);
				}
				else 
				{
					str_val = (char *)Opt_Allocator::get_opt_allocator()->allocate(sizeof(char)*(len+1), 4);
					str_val[0]='\0';
				}
				x_strcat(str_val, len+1, str_nth_val);
				x_strcat(str_val, len+1, str_delim);
			}
			return str_val;
		}

		/* ------------------------------------------- */
		/* Routine to register each option as an array */
		/* Added by: Damian Eads <dre9227@cs.rit.edu>  */
		/* ------------------------------------------- */

		s32	Opt_Proc::optrega_array(  s32 *size, void *val, opt_TYPE otype, char name, char *longname, char *descript )
		{
#if 0  
			static char **str_val_array=NULL;
			static s32    str_val_array_n=0;

			++str_val_array_n;
			str_val_array = (char **)realloc(str_val_array, str_val_array_n*sizeof(char *));
			str_val_array[str_val_array_n-1] = optarray_getstrval(*size,val,otype,opt_array_delim);

			/* Register the option as an OPT_STRING */
			optrega( &str_val_array[str_val_array_n-1], OPT_STRING, name, longname, descript );
#else
			char **s = (char **)Opt_Allocator::get_opt_allocator()->allocate(sizeof(char *), 4);
			*s = optarray_getstrval(*size,val,otype,opt_array_delim);
			optrega( s, OPT_STRING, name, longname, descript );
#endif     

			/* Warn if inappropriate base_type is specified */
			switch (otype) 
			{
			case OPT_INT:
			case OPT_UINT:
			case OPT_SHORT:
			case OPT_USHORT:
			case OPT_LONG:
			case OPT_ULONG:
			case OPT_FLOAT:
			case OPT_DOUBLE:
			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:
			case OPT_UBYTE:
				break;

			case OPT_STRING:
				break;

			case OPT_CHAR:
			case OPT_UCHAR:
			case OPT_INTLEVEL:
			case OPT_TOGGLE:
			case OPT_BOOL:
			case OPT_NEGTOGGLE:
			case OPT_NEGBOOL:
			case OPT_NUL:
				opt_warn_1("Undefined behavior for %s array", optstrtyp(opt_nreg-1));
				break;

			default:
				Opt_Util::opt_fatal("opt_getstrval: Undefined o-type");
				break;
			}

			/* Allocate an OptArray_t structure */
			optlist[ opt_nreg-1 ].array = (OptArray_t *)Opt_Allocator::get_opt_allocator()->allocate(sizeof(OptArray_t), 4);

			/* Then add array-specific information to the registered option */
			optlist[ opt_nreg-1 ].array->value     = (void **)val;
			optlist[ opt_nreg-1 ].array->size      = size;
			optlist[ opt_nreg-1 ].array->base_type = otype;
			optlist[ opt_nreg-1 ].array->delim     = opt_array_delim;

			return opt_nreg-1;
		}

		s32 Opt_Proc::optreg_array( s32 *size, void *val, opt_TYPE otype, char name, char *descript )
		{
			return optrega_array(size,val,otype,name,NULL,descript);
		}
		s32 Opt_Proc::optregc_array( s32 *size, void *val, opt_TYPE otype, char name ) 
		{
			return optrega_array(size,val,otype,name,NULL,NULL);
		}
		s32 Opt_Proc::optregcb_array( s32 *size, void *val, opt_TYPE otype, char name, char *descript )
		{
			return optrega_array(size,val,otype,name,NULL,descript);
		}
		s32 Opt_Proc::optregs_array( s32 *size, void *val, opt_TYPE otype, char *longname ) 
		{
			return optrega_array(size,val,otype,'\0',longname,longname);
		}
		s32 Opt_Proc::optregsb_array( s32 *size, void *val, opt_TYPE otype, char *longname, char *descript)
		{
			return optrega_array(size,val,otype,'\0',longname,descript);
		}
		s32 Opt_Proc::optregp_array( s32 *size, void *val, opt_TYPE otype, char* longname, char* descript)
		{
			s32 n = optrega_array(size,val,otype,'\0',longname,descript);
			optmode_n(n, OPT_POSITIONAL);
			return n;
		}
		s32 Opt_Proc::optregf_array( s32 *size, void *val, opt_TYPE otype, char name, char* longname, char* descript) 
		{
			s32 n = optrega_array(size,val,otype,name,longname,descript);
			optmode_n(n, OPT_FLEXIBLE);
			return n;
		}


		/* ------------------------------- */
		/* Routine to register each option */
		/* ------------------------------- */

		s32 Opt_Proc::optreg(void *val, opt_TYPE otype, char name, char *descript) 
		{
			return optrega(val,otype,name,NULL,descript);
		}
		s32 Opt_Proc::optregc(void *val, opt_TYPE otype, char name) 
		{
			return optrega(val,otype,name,NULL,NULL);
		}
		s32 Opt_Proc::optregcb(void *val, opt_TYPE otype, char name, char *descript) 
		{
			return optrega(val,otype,name,NULL,descript);
		}
		s32 Opt_Proc::optregs(void *val, opt_TYPE otype, char *longname) 
		{
			return optrega(val,otype,'\0',longname,longname);
		}
		s32 Opt_Proc::optregsb(void *val, opt_TYPE otype, char *longname, char *descript)
		{
			return optrega(val,otype,'\0',longname,descript);
		}
		s32 Opt_Proc::optregp(void *val, opt_TYPE otype, char* longname, char* descript) 
		{
			s32 n = optrega(val, otype, '\0', longname, descript);
			optmode_n(n, OPT_POSITIONAL);
			return n;
		}
		s32 Opt_Proc::optregf(void *val, opt_TYPE otype, char name, char* longname, char* descript)
		{
			s32 n = optrega(val, otype, name, longname, descript);
			optmode_n(n, OPT_FLEXIBLE);
			return n;
		}
		s32 Opt_Proc::optexec(char *longname, OPT_HOOK fcn, char *descript)
		{
			s32 n;
			n = optrega(NULL,OPT_NUL,'\0',longname,descript);
			opthook_n(n,fcn);
			return n;
		}

		s32	Opt_Proc::optrega(void *val, opt_TYPE otype, char name, char *longname, char *descript)
		{
			opt_MODE mode = OPT_DELIMITED;

			/* write a warning if that option name has been used before */
			if( name != '\0' && opt_char_number(name) != -1 )
			{
				opt_warn_1("optrega: Duplicate option name \'%c\'",name);
			}
			if (!ISEMPTYSTRING(longname) && opt_longname_number(longname) != -1) 
			{
				opt_warn_1("optrega: Duplicate option longname \"%s\"",longname);
			}

			/* val points to the memory where the value of the option
			* is maintained.  This cannot be NULL.
			*/
			if ( val == NULL && otype != OPT_NUL ) 
			{
				if (name)
				{
					opt_warn_1("Name: -%c", name); 
				}

				if (longname)
				{
					opt_warn_1("Name: --%s",longname); 
				}

				Opt_Util::opt_fatal("optrega: invalid first argument=NULL\n");
			}

			/* This is probably a mistake to print out a warning message,
			since it will be printed every time the program is run.
			Perhaps it should only be run if the user types 'run --testopt'
			*/
#if 0    
			if ( val != NULL  && opt_val_number(val) != -1)
			{
				opt_warn_1("optrega: This variable [%s] has already been registered", thename);
			}
#endif    

			/* Programmer may optionally signal that an option is to be a
			* positional option by setting the name to '\0' and the longname
			* to NULL or an empty string.
			*/
			if(name=='\0' && ISEMPTYSTRING(longname))
			{
				mode = OPT_POSITIONAL;
			}

			/* Make sure that the longname doesn't have any whitespace */
			if (!ISEMPTYSTRING(longname)) 
			{
				char *t = longname;
				while (*t != '\0') 
				{
					if (x_isspace(*t))
					{
						opt_fatal_1("invalid longname [%s] contains whitespace\n",longname);
					}
					++t;
				}
			}

			/* reallocate another struct for the new opt */
			++opt_nreg;
			if (optlist == NULL)
				optlist = (Option_t *)Opt_Allocator::get_opt_allocator()->allocate(opt_nreg*sizeof(Option_t), 4);
			else
				optlist = (Option_t *)Opt_Allocator::get_opt_allocator()->reallocate(optlist, opt_nreg*sizeof(Option_t), 4);

			/* Having checked for various warnings, now register the options */

			optlist[opt_nreg-1].value         = val;
			optlist[opt_nreg-1].type          = otype;
			optlist[opt_nreg-1].name          = name;
			optlist[opt_nreg-1].longname      = longname ? Opt_Allocator::opt_strdup(longname) : NULL;
			optlist[opt_nreg-1].mode          = mode;
			optlist[opt_nreg-1].descript      = descript ? Opt_Allocator::opt_strdup(descript) : NULL;

			optlist[opt_nreg-1].invoked       = 0;
			optlist[opt_nreg-1].help          = NULL;
			optlist[opt_nreg-1].hook          = NULL;
			optlist[opt_nreg-1].array         = NULL;

			return opt_nreg-1;

		}

		void Opt_Proc::optchar_n(s32 n, char name)
		{
			if (OPT_isvalidnum(n)) 
				optlist[n].name = name;
		}    
		void Opt_Proc::optmode_n(s32 n, opt_MODE mode)
		{
			if (OPT_isvalidnum(n))
			{
				if (!(mode == OPT_POSITIONAL || mode == OPT_DELIMITED || mode == OPT_FLEXIBLE))
					Opt_Util::opt_warning("optmode() was passed an invalid mode");
				optlist[n].mode = mode;
			}
		}
		void Opt_Proc::optdescript_n(s32 n, char *descript)
		{
			if (OPT_isvalidnum(n))
				Opt_Util::opt_setstring(&optlist[n].descript,descript);
		}
		void Opt_Proc::opthelp_n(s32 n, char *help)
		{
			if (OPT_isvalidnum(n)) 
				Opt_Util::opt_setstring(&optlist[n].help,help);
		}
		void Opt_Proc::optlongname_n(s32 n, char *longname)
		{
			if (OPT_isvalidnum(n)) 
				Opt_Util::opt_setstring(&optlist[n].longname,longname);
		}
		void Opt_Proc::opthook_n(s32 n, OPT_HOOK hook)
		{
			if (OPT_isvalidnum(n)) 
				optlist[n].hook = hook;
		}
		s32 Opt_Proc::optinvoked_n(s32 n)
		{
			if (OPT_isvalidnum(n))
				return optlist[n].invoked;
			return 0;
		}

		void Opt_Proc::optchar(void *v,  char name) 
		{ 
			optchar_n( opt_number(v),   name ); 
		}
		void Opt_Proc::opthelp(void *v,  char *help) 
		{ 
			opthelp_n( opt_number(v),  help ); 
		}
		void Opt_Proc::opthook(void *v,  OPT_HOOK hook)
		{
			opthook_n( opt_number(v), hook ); 
		}      
		void Opt_Proc::optlongname(void *v, char *longname)
		{
			optlongname_n( opt_number(v), longname );
		}
		void Opt_Proc::optmode(void* v, opt_MODE mode) 
		{
			optmode_n( opt_number(v), mode );
		}
		void Opt_Proc::optdescript(void *v, char *descript)
		{
			optdescript_n( opt_number(v), descript );
		}
		void Opt_Proc::optarraydelim_n(s32 n, char delim)
		{
			if (OPT_isvalidnum(n) && optlist[n].array)
			{
				if (optlist[n].array->delim != delim)
				{
					optlist[n].array->delim = delim;
					if (*(optlist[n].array->size) > 0)
					{
						OPT_SET_ITH_VALUE(char *,n, optarray_getstrval(*(optlist[n].array->size), optlist[n].array->value, optlist[n].array->base_type, optlist[n].array->delim));
					}

				}
			}
		}  
		void Opt_Proc::optarraydelim(void *v, char delim)
		{
			s32 n;
			if (v == NULL) 
			{
				/* then reset default array delim and all opt delims*/
				opt_array_delim = delim;
				for (n=0; n<opt_nreg; ++n)
				{
					optarraydelim_n(n,delim);
				}
			}
			else 
			{
				/* just reset for the specific option */
				n = opt_number(v);
				optarraydelim_n(n,delim);
			}
		}

		void  Opt_Proc::optExitNumber(s32 n)
		{
			opt_exit_number = n;
		}

		s32	Opt_Proc::opt_number(void *v)
		{
			s32 i;                         /* see which registered option */
			for(i=0; i<opt_nreg; ++i) 
			{
				if (optlist[i].array && v == optlist[i].array->value)
					return i;
				if (!optlist[i].array && v == optlist[i].value ) 
					return i;
			}
			return -1;                     /* to signify not an option */
		}
		s32	Opt_Proc::opt_longname_number(char *s)
		{
			s32 i;                         /* see which registered option */
			if ( ISEMPTYSTRING(s) )
				return -1;

			for(i=0; i<opt_nreg; ++i)
			{
				if ( !ISEMPTYSTRING(optlist[i].longname) ) 
				{
					if (0==x_strcmp(s, optlist[i].longname)) 
					{
						return i;
					}
				}
			}
			return -1;                     /* to signify not an option */
		}

		/* -------------------------------------------- */
		/* Print value of registered option to a string */
		/* without specifying an index. (eads)          */
		/* -------------------------------------------- */

		char*	Opt_Proc::opt_getstrval( void *val, opt_TYPE typ )
		{
			s32 maybe;
			static char stval_buf[80];

			switch( typ ) {
			case OPT_INT:
				opt_snprintf_1(stval_buf,80,"%d", OPT_GETVALUE(s32,val) );
				break;
			case OPT_UINT:
				opt_snprintf_1(stval_buf,80,"%u", OPT_GETVALUE(u32,val) );
				break;
			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:
				opt_snprintf_1(stval_buf,80,"%d", OPT_GETVALUE(s8,val) );
				break;
			case OPT_UBYTE:
				opt_snprintf_1(stval_buf,80,"%u", OPT_GETVALUE(u8,val) );
				break;
			case OPT_SHORT:
				opt_snprintf_1(stval_buf,80,"%d", OPT_GETVALUE(s16,val) );
				break;
			case OPT_USHORT:
				opt_snprintf_1(stval_buf,80,"%u", OPT_GETVALUE(u16,val) );
				break;
			case OPT_LONG:
				opt_snprintf_1(stval_buf,80,"%ld", OPT_GETVALUE(s64,val) );
				break;
			case OPT_ULONG:
				opt_snprintf_1(stval_buf,80,"%lu", OPT_GETVALUE(u64,val) );
				break;
			case OPT_CHAR:
				opt_snprintf_1(stval_buf,80,"%c", OPT_GETVALUE(char,val));
				break;
			case OPT_UCHAR:
				opt_snprintf_1(stval_buf,80,"%c", OPT_GETVALUE(unsigned char,val));
				break;
			case OPT_FLOAT:
				opt_snprintf_1(stval_buf,80,"%g", OPT_GETVALUE(f32,val) );
				break;
			case OPT_DOUBLE:
				opt_snprintf_1(stval_buf,80,"%g", OPT_GETVALUE(f64,val) );
				break;
			case OPT_INTLEVEL:
				if (1)
				{
					s32 v = OPT_GETVALUE(s32,val);
					if (v < 0) 
					{
						opt_fatal_1("Invalid (negative) value [v=%d] for OPT_INTLEVEL",v);
					}
					if (v < 10)
					{
						opt_snprintf_1(stval_buf,80,"%1d",v);
					}
					if (v >= 10) 
					{
						/* Could set it to "- -xxxxx" but need to know x */
						/* and it only works if 'x' is a one-character name */
						opt_warn_1("Cannot set string for OPT_INTLEVEL v=%d > 9",v);
					}
				}
				break;
			case OPT_TOGGLE:
			case OPT_BOOL:
				maybe = OPT_GETVALUE(s32,val);
				opt_snprintf_1(stval_buf,80,"%c", (maybe==OPT_TRUE ? FLAG_TRUE : FLAG_FALSE) );
				break;
			case OPT_NEGTOGGLE:
			case OPT_NEGBOOL:
				maybe = OPT_GETVALUE(s32,val);
				opt_snprintf_1(stval_buf,80,"%c", (maybe==OPT_TRUE ? FLAG_FALSE : FLAG_TRUE) );
				break;
			case OPT_STRING:
				if( val == NULL )	/* this should never happen */
					x_sprintf(stval_buf,sizeof(stval_buf)-1,"\"\"");
				else 
					if( OPT_GETVALUE(char *,val) == NULL )
						x_sprintf(stval_buf,sizeof(stval_buf)-1,"\"\"");
					else
						opt_snprintf_1(stval_buf,80,"\"%s\"",OPT_GETVALUE(char *,val));
				break;
			case OPT_NUL:
				stval_buf[0]='\0';
				break;
			default:
				Opt_Util::opt_fatal("opt_getstrval: Undefined o-type");
				break;
			}
			/* ---- Return value is static buffer ---- */
			return stval_buf;
		}/* opt_getstrval */

		s32	Opt_Proc::optsizeof( opt_TYPE typ )
		{
			s32 retval=0;

			switch( typ )
			{
			case OPT_INT:
				retval = sizeof(s32);
				break;
			case OPT_UINT:
				retval = sizeof(u32);
				break;

			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:
				retval = sizeof(s8);
				break;
			case  OPT_UBYTE:
				retval = sizeof(u8);
				break;


			case OPT_SHORT:
				retval = sizeof(s16);
				break;
			case OPT_USHORT:
				retval = sizeof(u16);
				break;
			case OPT_LONG:
				retval = sizeof(s64);
				break;
			case OPT_ULONG:
				retval = sizeof(u64);
				break;
			case OPT_CHAR:
				retval = sizeof(char);
				break;
			case OPT_UCHAR:
				retval = sizeof(unsigned char);
				break;
			case OPT_FLOAT:
				retval = sizeof(f32);
				break;
			case OPT_DOUBLE:
				retval = sizeof(f64);
				break;
			case OPT_INTLEVEL:
				retval = sizeof(s32);
				break;
			case OPT_TOGGLE:
			case OPT_BOOL:
				retval = sizeof(s32);
				break;
			case OPT_NEGTOGGLE:
			case OPT_NEGBOOL:
				retval = sizeof(s32);
				break;
			case OPT_STRING:
				retval = sizeof(char*);
				break;
			case OPT_NUL:
				/* FIXME */
				retval = sizeof(s32(*)());
				break;
			default:
				Opt_Util::opt_fatal("optsizeof: Undefined o-type");
				break;
			}
			return retval;
		}/*optsizeof*/

		/* ----------------------- */
		/* Get and process options */
		/* ----------------------- */

		/*
		* The routines below make use of the xargv structure
		* defined in "ag.h" to process the command line options
		*/

		/* -------------------------------------- */
		/* get and process a positional option */
		/* -------------------------------------- */





		void Opt_Proc::opt_setvalue(void *v, opt_TYPE o, char *s)
		{
			switch(o)
			{
			case OPT_INT:
				OPT_SETVALUE(s32,v, Opt_Num::opt_atoi(s));
				break;
			case OPT_UINT:
				OPT_SETVALUE(u32, v, Opt_Num::opt_atou(s));
				break;
			case OPT_SHORT:
				OPT_SETVALUE(s16, v, Opt_Num::opt_atoi(s));
				break;
			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:
				OPT_SETVALUE(s8, v, Opt_Num::opt_atoi(s));
				break;
			case OPT_UBYTE:
				OPT_SETVALUE(u8, v, Opt_Num::opt_atou(s));
				break;

			case OPT_LONG:
				OPT_SETVALUE(s64, v, Opt_Num::opt_atoi(s));
				break;
			case OPT_USHORT:
				OPT_SETVALUE(u16, v, Opt_Num::opt_atou(s));
				break;
			case OPT_ULONG:
				OPT_SETVALUE(u64, v, Opt_Num::opt_atou(s));
				break;
			case OPT_FLOAT:
				OPT_SETVALUE(f32, v, Opt_Num::opt_atof(s));
				break;
			case OPT_DOUBLE:
				OPT_SETVALUE(f64, v, Opt_Num::opt_atof(s));
				break;
			case OPT_CHAR:
				OPT_SETVALUE(char, v, s[0]);
				break;
			case OPT_UCHAR:
				OPT_SETVALUE(unsigned char, v, s[0]);
				break;

			case OPT_STRING: /* warning: memory leak! */
				OPT_SETVALUE(char *, v, Opt_Allocator::opt_strdup(s));
				break;

			default:
				Opt_Util::opt_fatal("Invalid type");
			}
		}

		void Opt_Proc::optarray_action(OptArray_t *arr, char *stmp)
		{
			char *s = Opt_Allocator::opt_strdup(stmp);
			char *t = s;
			char *u = s;
			char cu = '\0';
			char *v;  /* why is this a char * and not a void *, even though it
					  * is meant as a generic pointer?  see the long comment
					  * above in the optarray_getstrval() function
					  */

			/* Reset the array values */
			if (*(arr->value))
			{
#if 0 /* Trying to free some memory, but I get "FREE DANGLING" */   
				if (arr->base_type == OPT_STRING)
				{
					s32 i;
					for (i=0; i < *(arr->size); ++i)
					{
						char *ith_val = (char *)(*(arr->value)) + i;
						free( ith_val );
					}
				}
#endif    
				Opt_Allocator::get_opt_allocator()->deallocate(*(arr->value));
			}
			*(arr->value)=NULL;
			*(arr->size)=0;

			while (*t) 
			{
				for (u=t; !(*u == '\0' || *u == arr->delim); ++u) 
				{
					/* read until the first delimiter */
					/* t is first character of word */
					/* u is delimiter, so u-1 is last character of t's word */
				}
				cu = *u;
				*u = '\0';  /* now t is null-terminated string */

				/* add new element to array */
				*(arr->size) += 1;
				if (*(arr->value) == NULL)
				{
					*(arr->value) = Opt_Allocator::get_opt_allocator()->allocate(*(arr->size)*optsizeof(arr->base_type), 4);
				}
				else
				{
					*(arr->value) = Opt_Allocator::get_opt_allocator()->reallocate(*(arr->value),*(arr->size)*optsizeof(arr->base_type), 4);
				}

				/* v points to the next element in the array */
				v = (char *)(*(arr->value)) + (*(arr->size)-1)*optsizeof(arr->base_type);
				/* set the value of v according to the string t */
				opt_setvalue(v,arr->base_type,t);
				/* now go to the next word */
				if (cu)
					t = u+1;
				else    
					t = u; /* unless that was the last word */
			}
			Opt_Allocator::get_opt_allocator()->deallocate(s);
		}

		s32 Opt_Proc::opt_action(s32 i, xargv *ag)
		{
			/* ------------- */
			/* act on option */
			/* ------------- */
			opt_TYPE  o;
			char      c;
			char      *s;
			s32       yes,no,maybe,toggle;    /* flag values */

			if (!OPT_isvalidnum(i)) 
			{
				opt_warn_1("opt_action: invalid number %d\n",i);
				return 0; /* failure */
			}

			switch( o=optlist[i].type ) 
			{
			case OPT_INT:
			case OPT_UINT:
			case OPT_SHORT:

			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:	
			case OPT_UBYTE:


			case OPT_LONG:
			case OPT_USHORT:
			case OPT_ULONG:
			case OPT_FLOAT:
			case OPT_DOUBLE:
				opt_setvalue(optlist[i].value,o,Ag_Func::argnext(ag));
				break;
			case OPT_CHAR:
//				OPT_SET_ITH_VALUE(char,i,ag_c_advance(ag));

				//process 2 lines arguments
				char result;
				result = Ag_Func::argnext(ag)[0];
				OPT_SET_ITH_VALUE(char,i,result); 

				break;
			case OPT_UCHAR:
				OPT_SET_ITH_VALUE(unsigned char,i,Ag_Func::ag_c_advance(ag));
				break;

			case OPT_TOGGLE:
			case OPT_BOOL:
			case OPT_NEGTOGGLE:
			case OPT_NEGBOOL:
				/* define terms:
				* yes: value of flag when  ON
				* no:  value of flag when OFF
				* maybe:  present value of flag
				* toggle: negative of maybe
				*/
				maybe  = OPT_GET_ITH_VALUE(s32,i);
				toggle = (maybe==OPT_TRUE ? OPT_FALSE : OPT_TRUE);
				
				if( o == OPT_TOGGLE || o==OPT_BOOL )
					yes=OPT_TRUE;
				else
					yes=OPT_FALSE;

				no = (yes==OPT_TRUE ? OPT_FALSE : OPT_TRUE);
				c = Ag_Func::ag_c(ag);           /* character following 'c' */



				//process 2 lines arguments
				if(ag->iw+1 < ag->c	&&	x_strlen(ag->v[ag->iw+1])==1 && ag->v[ag->iw+1][0]!='-')
				{
					c = ag->v[ag->iw+1][0];
					ag->iw++;
				}



				switch(c) 
				{
				case '+':
				case '1':
					OPT_SET_ITH_VALUE(s32,i,yes);
					Ag_Func::ag_c_advance(ag);       /* eat the '+' */
					break;
				case '-':
				case '0':
					OPT_SET_ITH_VALUE(s32,i,no);
					Ag_Func::ag_c_advance(ag);       /* eat the '-' */
					break;
				default:
					if( o==OPT_BOOL || o==OPT_NEGBOOL ) 
					{
						OPT_SET_ITH_VALUE(s32,i,yes);
					} 
					else
					{    /* otherwise toggle value */
						OPT_SET_ITH_VALUE(s32,i,toggle);
					}
					break;
				}
				
				break;

			case OPT_INTLEVEL:
				/* default is to increment */
				OPT_SET_ITH_VALUE(s32,i,OPT_GET_ITH_VALUE(s32,i) + 1);  
				c = Ag_Func::ag_c(ag);
				switch(c) {
				case '+':
					/* we've already incremented */
					Ag_Func::ag_c_advance(ag);
					break;
				case '-':
					OPT_SET_ITH_VALUE(s32,i,0);
					Ag_Func::ag_c_advance(ag);
					break;
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					OPT_SET_ITH_VALUE(s32,i,c-'0');
					Ag_Func::ag_c_advance(ag);
				default:
					break;
				}
				break;

			case OPT_STRING:
				s = Ag_Func::argnext(ag);
				if (optinvoked_n(i) && optlist[i].value != NULL)
				{
					/* if this option has been invoked before, then we can be
					* confident that it can be safely freed, before reassigning
					* it to a new value
					*/
#if 0
					fprintf(stderr,"invoked opt[%d] for %d times:s=%s\n",i,optinvoked_n(i),s);
#endif          
					Opt_Allocator::get_opt_allocator()->deallocate(*((char **)(optlist[i].value)));
				}
				OPT_SET_ITH_VALUE(char *,i, Opt_Allocator::opt_strdup(s));
				

				/* If this type is really an array, then act accordingly */
				if (optlist[i].array) 
				{
					optarray_action(optlist[i].array,s);
				}
				break;

			case OPT_NUL:
				break;

			default:
				Opt_Util::opt_fatal("opt_delim: don't know this type");
				break;
			}

			/* Indicate that the option was invoked */
			optlist[i].invoked += 1;

			/* Run the hook if its defined */
			if (optlist[i].hook) 
			{
				s32 retval;
				retval = optlist[i].hook(optlist[i].value);

				if (retval == OPT_ERROR) 
				{
					/* do nothing for now */;
				}

				if (retval == OPT_ABORT)
					return 0;	//exit(opt_exit_number);
				if (retval == OPT_QUIT)
					return 0;	//opt_quit();
				if (retval == OPT_EXIT)
					return 0;	//exit(opt_exit_number);
			}

			return 1;                  /* success */
		}

		char*	Opt_Proc::optquotedstrtyp(s32 i)
		{
			char *s,*ss;
			ss = s = optstrtyp(i);
			while (*s) 
			{
				if (*s == '<') 
				{ 
					*s = '\"'; ++s;
				}
				else if (*s == '>') 
				{
					*s = '\"'; ++s; *s='\0';
				}
				else ++s;
			}
			return ss;
		}


	}
}
