#ifndef __XCMDLINE_OPT_H__
#define	__XCMDLINE_OPT_H__
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase\x_types.h"
#include "xbase\x_allocator.h"

/***
* Enable linking with C++
***/
namespace xcore
{
	namespace xcmdline
	{
		struct xargv;
		typedef enum
		{
			OPT_NUL,
			OPT_INT,        OPT_SHORT,      OPT_LONG,       OPT_CHAR,
			OPT_UINT,       OPT_USHORT,     OPT_ULONG,      OPT_UCHAR,

			// Add this two types to deal with the s8 and u8
			OPT_BYTE,       OPT_UBYTE,

			OPT_FLOAT,      OPT_DOUBLE,
			OPT_TOGGLE,     OPT_BOOL,
			OPT_NEGTOGGLE,  OPT_NEGBOOL,    OPT_INTLEVEL,       
			OPT_STRING,     
		} opt_TYPE;

		/* define the deprecated type names */
		#define OPT_FLAG OPT_TOGGLE
		#define OPT_ABSFLAG OPT_BOOL
		#define OPT_NEGFLAG OPT_NEGTOGGLE
		#define OPT_ABSNEGFLAG OPT_NEGBOOL
		#define OPT_VSTRING OPT_STRING
		#define OPT_UNSINT OPT_UINT
		#define OPT_UNDELIMV OPT_UNDELIM

		/* OPT_PFI is the ever popular: Pointer to Function returning an Integer */
		typedef	s32 (*OPT_PFI)(void);
		typedef   s32 (*OPT_HOOK)(void *);
		typedef	s32 (*OPT_PFI_ARG)(s32,char**);

		#define	OPT_EXT	".opt"	  /* standard options file extension */
		#define OPT_MAXSTRLEN 80  /* CSTRING's should be no longer than this */

		#define OPT_TRUE 1
		#define OPT_FALSE 0

		/* Return values from hook functions */
		#define OPT_OK    0
		#define OPT_ERROR 1
		#define OPT_ABORT 2
		#define OPT_QUIT  3
		#define OPT_EXIT  4


		/* OPT_GETVALUE: returns value of what 'void *' points to */
		#define OPT_GETVALUE(typ,v)			((typ)(*((typ *)(v))))

		

		/* Possible values for the option mode attribute */
		typedef enum
		{
			OPT_POSITIONAL,
			OPT_DELIMITED,
			OPT_FLEXIBLE
		} opt_MODE;


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
			char			*longname;		/* long version of the name */
			opt_MODE		mode;			/* delimited, positional or flexible? */
			char			*descript;		/* a brief description */
			char			*help;			/* longer help message */
			s32				invoked;		/* counts number of times option is invoked */
			OPT_HOOK		hook;			/* code to evaluate if option is invoked */
			OptArray_t		*array;			/* set to null if not an array option */
		};



		/* opt(&argc,&argv) is the main function call that does all the work.
		* it processes the options on the command line, setting variables,
		* calling hooks, etc. 
		*/
 		class Opt_Allocator
 		{
	 	public:
	 		static void				set_opt_allocator(x_iallocator*);
 			static x_iallocator*	get_opt_allocator();
			static char*				opt_strdup(char *s);
			
 		private:
 			static x_iallocator*		sCmdLineOptAllocator;
 		};

 		class Opt_Reg
 		{
 		public:
			friend class			Opt_Proc;
			/* opt_free() is called when you are sure you won't be using any more
			* opt functions; usually, you could call it right after opt(), but
			* after opt_free(), you won't be able to use any other opt functions,
			* such as optinvoked
			*/
			static void			opt_free();

			/* But before opt() can be called, variables/hooks/etc must be
			* registered.  Various ways are provided to register variables; one
			* identifies the variable by its (void *) pointer.  To register a
			* variable, an opt_TYPE must be specified, and either a character or
			* a string name (or both), and optionally a brief description.
			*/
			/*  optrega: "all"
			*  optreg:  standard
			*  optregc: just sets character name
			*  optregs: just sets string name
			*  optregcb: sets character name and brief description
			*  optregsb: sets string name and brief description
			*  optregp: sets string name, brief description and OPT_POSITIONAL mode
			*  optregf: sets character name, string name, brief description and OPT_FLEXIBLE mode
			*  optexec: registers an 'exec function' (something that is run, then exits)
			*/

 			static  s32			optrega(  void *, opt_TYPE, char,  char *, char * );
 			static  s32			optreg(   void *, opt_TYPE, char,          char * );
 			static  s32			optregc(  void *, opt_TYPE, char                  );
 			static  s32			optregcb( void *, opt_TYPE, char,          char * );
 			static  s32			optregs(  void *, opt_TYPE,        char *         );
 			static  s32			optregsb( void *, opt_TYPE,        char *, char * );
 			static  s32			optregp(  void *, opt_TYPE,        char *, char * );
 			static  s32			optregf(  void *, opt_TYPE, char,  char *, char * );
			/*    ARGUMENT LIST: &var    OPT_INT   'c'   "name"   "Brief Description" */
			/*    ARGUMENTS:    "name"  hook      "Descript" */
 			static  s32			optexec( char *, OPT_HOOK, char * );


			/* ARRAY OPTIONS, same as scalar except first argument points to
			* the size of the array. Not currently available for positionals */
			/*  ARGUMENT LIST:      &nelts, &array, OPT_INT, 'c'   "name"  "Descript" */
 			static  s32			optrega_array(  s32 *, void *, opt_TYPE, char, char *, char *);
 			static  s32			optreg_array(   s32 *, void *, opt_TYPE, char,         char *);
 			static  s32			optregc_array(  s32 *, void *, opt_TYPE, char                );
			static  s32			optregcb_array( s32 *, void *, opt_TYPE, char,         char *);
			static  s32			optregs_array(  s32 *, void *, opt_TYPE,       char *        );
			static  s32			optregsb_array( s32 *, void *, opt_TYPE,       char *, char *);
			static  s32			optregp_array(  s32 *, void *, opt_TYPE,       char *, char *);
			static  s32			optregf_array(  s32 *, void *, opt_TYPE, char, char *, char *);

		/* Using the following routines, one can specify features (such as
		* char, longname, descript) of the variable that were left out in the
		* registration process, or one can add features (such as help, hook,
		* exec) that the registration routines do not permit.
		*/
		/* These functions use &var to identify which option */
			static  void			optchar(         void *, char);
			static  void			optlongname(     void *, char *);
			static  void			optmode(         void *, opt_MODE);
			static  void			optdescript(     void *, char *);
			static  void			opthelp(         void *, char *);
			static  void			opthook(         void *, OPT_HOOK);
			static  void			optarraydelim(   void *, char);
 		/* These functions use option number to identify which option */
			static  void			optchar_n(       s32, char);
			static  void			optlongname_n(   s32, char *);
			static  void			optmode_n(       s32, opt_MODE);
			static  void			optdescript_n(   s32, char *);
			static  void			opthelp_n(       s32, char *);
			static  void			opthook_n(       s32, OPT_HOOK);
			static  void			optarraydelim_n( s32, char);  

			static  void			opt_setvalue(void *v, opt_TYPE o, char *s);

		/* The following routines are used to set features (such as usage),
		* which are not specific to single variables, but apply to the whole
		* program.  In general, I try to use the convention optUpperCase()
		* for the registration of these more global features, while
		* optlowercase() refers to functions that apply to a specific
		* registered variable.
		*/
			static  void			optTitle(char *);	
			static  void			optProgName(char *);
			static  void			optVersion(char *);
			static  void			optUsage(char *);
			static  void			optEnvVarName(char *);
			static  void			optDefaultString(char *);
			static  void			optAdditionalUsage(OPT_PFI);
			static  void			optExitNumber(s32 n);

		protected:
			static  void			opt_freestrings();

		private:
			static  s32			opt_action(s32 i, xargv *ag);
			static  s32            opt_longname_number(char *s);
			static  s32			opt_number(void *);
			static  char*			opt_getstrval( void *val, opt_TYPE typ );
			static  char			*optarray_getstrval(s32, void *, opt_TYPE, char);
			static  s32			optsizeof(opt_TYPE);
			static  void			optarray_action(OptArray_t *, char *);
			static  s32			optinvoked_n(s32 n);

		public:
			static  inline  char		*getProgramName() {return opt_program_name;}
			static  inline  s32			getNumReg() {return opt_nreg;}
			static  inline  s32			getExitNumber() {return opt_exit_number;}
			static  Option_t			*optlist;
		private:
			static  char					opt_array_delim;
			static  char					*opt_program_name;
			static  OPT_PFI				opt_additional_usage_fcn; 
			static  s32					opt_nreg;
			static  s32					opt_exit_number;
			
		};

		/* ---------- */
		/* OPT macros */
		/* ---------- */

		#define OPT_isvalidnum(n) ((n)>=0 && (n)<Opt_Reg::opt_nreg)
		#define	OPT_isflagtype(o) \
			((o)==OPT_TOGGLE || (o)==OPT_BOOL || \
			(o)==OPT_NEGTOGGLE || (o)==OPT_NEGBOOL)


				/* OPT_SETVALUE: sets the value that the 'void *' points to */

		#define   OPT_SETVALUE(typ,v,val)		do { typ *xptr;			\
			xptr = (typ *)v;		\
			*xptr = (typ)val; } while (0)

				/* OPT_GET_ITH_VALUE: gets value of i'th option */
				/* OPT_SET_ITH_VALUE: sets value of i'th option */

		#define	OPT_GET_ITH_VALUE(typ,i)     OPT_GETVALUE(typ,Opt_Reg::optlist[i].value)
		#define	OPT_SET_ITH_VALUE(typ,i,val) OPT_SETVALUE(typ,Opt_Reg::optlist[i].value,val)
	}
}

#endif /// __XCMDLINE_OPT_H__


