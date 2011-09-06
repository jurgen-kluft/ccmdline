#include "xbase\x_string_std.h"

#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"
#include "xcmdline\private\ag.h"

namespace xcore
{
	namespace xcmdline
	{

		/* primitive routines for manipulation argument vectors */

		/*
		make-new-argvector              ag_new()
		reset-argvector                 ag_reset()
		flag-end-of-word                ag_eow()
		what-is-character               ag_c()
		what-is-string                  ag_s()

		read-character                  ag_c()
		read-next-character             ag_c_next()
		read-next-non-null-character    ag_cnn_next()
		read-character-advance          ag_c_advance()
		read-string                     ag_s()
		read-next-string                ag_s_next()
		read-string-advance             ag_s_advance()
		word-number                     ag_w_number()
		word-advance                    ag_w_advance()
		backspace                       ag_backspace()
		clear                           ag_clear()

		get-argc                        ag_argc()
		get-argv                        ag_argv()

		get-next-argument               argnext()
		*/

		xargv* ag_new(s32 argc, char **argv)
		{
			xargv *ag;
			ag = (xargv *)get_opt_allocator()->allocate(sizeof(xargv), 4);

			/* Note...we do not actually make a COPY of argv; but this
			* shouldn't be necessary, since *none* of the following routines
			* should ever write to ag->c and *especially* not ag->v
			* ...um, actually ag_prepend_argv0 does
			*/
			ag->v = argv;
			ag->c = argc;
			ag->iw = 0;
			ag->ic = 0;
			return(ag);
		}

		void ag_free(xargv *ag)
		{
			get_opt_allocator()->deallocate((char *)ag);
		}

		void ag_reset(xargv *ag)
		{
			ag->iw = 0;
			ag->ic = 0;
		}

		s32 ag_w_number(xargv *ag)
		{
			return ag->iw;
		}

		void ag_w_advance(xargv *ag)
		{
			++(ag->iw);             /* advance to next word */
			ag->ic=0;               /* beginning of next word */
		}
		/*      ag_eow: end of word
		flag whether current position is at end of word
		*/
		s32 ag_eow(xargv *ag)
		{
			if( ag->iw >= ag->c )
				return(TRUE);
			if( ag->ic >= (s32)x_strlen(ag->v[ag->iw]) )
				return(TRUE);
			return(FALSE);
		}
		/*      ag_end: end of command line
		flag whether current position is at end of command line 
		*/
		s32 ag_end(xargv *ag)
		{
			if( ag->iw >= ag->c )
				return(TRUE);
			if( ag_eow(ag) && ag->iw == (ag->c)-1 )
				return(TRUE);
			return(FALSE);
		}
		/*      ag_c:   return current character
		do not advance
		*/
		char ag_c(xargv *ag)
		{
			return(ag->v[ag->iw][ag->ic]);
		}

		char ag_c_next(xargv *ag)
		{
			return(ag->v[ag->iw][ag->ic+1]);
		}

		char ag_cnn_next(xargv *ag)
		{
			if( ag_c_next(ag) == '\0' )
			{
				if(ag->iw+1 >= ag->c)
					return('\0');
				else
					return(ag->v[ag->iw+1][0]);
			}
			else
				return( ag_c_next(ag) );
		}
		/*      ag_c_advance:   read current character, and advance to next
		return '\0' if end of word
		do not advance to next word
		*/
		char ag_c_advance(xargv *ag)
		{
			char    c;				/* current character */

			if( ag_eow(ag) )
				return(c='\0');		/* return NULL to signal that current*/

			/* character is past end of word     */
			c = ag->v[ag->iw][ag->ic];
			++(ag->ic);             /* advance to next character */
			return(c);
		}

		void ag_backword(xargv *ag)
		{
			ag->iw -= 1;								/*should we fix the iw to 0 if the iw is going to be set below 0 ?*/
		}

		char ag_backspace(xargv *ag)
		{
			if( --(ag->ic) < 0 )			/* if back past beginning of word */
			{
				ag->ic=0;
				if(--(ag->iw) < 0)			/* goto beginning of previous word */
				{
					ag_reset(ag);           /* if no previous, reset */
				}
				else
				{
					while( !ag_eow(ag) )    /* goto end of previous word */
						ag_c_advance(ag);
					ag_backspace(ag);       /* back to just before end */
				}
			}
			return(ag->v[ag->iw][ag->ic]);
		}
		/*      ag_s:   returns current string
		returns "" if current position is at end of word
		returns NULL if past end of argument vector
		*/
		char*	ag_s(xargv *ag)
		{
			if( ag->iw < ag->c )
				return( ag->v[ag->iw]+ag->ic );
			else   
				return( NULL );
		}

		char*	ag_s_next(xargv *ag)
		{
			if( ag->v[ag->iw][ag->ic+1] == '\0' )
			{
				if(ag->iw+1 >= ag->c)
					return(NULL);
				else  
					return(ag->v[ag->iw+1]);
			}
			else
				return( ag->v[ag->iw]+ag->ic+1 );
		}
		/*      ag_s_advance:   read current string and advance to next
		returns NULL if current string is at end
		does not check advanced string
		*/
		char*	ag_s_advance(xargv *ag)
		{
			char    *s;				/* current string */

			if( ag_eow(ag) )		/* if end of word, go to next word */
				ag_w_advance(ag);

			if( ag_eow(ag) )		/* if still at end of word,  */
				s = NULL;			/* signify by returning NULL */
			else
				s = ag->v[ag->iw] + ag->ic;

			ag_w_advance(ag);       /* advance to next word */

			return(s);
		}

		s32		ag_clear(xargv *ag)
		{
			while( !ag_end(ag) )
				argnext(ag);
			return 1;
		}

		s32		ag_argc(xargv *ag)
		{
			return ag->c - ag->iw;
		}

		char**	ag_argv(xargv *ag)
		{
			return ag->v + ag->iw;
		}

		xargv*	ag_copy(xargv *ag)
		{
			/* Here is how you would make a copy of an xargv */
			/* To free this, do not use ag_free, but ag_freeall */
			s32 i;
			xargv *agnew;
			agnew = (xargv *)get_opt_allocator()->allocate(sizeof(xargv), 4);
			agnew->c = ag->c;
			agnew->v = (char **)get_opt_allocator()->allocate(sizeof(char *) * agnew->c, 4);
			for (i=0; i< agnew->c; ++i) 
			{
				agnew->v[i] = opt_strdup(ag->v[i]);
			}
			agnew->iw = 0;
			agnew->ic = 0;
			return agnew;
		}

		void	ag_freeall(xargv *ag)
		{
			s32 i;
			for (i=0; i<ag->c; ++i)
			{
				get_opt_allocator()->deallocate(ag->v[i]);
			}
			get_opt_allocator()->deallocate((char *)ag->v);
			get_opt_allocator()->deallocate((char *)ag);
		}

		xargv*	ag_prepend_argv0(xargv *ag, char *argv0)
		{
			s32 i;
			xargv *agnew;
			agnew = (xargv *)get_opt_allocator()->allocate(sizeof(xargv), 4);
			agnew->c = ag->c - ag->iw + 1;
			agnew->v = (char **)get_opt_allocator()->allocate(agnew->c * sizeof(char *), 4);
			agnew->v[0] = argv0; /* NOTE: we are not strdup'ing */
			for (i=1; i< agnew->c; ++i) 
			{
				agnew->v[i] = ag->v[i + ag->iw - 1];
			}
			agnew->iw = 0;
			agnew->ic = 0;
			return agnew;
		}

		/* ------------------------ */
		/* return the next argument */
		/* ------------------------ */
		char*	argnext(xargv *ag)
		{
			static char nullchar='\0';
			char *s;

			s = ag_s_advance(ag);
			if( s==NULL )	
				s = &nullchar;
			return(s);
		}
	}
};

