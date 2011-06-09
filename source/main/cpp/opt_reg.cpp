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
*        7) a s64er help message
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

#include "xcmdline\private\opt.h"
#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_p.h"

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

		s32 opt_exit_number=0;          /* if we have to exit(), then exit
										* with this number.
										*/

		/* -------------------------- */
		/* List of Registered Options */
		/* -------------------------- */


		struct OptArray_t
		{
			void			**value;		/* pointer to array of data */
			s32				*size;			/* pointer to size of data (number of elements in array) */
			char			delim;			/* char delimits elements of array: eg "," in "1,2,3" */
			opt_TYPE		base_type;		/* what the array of data is... */
		};


		struct Option_t
		{
			void			*value;			/* pointer to value of option */
			opt_TYPE		type;			/* will be cast according to type */
			char			name;			/* name by which option is invoked */
			char			*s64name;		/* s64 version of the name */
			opt_MODE		mode;			/* delimited, positional or flexible? */
			char			*descript;		/* a brief description */
			char			*help;			/* s64er help message */
			s32				invoked;		/* counts number of times option is invoked */
			OPT_HOOK		hook;			/* code to evaluate if option is invoked */
			OptArray_t		*array;			/* set to null if not an array option */
		};

		static	Option_t *optlist=NULL;             /* array of options */
		s32              opt_nreg=0;                /* number of registered opts */

		/* Prototypes for static (local) functions */
		//static void opt_fileheader(FILE *);
		static s32		opt_action(s32, xargv *);
		static s32		opt_number(void *);
		static s32		opt_s64name_number(char *);
		static char		*optarray_getstrval(s32, void *, opt_TYPE, char);
		static char		*opt_getstrval( void *, opt_TYPE );
		static s32		optsizeof(opt_TYPE);
		static void		opt_setvalue(void *, opt_TYPE, char *);
		static void		optarray_action(OptArray_t *, char *);

		static char opt_array_delim=',';    /* the delimiter for arrays. (eads)*/

		/* ---------- */
		/* OPT macros */
		/* ---------- */

		#define OPT_isvalidnum(n) ((n)>=0 && (n)<opt_nreg)

		#define	OPT_isflagtype(o) \
			((o)==OPT_TOGGLE || (o)==OPT_BOOL || \
			(o)==OPT_NEGTOGGLE || (o)==OPT_NEGBOOL)


		/* OPT_GETVALUE: returns value of what 'void *' points to */
		/* OPT_SETVALUE: sets the value that the 'void *' points to */

		#define OPT_GETVALUE(typ,v)			((typ)(*((typ *)(v))))
		#define OPT_SETVALUE(typ,v,val)		do { typ *xptr;       \
											xptr = (typ *)v; \
											*xptr = (typ)val; } while (0)

		/* OPT_GET_ITH_VALUE: gets value of i'th option */
		/* OPT_SET_ITH_VALUE: sets value of i'th option */

		#define	OPT_GET_ITH_VALUE(typ,i)     OPT_GETVALUE(typ,optlist[i].value)
		#define OPT_SET_ITH_VALUE(typ,i,val) OPT_SETVALUE(typ,optlist[i].value,val)


		void	opt_free()
		{
			s32 i;
			for (i=0; i<opt_nreg; ++i) 
			{
				OPT_FREE(optlist[i].s64name);
				OPT_FREE(optlist[i].help);
				OPT_FREE(optlist[i].descript);
				if (optlist[i].array && optlist[i].type == OPT_STRING)
				{
					char *s = *((char **)(optlist[i].value));
					OPT_FREE(s);
					OPT_FREE(optlist[i].value);
				}
				OPT_FREE(optlist[i].array);
			}
			OPT_FREE(optlist);
			opt_nreg=0;
		}

		static char*	optarray_getstrval( s32 size, void *val, opt_TYPE otype, char delim )
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

				if ( n == size-1 ) str_delim[0]='\0';  /* no delimiter at the end */

				/* Make room in str_val for the next item */
				len += x_strlen(str_nth_val) + x_strlen(str_delim);
				if (str_val)
				{
					str_val = (char *)get_opt_allocator()->reallocate(str_val, sizeof(char)*(len+1), 4);
				}
				else 
				{
					str_val = (char *)get_opt_allocator()->allocate(sizeof(char)*(len+1), 4);
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

		s32	optrega_array(  s32 *size, void *val, opt_TYPE otype, char name, char *s64name, char *descript )
		{
	#if 0  
			static char **str_val_array=NULL;
			static s32    str_val_array_n=0;

			++str_val_array_n;
			str_val_array = (char **)realloc(str_val_array,
				str_val_array_n*sizeof(char *));
			str_val_array[str_val_array_n-1] = optarray_getstrval(*size,val,otype,opt_array_delim);

			/* Register the option as an OPT_STRING */
			optrega( &str_val_array[str_val_array_n-1],
				OPT_STRING, name, s64name, descript );
	#else
			char **s = (char **)get_opt_allocator()->allocate(sizeof(char *), 4);
			*s = optarray_getstrval(*size,val,otype,opt_array_delim);
			optrega( s, OPT_STRING, name, s64name, descript );
	#endif     

			/* Warn if inappropriate base_type is specified */
			switch (otype) 
			{
				/* These are all okay */
			case OPT_INT:
			case OPT_UINT:
			case OPT_SHORT:
			case OPT_USHORT:
			case OPT_LONG:
			case OPT_ULONG:
			case OPT_FLOAT:
			case OPT_DOUBLE:
				break;

				/* These _should_ be okay too */
			case OPT_STRING:
				break;

				/* These might be problematic */
			case OPT_CHAR:
			case OPT_UCHAR:
			case OPT_INTLEVEL:
			case OPT_TOGGLE:
			case OPT_BOOL:
			case OPT_NEGTOGGLE:
			case OPT_NEGBOOL:
			case OPT_NUL:
				opt_warn_1("Undefined behavior for %s array",optstrtyp(opt_nreg-1));
				break;

			default:
				opt_fatal("opt_getstrval: Undefined o-type");
				break;
			}

			/* Allocate an OptArray_t structure */
			optlist[ opt_nreg-1 ].array = (OptArray_t *)get_opt_allocator()->allocate(sizeof(OptArray_t), 4);

			/* Then add array-specific information to the registered option */
			optlist[ opt_nreg-1 ].array->value     = (void **)val;
			optlist[ opt_nreg-1 ].array->size      = size;
			optlist[ opt_nreg-1 ].array->base_type = otype;
			optlist[ opt_nreg-1 ].array->delim     = opt_array_delim;

			return opt_nreg-1;
		}

		s32 optreg_array( s32 *size, void *val, opt_TYPE otype, char name, char *descript )
		{
			return optrega_array(size,val,otype,name,NULL,descript);
		}
		s32 optregc_array( s32 *size, void *val, opt_TYPE otype, char name ) 
		{
			return optrega_array(size,val,otype,name,NULL,NULL);
		}
		s32 optregcb_array( s32 *size, void *val, opt_TYPE otype, char name, char *descript )
		{
			return optrega_array(size,val,otype,name,NULL,descript);
		}
		s32 optregs_array( s32 *size, void *val, opt_TYPE otype, char *s64name ) 
		{
			return optrega_array(size,val,otype,'\0',s64name,s64name);
		}
		s32 optregsb_array( s32 *size, void *val, opt_TYPE otype, char *s64name, char *descript)
		{
			return optrega_array(size,val,otype,'\0',s64name,descript);
		}
		s32 optregp_array( s32 *size, void *val, opt_TYPE otype, char* s64name, char* descript)
		{
			s32 n = optrega_array(size,val,otype,'\0',s64name,descript);
			optmode_n(n, OPT_POSITIONAL);
			return n;
		}
		s32 optregf_array( s32 *size, void *val, opt_TYPE otype, char name, char* s64name, char* descript) 
		{
			s32 n = optrega_array(size,val,otype,name,s64name,descript);
			optmode_n(n, OPT_FLEXIBLE);
			return n;
		}


		/* ------------------------------- */
		/* Routine to register each option */
		/* ------------------------------- */

		s32 optreg(void *val, opt_TYPE otype, char name, char *descript) 
		{
			return optrega(val,otype,name,NULL,descript);
		}
		s32 optregc(void *val, opt_TYPE otype, char name) 
		{
			return optrega(val,otype,name,NULL,NULL);
		}
		s32 optregcb(void *val, opt_TYPE otype, char name, char *descript) 
		{
			return optrega(val,otype,name,NULL,descript);
		}
		s32 optregs(void *val, opt_TYPE otype, char *s64name) 
		{
			return optrega(val,otype,'\0',s64name,s64name);
		}
		s32 optregsb(void *val, opt_TYPE otype, char *s64name, char *descript)
		{
			return optrega(val,otype,'\0',s64name,descript);
		}
		s32 optregp(void *val, opt_TYPE otype, char* s64name, char* descript) 
		{
			s32 n = optrega(val, otype, '\0', s64name, descript);
			optmode_n(n, OPT_POSITIONAL);
			return n;
		}
		s32 optregf(void *val, opt_TYPE otype, char name, char* s64name, char* descript)
		{
			s32 n = optrega(val, otype, name, s64name, descript);
			optmode_n(n, OPT_FLEXIBLE);
			return n;
		}
		s32 optexec(char *s64name, OPT_HOOK fcn, char *descript)
		{
			s32 n;
			n = optrega(NULL,OPT_NUL,'\0',s64name,descript);
			opthook_n(n,fcn);
			return n;
		}

		s32	optrega(void *val, opt_TYPE otype, char name, char *s64name, char *descript)
		{
			opt_MODE mode = OPT_DELIMITED;

			/* write a warning if that option name has been used before */
			if( name != '\0' && opt_char_number(name) != -1 )
			{
				opt_warn_1("optrega: Duplicate option name \'%c\'",name);
			}
			if (!ISEMPTYSTRING(s64name) && opt_s64name_number(s64name) != -1) 
			{
				opt_warn_1("optrega: Duplicate option s64name \"%s\"",s64name);
			}

			/* val points to the memory where the value of the option
			* is maintained.  This cannot be NULL.
			*/
			if( val == NULL && otype != OPT_NUL ) {
				if (name)     { opt_warn_1("Name: -%c", name); }
				if (s64name) { opt_warn_1("Name: --%s",s64name); }
				opt_fatal("optrega: invalid first argument=NULL\n");
			}

			/* This is probably a mistake to print out a warning message,
			since it will be printed every time the program is run.
			Perhaps it should only be run if the user types 'run --testopt'
			*/
	#if 0    
			if( val != NULL  && opt_val_number(val) != -1)
			{
				opt_warn_1("optrega: This variable [%s] has already been registered",
					thename);
			}
	#endif    

			/* Programmer may optionally signal that an option is to be a
			* positional option by setting the name to '\0' and the s64name
			* to NULL or an empty string.
			*/
			if(name=='\0' && ISEMPTYSTRING(s64name))
			{
				mode = OPT_POSITIONAL;
			}

			/* Make sure that the s64name doesn't have any whitespace */
			if (!ISEMPTYSTRING(s64name)) 
			{
				char *t = s64name;
				while (*t != '\0') 
				{
					if (x_isspace(*t))
					{
						opt_fatal_1("invalid s64name [%s] contains whitespace\n",s64name);
					}
					++t;
				}
			}

			/* reallocate another struct for the new opt */
			++opt_nreg;
			optlist = (Option_t *)get_opt_allocator()->reallocate(optlist, opt_nreg*sizeof(Option_t), 4);

			/* Having checked for various warnings, now register the options */

			optlist[opt_nreg-1].value         = val;
			optlist[opt_nreg-1].type          = otype;
			optlist[opt_nreg-1].name          = name;
			optlist[opt_nreg-1].s64name      = s64name ? opt_strdup(s64name) : NULL;
			optlist[opt_nreg-1].mode          = mode;
			optlist[opt_nreg-1].descript      = descript ? opt_strdup(descript) : NULL;

			optlist[opt_nreg-1].invoked       = 0;
			optlist[opt_nreg-1].help          = NULL;
			optlist[opt_nreg-1].hook          = NULL;
			optlist[opt_nreg-1].array         = NULL;

			return opt_nreg-1;

		}

		void optchar_n(s32 n, char name)
		{
			if (OPT_isvalidnum(n)) optlist[n].name = name;
		}    
		void optmode_n(s32 n, opt_MODE mode)
		{
			if (OPT_isvalidnum(n)) {
				if (!(mode == OPT_POSITIONAL || mode == OPT_DELIMITED || mode == OPT_FLEXIBLE))
					opt_warning("optmode() was passed an invalid mode");
				optlist[n].mode = mode;
			}
		}
		void optdescript_n(s32 n, char *descript)
		{
			if (OPT_isvalidnum(n)) opt_setstring(&optlist[n].descript,descript);
		}
		void opthelp_n(s32 n, char *help)
		{
			if (OPT_isvalidnum(n)) opt_setstring(&optlist[n].help,help);
		}
		void opts64name_n(s32 n, char *s64name)
		{
			if (OPT_isvalidnum(n)) opt_setstring(&optlist[n].s64name,s64name);
		}
		void opthook_n(s32 n, OPT_HOOK hook)
		{
			if (OPT_isvalidnum(n)) optlist[n].hook = hook;
		}
		s32 optinvoked_n(s32 n)
		{
			if (OPT_isvalidnum(n))
				return optlist[n].invoked;
			return 0;
		}

		s32 optinvoked(void *v) { return optinvoked_n( opt_number(v) ); }

		void optchar(void *v,  char name) { optchar_n( opt_number(v),   name ); }
		void opthelp(void *v,  char *help) { opthelp_n( opt_number(v),  help ); }
		void opthook(void *v,  OPT_HOOK hook) { opthook_n( opt_number(v), hook ); }      
		void opts64name(void *v, char *s64name)
		{
			opts64name_n( opt_number(v), s64name );
		}
		void optmode(void* v, opt_MODE mode) 
		{
			optmode_n( opt_number(v), mode );
		}
		void optdescript(void *v, char *descript)
		{
			optdescript_n( opt_number(v), descript );
		}
		void optarraydelim_n(s32 n, char delim)
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
		void optarraydelim(void *v, char delim)
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

		void optExitNumber(s32 n)
		{
			opt_exit_number = n;
		}


		void	opt_get_help(char c)
		{
			s32 n;
			n = opt_char_number(c);
			if (OPT_isvalidnum(n) && optlist[n].help != NULL)
				opt_mess_2("%c: %s\n",c,optlist[n].help);
			else
				opt_mess_1("Help unavailable for \'%c\'\n",c);
		}



		/* opt_XXX_number(): Get number corresponding to option name; return
		* a value of -1 if the option does not exist.
		*  opt_char_number(c) takes as input the single-character name;
		*  opt_s64name_number(s64name) takes as input the s64 name
		*  opt_number(void *) takes as input pointer to variable
		*/

		s32	opt_char_number(char c)
		{
			s32 i;                         /* see which registered option */
			for(i=0; i<opt_nreg; ++i)
			{
				if( c == optlist[i].name )
					return i;
			}
			return -1;
		}

		static s32	opt_number(void *v)
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
		static s32	opt_s64name_number(char *s)
		{
			s32 i;                         /* see which registered option */
			if ( ISEMPTYSTRING(s) )
				return -1;
			for(i=0; i<opt_nreg; ++i)
			{
				if ( !ISEMPTYSTRING(optlist[i].s64name) ) 
				{
					if (0==x_strcmp(s, optlist[i].s64name)) 
					{
						return i;
					}
				}
			}
			return -1;                     /* to signify not an option */
		}

		/* -------------------------------------------- */
		/* Print value of registered option to a string */
		/* -------------------------------------------- */

		/* optstrval:
		* returns a string corresponding to the ith option
		* Thus if x = 12.6, this returns the string "12.6"
		* Calls opt_getstrval for all types except INTLEVEL.
		* The return value is a pointer to a static string which is
		* overwritten with each call
		* FLAG values are returned as "+" or "-"
		* Previously: INTLEVEL values are returned as "- -xxxx..." depending on value
		* Now, more straightforward, treat INTLEVEL just like INT
		*/

		char*	optstrval(s32 i)
		{
			return opt_getstrval(optlist[i].value,optlist[i].type);
		}

		/* -------------------------------------------- */
		/* Print value of registered option to a string */
		/* without specifying an index. (eads)          */
		/* -------------------------------------------- */

		static char*	opt_getstrval( void *val, opt_TYPE typ )
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
					if (1) {
						s32 v = OPT_GETVALUE(s32,val);
						if (v < 0) {
							opt_fatal_1("Invalid (negative) value [v=%d] for OPT_INTLEVEL",v);
						}
						if (v < 10) {
							opt_snprintf_1(stval_buf,80,"%1d",v);
						}
						if (v >= 10) {
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
					opt_fatal("opt_getstrval: Undefined o-type");
					break;
				}
				/* ---- Return value is static buffer ---- */
				return stval_buf;
		}/* opt_getstrval */

		static s32	optsizeof( opt_TYPE typ )
		{
			s32 retval=0;

			switch( typ ) {
			case OPT_INT:
				retval = sizeof(s32);
				break;
			case OPT_UINT:
				retval = sizeof(u32);
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
				opt_fatal("optsizeof: Undefined o-type");
				break;
			}
			return retval;
		}/*optsizeof*/

		/* optstrtyp:
		* returns the type of the ith registered option as a string
		* Thus if x = 12.6 is a f32 , this returns the string "<f32>"
		* The return value is a pointer to a static string which is
		* overwritten with each call
		*/
		char*	optstrtyp(s32 i)
		{
			static char	sttyp_buf[80];
			opt_TYPE o;

			o = (optlist[i].array) ? optlist[i].array->base_type : optlist[i].type;

			switch( o ) 
			{
				/* Integers */
			case OPT_INT:         x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<INT"); break;
			case OPT_UINT:        x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<UINT"); break;
			case OPT_SHORT:       x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<s16"); break;
			case OPT_LONG:        x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<LONG"); break;
			case OPT_USHORT:      x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<Us16"); break;
			case OPT_ULONG:       x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<ULONG"); break;
				/* f32ing point */
			case OPT_FLOAT:       x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<f32"); break;
			case OPT_DOUBLE:      x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<f64"); break;
				/* Char */
			case OPT_CHAR:        x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<CHAR"); break;
			case OPT_UCHAR:       x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<UCHAR"); break;
				/* Flag */
			case OPT_INTLEVEL:    x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<INTLEVEL"); break;
			case OPT_TOGGLE:      x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<TOGGLE"); break;
			case OPT_BOOL:        x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<BOOL"); break;
			case OPT_NEGTOGGLE:   x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<NEGTOGGLE"); break;
			case OPT_NEGBOOL:     x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<NEGBOOL"); break;
				/* String */
			case OPT_STRING:      x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<STRING"); break;
				/* Nul */
			case OPT_NUL:         x_strcpy(sttyp_buf, sizeof(sttyp_buf),"<NUL"); break;

			default:
				opt_fatal("usage: undefined o-type");
			}
			/* If it's an array, then add " ARRAY" to the name of the type */
			if ( optlist[i].array )
			{
				x_strcat(sttyp_buf, sizeof(sttyp_buf), " ARRAY");
			}
			/* finish with closing bracket */
			x_strcat(sttyp_buf, sizeof(sttyp_buf), ">");
			return sttyp_buf;
		}

		char		*opt_program_name;
		OPT_PFI		opt_additional_usage_fcn; 

		/* opt_usage: Write a usage statement describing registered options.
		* This is what the program writes in response to '--help' on the
		* command line.
		*/

		#define OPTUSAGEDEBUG 0
		#define OPTDESCRIPTWIDTH  43
		#define OPTDESCRIPTOFFSET 38
		#define OPTMAXLONGNAMELEN 14
		void	opt_usage(void)
		{
			static char *dformat =        " -%c                   %-14s  %s\n";
			static char *lformat =                 " -%c, --%-14s %-14s  %s\n";
			static char *pformat =                        " %-20s %-14s  %s\n";
			static char *uformat =  " %-10s                              %s\n";
			static char *aformat =                  "     --%-14s %-14s  %s\n";
			s32  i;
			s32 initial;
			char* justified_descript;

			/* First we print the positional options */
			initial = 1;
			for (i = 0; i < opt_nreg; ++i) 
			{
				if (optlist[i].mode != OPT_DELIMITED) 
				{
					if (initial) 
					{
						opt_message("The arguments are:\n");
						initial = 0;
					}
					/* Get a suitably formatted version of the description  */
					/* descript may be NULL, but opt_justify always returns */
					/* a valid (perhaps empty) string.                      */
					justified_descript = opt_justify(optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
					if (OPTUSAGEDEBUG) 
						x_printf("%2d u ",i);

					if (!ISEMPTYSTRING(optlist[i].descript))
						opt_mess_3(pformat,optlist[i].s64name,optstrtyp(i),justified_descript);
					else
						opt_mess_2(uformat,optstrtyp(i),justified_descript);

					get_opt_allocator()->deallocate(justified_descript);
				}
			}

			/* Second we print the delimited options */
			initial = 1;
			for(i=0; i<opt_nreg; ++i) 
			{
				if (optlist[i].mode != OPT_POSITIONAL && (optlist[i].name != '\0' || !ISEMPTYSTRING(optlist[i].s64name))) 
				{
						if (initial) 
						{
							opt_message("The options are:\n");
							initial = 0;
						}
						/* Get a suitably formatted version of the description */
						/* If the option is positional, then refer to the argument description */
						if (optlist[i].mode == OPT_FLEXIBLE) 
						{
							justified_descript = opt_justify("(See argument description above)", OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
						} 
						else if (optlist[i].s64name && x_strlen(optlist[i].s64name) > OPTMAXLONGNAMELEN) 
						{
							justified_descript = opt_justify(optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET, OPTDESCRIPTOFFSET,0);
						} 
						else 
						{
							justified_descript = opt_justify(optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
						}

						if (optlist[i].name == '\0') 
						{
							if(OPTUSAGEDEBUG) 
								x_printf("a ");

							if (x_strlen(optlist[i].s64name) > OPTMAXLONGNAMELEN) 
							{
								opt_mess_3(aformat,optlist[i].s64name, optstrtyp(i),"");
								opt_mess_1("%s\n",justified_descript);
							} 
							else
							{
								opt_mess_3(aformat,optlist[i].s64name, optstrtyp(i),justified_descript);
							}            
						}
						else
						{
							if (!ISEMPTYSTRING(optlist[i].s64name)) 
							{
								if(OPTUSAGEDEBUG) x_printf("l ");
								if (x_strlen(optlist[i].s64name) > OPTMAXLONGNAMELEN) 
								{
									opt_mess_4(lformat,optlist[i].name,optlist[i].s64name, optstrtyp(i),"");
									opt_mess_1("%s\n",justified_descript);
								} 
								else
								{
									opt_mess_4(lformat,optlist[i].name,optlist[i].s64name, optstrtyp(i),justified_descript);
								}            
							} 
							else 
							{
								if (OPTUSAGEDEBUG) x_printf("d ");
									opt_mess_3(dformat,optlist[i].name, optstrtyp(i),justified_descript);
							}
						}
						get_opt_allocator()->deallocate(justified_descript);
				}
			}

			if (opt_additional_usage_fcn != NULL)
			{
				/* precede additional usage by -- to indicate that we
				* are finished with the options
				*/
				opt_message(" --\n");
				/* return value is ignored */
				(*opt_additional_usage_fcn)();
			}
		}

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

		s32	opt_parse_positional(xargv *ag)
		{
			s32     i;

			/* Find the first positional option that has not been invoked and
			* set that to the argument string.  Note that once a positional
			* option has been invoked, it cannot be un-invoked.
			*/

			for (i=0; i<opt_nreg; ++i) 
			{
				if (optlist[i].invoked || optlist[i].mode == OPT_DELIMITED) 
					continue;
				return opt_action(i,ag);
			}
			/* If we get here, we didn't find an un-invoked positional option */
			return 0;
		}

		/* ---------------------------------- */
		/* get and process a delimited option */
		/* ---------------------------------- */

		s32	opt_parse_delim(xargv *ag)
		{
			s32      i;
			char     c;

			c = ag_c_advance(ag);		/* first character gives name of option */
			i = opt_char_number(c);		/* this is number of option w/ name 'c' */

			if(! OPT_isvalidnum(i))
			{	/* if invalid option name */
				opt_warn_1("%c not a registered option name\n",c);
				return 0;
			}
			return opt_action(i,ag);
		}


		s32	opt_parse_s64delim(xargv *ag)
		{
			s32      i;
			char     c;
			char     *sc,*scptr;

			/* this code parses input options of the form --var=value */

			sc = opt_strdup(ag_s(ag));      /* s64 option name is a string */
			scptr = sc;
			while( !ag_eow(ag) )
			{
				c=ag_c_advance(ag);
				if (c == '=')
				{
					*scptr = '\0';
					break;
				}
				++scptr;
			}

			i  = opt_s64name_number(sc);   /* this is number of option w/ name `sc' */
			if(! OPT_isvalidnum(i))
			{
				opt_warn_1("%s not a registered option name\n",sc);
				return 0;
			}
			get_opt_allocator()->deallocate(sc); 
			return opt_action(i,ag);
		}

		static void
			opt_setvalue(void *v, opt_TYPE o, char *s)
		{
			switch(o)
			{
			case OPT_INT:
				OPT_SETVALUE(s32,v, opt_atoi(s));
				break;
			case OPT_UINT:
				OPT_SETVALUE(u32, v, opt_atou(s));
				break;
			case OPT_SHORT:
				OPT_SETVALUE(s16, v, opt_atoi(s));
				break;
			case OPT_LONG:
				OPT_SETVALUE(s64, v, opt_atoi(s));
				break;
			case OPT_USHORT:
				OPT_SETVALUE(u16, v, opt_atou(s));
				break;
			case OPT_ULONG:
				OPT_SETVALUE(u64, v, opt_atou(s));
				break;
			case OPT_FLOAT:
				OPT_SETVALUE(f32, v, opt_atof(s));
				break;
			case OPT_DOUBLE:
				OPT_SETVALUE(f64, v, opt_atof(s));
				break;
			case OPT_CHAR:
				OPT_SETVALUE(char, v, s[0]);
				break;
			case OPT_UCHAR:
				OPT_SETVALUE(unsigned char, v, s[0]);
				break;

			case OPT_STRING: /* warning: memory leak! */
				OPT_SETVALUE(char *, v, opt_strdup(s));
				break;

			default:
				opt_fatal("Invalid type");
			}
		}

		static void
			optarray_action(OptArray_t *arr, char *stmp)
		{
			char *s = opt_strdup(stmp);
			char *t = s;
			char *u = s;
			char cu = '\0';
			char *v;  /* why is this a char * and not a void *, even though it
					  * is meant as a generic pointer?  see the s64 comment
					  * above in the optarray_getstrval() function
					  */

			/* Reset the array values */
			if (*(arr->value))
			{
	#if 0 /* Trying to free some memory, but I get "FREE DANGLING" */   
				if (arr->base_type == OPT_STRING) {
					s32 i;
					for (i=0; i < *(arr->size); ++i) {
						char *ith_val = (char *)(*(arr->value)) + i;
						free( ith_val );
					}
				}
	#endif    
				get_opt_allocator()->deallocate(*(arr->value));
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
				*(arr->value) = get_opt_allocator()->reallocate(*(arr->value),*(arr->size)*optsizeof(arr->base_type), 4);

				/* v points to the next element in the array */
				v = (char *)(*(arr->value)) + (*(arr->size)-1)*optsizeof(arr->base_type);
				/* set the value of v according to the string t */
				opt_setvalue(v,arr->base_type,t);
				/* now go to the next word */
				if (cu) t = u+1;
				else    t = u; /* unless that was the last word */
			}
			get_opt_allocator()->deallocate(s);
		}

		static s32 opt_action(s32 i, xargv *ag)
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
			case OPT_LONG:
			case OPT_USHORT:
			case OPT_ULONG:
			case OPT_FLOAT:
			case OPT_DOUBLE:
				opt_setvalue(optlist[i].value,o,argnext(ag));
				break;
			case OPT_CHAR:
				OPT_SET_ITH_VALUE(char,i,ag_c_advance(ag));
				break;
			case OPT_UCHAR:
				OPT_SET_ITH_VALUE(unsigned char,i,ag_c_advance(ag));
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
				c = ag_c(ag);           /* character following 'c' */
				switch(c) {
				case '+':
				case '1':
					OPT_SET_ITH_VALUE(s32,i,yes);
					ag_c_advance(ag);       /* eat the '+' */
					break;
				case '-':
				case '0':
					OPT_SET_ITH_VALUE(s32,i,no);
					ag_c_advance(ag);       /* eat the '-' */
					break;
				default:
					if( o==OPT_BOOL || o==OPT_NEGBOOL ) {
						OPT_SET_ITH_VALUE(s32,i,yes);
					} else {    /* otherwise toggle value */
						OPT_SET_ITH_VALUE(s32,i,toggle);
					}
					break;
				}
				break;

			case OPT_INTLEVEL:
				/* default is to increment */
				OPT_SET_ITH_VALUE(s32,i,OPT_GET_ITH_VALUE(s32,i) + 1);  
				c = ag_c(ag);
				switch(c) {
				case '+':
					/* we've already incremented */
					ag_c_advance(ag);
					break;
				case '-':
					OPT_SET_ITH_VALUE(s32,i,0);
					ag_c_advance(ag);
					break;
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					OPT_SET_ITH_VALUE(s32,i,c-'0');
					ag_c_advance(ag);
				default:
					break;
				}
				break;

			case OPT_STRING:
				s = argnext(ag);
				if (optinvoked_n(i) && optlist[i].value != NULL)
				{
					/* if this option has been invoked before, then we can be
					* confident that it can be safely freed, before reassigning
					* it to a new value
					*/
	#if 0
					fprintf(stderr,"invoked opt[%d] for %d times:s=%s\n",i,optinvoked_n(i),s);
	#endif          
					get_opt_allocator()->deallocate(*((char **)(optlist[i].value)));
				}
				OPT_SET_ITH_VALUE(char *,i, opt_strdup(s));

				/* If this type is really an array, then act accordingly */
				if (optlist[i].array) 
				{
					optarray_action(optlist[i].array,s);
				}
				break;

			case OPT_NUL:
				break;

			default:
				opt_fatal("opt_delim: don't know this type");
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

		static char *optquotedstrtyp(s32 i)
		{
			char *s,*ss;
			ss = s = optstrtyp(i);
			while (*s) 
			{
				if (*s == '<') { *s = '\"'; ++s; }
				else if (*s == '>') { *s = '\"'; ++s; *s='\0'; }
				else ++s;
			}
			return ss;
		}
	}
}
