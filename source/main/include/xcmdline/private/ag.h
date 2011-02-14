#ifndef _AG_H
#define _AG_H

namespace xcore
{

	#ifndef	TRUE
	#define	TRUE	1
	#define	FALSE	0
	#endif

	/*************************************
	 * ARGVECTOR structure
	 * Basically, an (argc,argv) construct
	 * with indices to which character of 
	 * which word is the current position
	 */
	struct ARGVECTOR
	{
		char    **v;            /* argument vector */
		int     c;              /* argument count  */
		int     iw;             /* current word    */
		int     ic;             /* current character */
	};

	/* 
	 * Function prototypes 
	 */

	extern ARGVECTOR	*ag_new(int,char **);
	extern void			ag_free(ARGVECTOR *);
	extern void			ag_fprint(FILE *, ARGVECTOR *);
	extern int			ag_enstring(char *,ARGVECTOR *,int);
	extern void			ag_reset(ARGVECTOR *);
	extern int			ag_w_number(ARGVECTOR *);
	extern void			ag_w_advance(ARGVECTOR *);
	extern int			ag_eow(ARGVECTOR *);
	extern int			ag_end(ARGVECTOR *);
	extern char			ag_c(ARGVECTOR *);
	extern char			ag_c_next(ARGVECTOR *);
	extern char			ag_cnn_next(ARGVECTOR *);
	extern char			ag_c_advance(ARGVECTOR *);
	extern char			ag_backspace(ARGVECTOR *);
	extern void			ag_backword(ARGVECTOR *);
	extern char			*ag_s(ARGVECTOR *);
	extern char			*ag_s_next(ARGVECTOR *);
	extern char			*ag_s_advance(ARGVECTOR *);
	extern int			ag_clear(ARGVECTOR *);
	extern int			ag_argc(ARGVECTOR *);
	extern char			**ag_argv(ARGVECTOR *);
	extern ARGVECTOR	*ag_copy(ARGVECTOR *);
	extern void			ag_freeall(ARGVECTOR *);
	extern ARGVECTOR	*ag_prepend_argv0(ARGVECTOR *, char *);
	extern char			*argnext(ARGVECTOR *);

}


#endif /* _AG_H */








