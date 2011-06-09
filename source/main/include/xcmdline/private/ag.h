#ifndef __XCMDLINE_AG_H__
#define __XCMDLINE_AG_H__

namespace xcore
{
	namespace xcmdline
	{
		#ifndef	TRUE
			#define	TRUE	1
			#define	FALSE	0
		#endif

		/*************************************
		 * xargv structure
		 * Basically, an (argc,argv) construct
		 * with indices to which character of 
		 * which word is the current position
		 */
		struct xargv
		{
			char    **v;            /// argument vector
			s32     c;              /// argument count
			s32     iw;             /// current word
			s32     ic;             /// current character
		};

		/* 
		 * Function prototypes 
		 */

		extern xargv		*ag_new(s32,char **);
		extern void			ag_free(xargv *);
		extern s32			ag_enstring(char *,xargv *,s32);
		extern void			ag_reset(xargv *);
		extern s32			ag_w_number(xargv *);
		extern void			ag_w_advance(xargv *);
		extern s32			ag_eow(xargv *);
		extern s32			ag_end(xargv *);
		extern char			ag_c(xargv *);
		extern char			ag_c_next(xargv *);
		extern char			ag_cnn_next(xargv *);
		extern char			ag_c_advance(xargv *);
		extern char			ag_backspace(xargv *);
		extern void			ag_backword(xargv *);
		extern char			*ag_s(xargv *);
		extern char			*ag_s_next(xargv *);
		extern char			*ag_s_advance(xargv *);
		extern s32			ag_clear(xargv *);
		extern s32			ag_argc(xargv *);
		extern char			**ag_argv(xargv *);
		extern xargv		*ag_copy(xargv *);
		extern void			ag_freeall(xargv *);
		extern xargv		*ag_prepend_argv0(xargv *, char *);
		extern char			*argnext(xargv *);
	}
}


#endif	/// __XCMDLINE_AG_H__








