/**************************
*  some useful utilities *
**************************/
/*
*  *** NO LONGER AVAILABLE!!
*  *** 1) Variable argument lists: for error messages
*  ***   opt_errmess(), gives message and then aborts run
*     
*  2) Long Jump: for aborting run() without losing the entire job
*     optBeginRun()
*     optAbortRun()
*     2a) Signal: trapping ^C so that it aborts run()
*     
*/
/*  Can enable longjmp with: #define HAVE_LONGJMP 1
*  Only if longjmp is enabled can signal be enabled, using
*                           #define HAVE_SIGNAL 1
*  If this version of opt came with a configure script, then those
*  values should be set automatically by the system at configure time.
*/

#include "xbase\x_string_std.h"

#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"

//#define COUTDEBUG

#ifdef COUTDEBUG
	#include <iostream>
#endif
namespace xcore
{
	namespace xcmdline
	{
		static x_iallocator*	sCmdLineOptAllocator = NULL;
		void			set_opt_allocator(x_iallocator* allocator)
		{
			sCmdLineOptAllocator = allocator;
		}
		x_iallocator*	get_opt_allocator()
		{
			return sCmdLineOptAllocator;
		}

		/* If these are not defined on the compile command line, then assume
		* the worst, that you don't have anything!
		*/
		#ifndef HAVE_LONGJMP
			#define HAVE_LONGJMP 0
		#endif
		#ifndef HAVE_SIGNAL
			#define HAVE_SIGNAL 0
		#endif
		#ifndef RETSIGTYPE
			#define RETSIGTYPE void
		#endif

		/* Provide message/warning/fatal functions */
		/* Ordinary messages go to stdout, but warn/fatal messages to stderr;
		* (This convention adopted at suggestion of Jason V. Morgan) */
		void	opt_message(char *s)
		{
			//if (s) fputs(s,stdout);
		}
		void	opt_warning(char *s)
		{
			//fputs("OPT Warning: ",stderr);
			//if (s) fputs(s,stderr);
			//fputs("\n",stderr);
		}
		void	opt_fatal(char *s)
		{
			//fputs("OPT Fatal error: ",stderr);
			//if (s) fputs(s,stderr);
			//exit(opt_exit_number);
		}

		void	opt_setstring(char **s, char *t)
		{
			if (*s) 
				get_opt_allocator()->deallocate(*s);
			*s = opt_strdup(t);
		}

		/* A function to justify text to a given width, useful for      */
		/* printing long description strings. Returns a pointer to      */
		/* a malloc'd string buffer which must be free'd by the caller. */
		char*	opt_justify(char* s, s32 width, s32 indent, s32 initial, char* prefix) 
		{
			s32 i, j, n, p, len, approxLines, bufSize;
			s32 k;
			char* buf;

			/* If s is null or empty, then return an empty string */
			if (ISEMPTYSTRING(s)) 
			{
				buf = (char*) get_opt_allocator()->allocate(1, 4);
				buf[0] = '\0';
				return buf;
			}

			/* Allocate a nice big buffer for the justified string */
			/* (This is very generous estimate...) */
			len = x_strlen(s);
			approxLines = len / width + 1;
			bufSize = initial + width + 1 +         /* The first line, inc. newline */
				(1 + indent + width) * approxLines * 3; /* Justified lines, assuming bad line breaks */

			buf = (char*) get_opt_allocator()->allocate(bufSize + 1, 4);
#ifdef COUTDEBUG
			std::cout << "allocate memory size: " << bufSize+1 << std::endl;
#endif

			if (!buf)
				opt_fatal("unable to allocate memory for string processing");

			j = 0;

			/* Insert initial indent and prefix */
			for (n = 0; n < initial; ++n)
				buf[j++] = ' ';

			if (prefix)
			{
				x_strcpy(buf + j, bufSize-j, prefix);
				j += x_strlen(prefix);
			}

			/* Scan through input, breaking up into words */
			p = 0;   /* Position with justified column */
			for (i = 0; i < len; ++i) 
			{
				/* Skip any leading whitespace */
				k = 0;
				while (s[i+k]==' ' || s[i+k]=='\t' || s[i+k]=='\r' || s[i+k]=='\n')
					++k;
				i += k;
				if (i >= len)
					break;

				/* Search for next whitespace char */
				k = 0;
				while (s[i+k]!=' ' && s[i+k]!='\t' && s[i+k]!='\r' && s[i+k]!='\n')
					++k;

				/* Do we need a line break? */
				if (p + k + 1 >= width && p != 0) 
				{
					buf[j++] = '\n';
					for (n = 0; n < indent; ++n)
						buf[j++] = ' ';
				
					if (prefix) 
					{
						x_strcpy(buf + j, bufSize-j, prefix);
						j += x_strlen(prefix);
					}
					p = 0;
				}
				/* Add the word to the output buffer */
				/* If not at the beginning of the line, add a space */
				if (p != 0) 
				{
					buf[j++] = ' ';
					++p;
				}

				/* Copy over the word */

				/*
				*	the original source code has a bug
				*	now change the k to k+1
				*/
				x_strcpy(buf + j, k+1, s + i);
				p += k;
				i += k;
				j += k;
			}
			/* Terminate output buffer */
			buf[j] = '\0';

			/* Return the justified buffer - remember to free this! */
			return buf;
		}

		s32  optBeginRun(OPT_PFI run)
		{
			return (*run)(); 
		}

		void optAbortRun()
		{
			//exit(opt_exit_number); 
		}

		s32  optBeginMain(OPT_PFI_ARG run_args, s32 argc, char **argv)
		{
			return (*run_args)(argc,argv);
		}

		void optAbortMain()
		{
			optAbortRun(); 
		}

		char* opt_strdup(char *s)
		{
			char *sdup;
			s32 i,n;
			n = x_strlen(s);
			sdup = (char*)get_opt_allocator()->allocate(n+1, 4);
			for (i=0; i<n; ++i)
				sdup[i] = s[i];
			sdup[n]='\0';
			return sdup;
		}
	}
}
