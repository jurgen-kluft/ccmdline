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
		extern	s32		opt_nreg;
		extern  s32		opt_exit_number;

		/*********************
		 * Function Prototypes
		 */

		extern void			opt_get_help(char);
		extern char			*optgetTitle(void);
		extern s32			opt_char_number(char);
		   
		extern char			*optstrval(s32);
		extern char			*optstrtyp(s32);
		extern s32			opt_parse_positional(xargv *);
		extern s32			opt_parse_delim(xargv *);
		extern s32			opt_parse_longdelim(xargv *);

		extern void			short_usage(void);
		extern void			long_usage(void);

  
		#define OPT_NUM_VALID   0x1
		#define OPT_NUM_FLOAT   0x2
		#define OPT_NUM_NEG     0x4

		extern f64			opt_atof(char *);
		extern s64			opt_atoi(char *);
		extern u64			opt_atou(char *);
		extern s32			opt_isvalidnumber(char *);
		extern void			opt_setstring(char **, char *);
		extern char			*opt_justify(char* s, s32 width, s32 indent, s32 initial, char* prefix); 

		/* --------------- */
		/* Process Options */
		/* --------------- */

		extern	char		*opt_program_name;
		extern  OPT_PFI		opt_additional_usage_fcn; 

		extern	void		opt_help(char *);
		extern	void		opt_usage(void);
		extern  void		opt_readline_init(char *);

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
				opt_warning(gstr); } while(0)
		#define opt_warn_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			opt_warning(gstr); } while(0)
		#define opt_warn_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			opt_warning(gstr); } while(0)

		#define opt_fatal_1(fmt,var1) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_1(gstr,OPT_ERRMAXSTRLEN,fmt,var1); \
			opt_fatal(gstr); } while(0)
		#define opt_fatal_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			opt_fatal(gstr); } while(0)
		#define opt_fatal_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
				opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			opt_fatal(gstr); } while(0)

		#define OPT_FREE(s) do { if (s) { get_opt_allocator()->deallocate(s); s=NULL; } } while (0)

		extern char* opt_strdup(char *s);
	}
};


#endif	///< __XCMDLINE_OPT_P_H__
