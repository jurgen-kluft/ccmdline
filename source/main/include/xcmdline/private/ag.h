#ifndef __XCMDLINE_AG_H__
#define __XCMDLINE_AG_H__

namespace xcore
{
	namespace xcmdline
	{
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
		class Ag_Func
		{
		public:
			static  xargv	*ag_new(s32,char **);
			static  void	ag_free(xargv *);
			static  s32		ag_enstring(char *,xargv *,s32);
			static  void	ag_reset(xargv *);
			static  s32		ag_w_number(xargv *);
			static  void	ag_w_advance(xargv *);
			static  s32		ag_eow(xargv *);
			static  s32		ag_end(xargv *);
			static  char	ag_c(xargv *);
			static  char	ag_c_next(xargv *);
			static  char	ag_cnn_next(xargv *);
			static  char	ag_c_advance(xargv *);
			static  char	ag_backspace(xargv *);
			static  void	ag_backword(xargv *);
			static  char	*ag_s(xargv *);
			static  char	*ag_s_next(xargv *);
			static  char	*ag_s_advance(xargv *);
			static  s32		ag_clear(xargv *);
			static  s32		ag_argc(xargv *);
			static  char	**ag_argv(xargv *);
			static  xargv	*ag_copy(xargv *);
			static  void	ag_freeall(xargv *);
			static  xargv	*ag_prepend_argv0(xargv *, char *);
			static  char	*argnext(xargv *);
		};
	}
}


#endif	/// __XCMDLINE_AG_H__








