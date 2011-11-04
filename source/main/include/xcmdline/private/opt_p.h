#ifndef __XCMDLINE_OPT_P_H__
#define __XCMDLINE_OPT_P_H__
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase\x_types.h"
#include "xbase\x_string_std.h"

namespace xcore
{
	namespace xcmdline
	{
		// Forward declares
		struct xargv;

		#ifndef	OPT_TRUE
		#define	OPT_TRUE	1
		#define	OPT_FALSE	0
		#endif

		#ifndef DEBUG
		#define DEBUG 0
		#endif

		/* 
		 * These are delimiter characters 
		 */
		#define DELIM			'-'		/* option delimiter character */
		#define	ALTDELIM		'/'		/* alternate delimiter character */
		#define	OPTFROMFILE		'@'		/* denotes options are in a file */
		#define	OPTTOFILE		'%'		/* says to put options in a file */
		#define	DOCUMENT		'-'		/* write document to file */
		#define	INTERACT		'$'		/* Flags interactive menu */
		#define IGNOREEOL       ';'		/* Ignore until the end of line */

		/*
		 * These are not available on command line 
		 * But may be invoked from a file or the menu
		 */
		#define	RUN             '='		/* Says to just run to completion */
		#define	QUITCH			'.'		/* Quit character */
		/*
		 * These are not available on command line or from a file
		 * But may be invoked from the menu
		 */
		#define	BANG			'!'		/* Shell escape character */
		#define	ADDITIONAL_OPTS	'+'		/* Additional options */

		/*
		 * These are not available from a file, but may be used on
		 * the command line or from the menu.
		 */
		#define	HELPCH			'?'		/* Help character */
		#define REASONCH        '~'     /* Displays deprecation reason */

		#define ISEMPTYSTRING(s) (!((s) && *(s)))

		/******************
		 * Global variables
		 */


		/*********************
		 * Function Prototypes
		 */

		#define OPT_NUM_VALID   0x1
		#define OPT_NUM_FLOAT   0x2
		#define OPT_NUM_NEG     0x4

		class Opt_Util
		{
		public:
			static void			opt_message(char *);
			static void			opt_warning(char *);
			static void			opt_fatal(char *);
			static void			opt_setstring(char **, char *);
			static char			*opt_justify(char* s, s32 width, s32 indent, s32 initial, char* prefix);

			/*  
			*/
			static s32				optBeginRun(OPT_PFI run);
			static void			optAbortRun();
			static s32				optBeginMain(OPT_PFI_ARG run_args, s32 argc, char **argv);
			static void			optAbortMain();
		};

		class Opt_Num
		{
		public:
			static f64			    opt_atof(char *);
			static s64				opt_atoi(char *);
			static u64				opt_atou(char *);
			static s32				opt_isvalidnumber(char *);
		};

		class Opt_Proc
		{
		public:
			/* opt(&argc,&argv) is the main function call that does all the work.
			* it processes the options on the command line, setting variables,
			* calling hooks, etc. 
			*/
			static void			opt(s32 *,char ***);

			static void			opt_get_help(char);
			static char		    *optgetTitle(void);
			static s32				opt_char_number(char);
			static char			*optstrval(s32);
			static char			*optstrtyp(s32);
			static s32				opt_parse_positional(xargv *);
			static s32				opt_parse_delim(xargv *);
			static s32				opt_parse_longdelim(xargv *);
			static void			short_usage(void);
			static void			long_usage(void);

			/* --------------- */
			/* Process Options */
			/* --------------- */
			static  xargv			*opt_process(int,char **);
			static  char			*short_progname(char *);
			static  s32			opt_lineprocess(char *);
			static  void			opt_help(char *);
			static  void			opt_usage(void);
			static  void			opt_readline_init(char *);
			/* The following routines provide opt-related services that might
			* be useful after opt() has been set up and run.
			*/
			/* optPrintusage() writes the opt usage message to stdout
			*/
			static  void			optPrintUsage();
			/* optinvoked(&var) returns the number of times the option was invoked
			* on the command line.  This function is not used in the registering
			* stage, but is used during execution of the code to see whether or
			* not a value was actually set (or if it's just using the default).
			*/
			static  s32			optinvoked(void *);
		};

		#define opt_mess_1(fmt,var1)                x_printf(fmt,x_va_list(x_va(var1)))
		#define opt_mess_2(fmt,var1,var2)           x_printf(fmt,x_va_list(x_va(var1),x_va(var2)))
		#define opt_mess_3(fmt,var1,var2,var3)      x_printf(fmt,x_va_list(x_va(var1),x_va(var2),x_va(var3)))
		#define opt_mess_4(fmt,var1,var2,var3,var4) x_printf(fmt,x_va_list(x_va(var1),x_va(var2),x_va(var3),x_va(var4)))

		#define opt_snprintf_1(buf,n,fmt,v1)           x_sprintf(buf,n-1,fmt,v1)
		#define opt_snprintf_2(buf,n,fmt,v1,v2)        x_sprintf(buf,n-1,fmt,v1,v2)
		#define opt_snprintf_3(buf,n,fmt,v1,v2,v3)     x_sprintf(buf,n-1,fmt,v1,v2,v3)
		#define opt_snprintf_4(buf,n,fmt,v1,v2,v3,v4)  x_sprintf(buf,n-1,fmt,v1,v2,v3,v4)

		#define OPT_ERRMAXSTRLEN 1024  /* shouldn't be fixed length, but it is! */
  
		#define opt_warn_1(fmt,var1) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_1(gstr,OPT_ERRMAXSTRLEN,fmt,var1); \
				Opt_Util::opt_warning(gstr); } while(0)
		#define opt_warn_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			Opt_Util::opt_warning(gstr); } while(0)
		#define opt_warn_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			Opt_Util::opt_warning(gstr); } while(0)

		#define opt_fatal_1(fmt,var1) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_1(gstr,OPT_ERRMAXSTRLEN,fmt,var1); \
			Opt_Util::opt_fatal(gstr); } while(0)
		#define opt_fatal_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			Opt_Util::opt_fatal(gstr); } while(0)
		#define opt_fatal_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			Opt_Util::opt_fatal(gstr); } while(0)

		#define OPT_FREE(s) do { if (s) { Opt_Allocator::get_opt_allocator()->deallocate(s); s=NULL; } } while (0)

	}
};


#endif	///< __XCMDLINE_OPT_P_H__
