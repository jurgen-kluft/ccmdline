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
		// cdk:  if VERSION or OPT_VERSION fails to compile it's because it needs to be 
		// cdk:  defined as a compile-time symbol.
		#ifndef OPT_VERSION
			#define OPT_VERSION 3.19
		#endif

		s32	opt_menuflag = 0;

		/******************
		* Global variables 
		*/
		OPT_PFI  opt_additional_usage_fcn=NULL;
		char	*opt_program_name = NULL;

		/* Global to this file */

		static s32   opt_fileflag = OPT_FALSE;
		static s32   opt_menu_enabled = OPT_TRUE;
		static char *opt_filename = NULL;
		static char *opt_defaultfile = NULL;
		static char *opt_titlestring = NULL;
		static char *opt_usagestring = NULL;
		static char *opt_envstring=NULL;
		static char *opt_defstring=NULL;
		static char *opt_pkg_version=NULL;

		static OPT_PFI	   opt_quit_fcn=NULL;
		static OPT_PFI	   opt_run_fcn=NULL;
		static OPT_PFI_ARG opt_main_fcn=NULL;

		/****************************
		* Static Function prototypes 
		*/

		static char		*append_string(char *, char *);
		static s32		line2argv(s32 *, char ***, char *);
		static s32		l2a(char *, char **);
		static s32		break_word(s32, char *);

		/**********************
		* free various strings
		*/

		void	opt_freestrings()
		{
			OPT_FREE( opt_filename );
			OPT_FREE( opt_defaultfile );
			OPT_FREE( opt_titlestring );
			OPT_FREE( opt_usagestring );
			OPT_FREE( opt_envstring );
			OPT_FREE( opt_defstring );
			OPT_FREE( opt_pkg_version );
		}

		/***************************
		* set title, usage, etc.  *
		***************************/
		void	optTitle(char *s)
		{
			opt_setstring(&opt_titlestring,s);
		}
		char *	optgetTitle(void)
		{
			if (!ISEMPTYSTRING(opt_titlestring))
				return opt_titlestring;
			else
				return opt_program_name;
		}
		void	optVersion(char *s)
		{
			if (!ISEMPTYSTRING(s)) 
				opt_setstring(&opt_pkg_version,s);
		}
		void	optUsage(char *s)
		{
			opt_setstring(&opt_usagestring,s);
		}
		void	optEnvVarName(char *s)
		{
			opt_setstring(&opt_envstring,s);
		}
		void	optDefaultString(char *s)
		{
			opt_setstring(&opt_defstring,s);
		}
		void	optDefaultFile(char *s)
		{
			opt_setstring(&opt_defaultfile,s);
		}
		void	optQuit(OPT_PFI fcn)
		{
			opt_quit_fcn = fcn;
		}
		void	optRun(OPT_PFI fcn)
		{
			opt_run_fcn = fcn;
		}
		void	optMain(OPT_PFI_ARG fcn)
		{
			opt_main_fcn = fcn;
		}
		void	optAdditionalUsage(OPT_PFI fcn)
		{
			opt_additional_usage_fcn = fcn;
		}
		void	optDisableMenu()
		{
			opt_menu_enabled = OPT_FALSE;
		}


		static void	optWriteVersion()
		{
			char *opt_pkg;
			opt_pkg = optgetTitle();

	#ifdef VERSION
			if (ISEMPTYSTRING(opt_pkg_version))
			{
				opt_pkg_version=VERSION;
			}
	#endif

			if (!ISEMPTYSTRING(opt_pkg_version))
			{
				opt_mess_2("%s: Version %s\n",opt_pkg,opt_pkg_version);
			} 
			else 
			{
				opt_mess_1("%s: no version information\n",opt_pkg);
			}
		}



		/* ------------------------------------------------------------------ */

		/* getopts:
		* This routine is maintained only for backward compatibility.  It
		* takes argc, argv and returns the new value of argc; Now, it is
		* recommended that you use opt(), which takes &argc,&argv as arguments,
		* and then returns new argc,argv through the pointers on the arg list.
		*/
		s32 getopts(s32 argc, char **argv)
		{

			/*
			* Before processing, set the global variables
			* opt_program_name : name of routine that is running
			* opt_filename     : default name of options file
			*/
			if (ISEMPTYSTRING(opt_program_name))
				opt_program_name = short_progname(argv[0]);

			opt_filename = append_string(opt_program_name,OPT_EXT);
			//opt_readline_init(opt_program_name);

			/* Begin options processing */
			/* ------------------------ */

			/* First process default string */
			opt_lineprocess(opt_defstring);

			/* Finally, parse the command line */
			opt_process(argc-1,argv+1);

			return(argc);
		}

		/*
		* opt: This is the routine that does all the work.  It is generally called
		* from main(), after all calls to optreg() have been made.
		* It returns nothing, but new argc/argv are returned via the arglist
		*/
		void	opt(s32 *argcptr, char ***argvptr)
		{
			s32	 argc;
			char **argv;
			xargv *ag;

			argc = *argcptr;
			argv = *argvptr;

			/*
			* Before processing, set the global variables
			* opt_program_name : name of routine that is running
			* opt_filename     : default name of options file
			*/
			if (ISEMPTYSTRING(opt_program_name))
				opt_program_name = short_progname(argv[0]);

			//opt_filename = append_string(opt_program_name,OPT_EXT);
			//opt_readline_init(opt_program_name);

			/* Begin options processing */
			/* ------------------------ */

			/* First process default string */
			opt_lineprocess(opt_defstring);

			/* Second, check the default file (eg ~/.programrc) */
			//if (!ISEMPTYSTRING(opt_defaultfile)) 
			//{
			//	opt_fromfname(opt_defaultfile);
			//}

			/* Third, check environment variable */
			//if(opt_envstring != NULL)
			//{
			//	char	*s;
			//	s = getenv(opt_envstring);
			//	opt_lineprocess(s);
			//}

			/* Finally, parse the command line */
			if (argc == 0)
				return;      /* assuming there is one... */

			ag = opt_process(argc-1,argv+1);
			/* And, return the leftover argc/argv, but first prepends argv[0]
			* to this!
			*/
			if (ag == NULL)
			{
				*argcptr = 1;
				*argvptr = argv;
			} 
			else
			{
				/* make a new argc,argv */
				xargv *agnew;
				agnew = ag_prepend_argv0(ag,argv[0]);
				*argcptr = ag_argc(agnew);
				*argvptr = ag_argv(agnew);
				ag_free(agnew);         /* don't ag_freeall(agnew) because
										* that will ruin the new argv strings
										*/
			}
			ag_free(ag);

			/* Now that we are done processing, free what we no longer need */
			OPT_FREE(opt_filename);
		}
		static char * append_string(char *s, char *t)
		{
			/* input two strings "s" and "t":
			* concatenates them to get string "st"
			* which it allocates space for and returns
			*/
			s32 sts = x_strlen(s)+x_strlen(t)+1;
			char *st = (char *)get_opt_allocator()->allocate( sts, 4 );
			x_strcpy(st,sts,s);
			x_strcat(st,sts,t);
			return(st);
		}

		/* Allow the user to explicitly set the program name */
		/* Note that this routine strips off the leading directory */
		/* so that you can safely use optProgName(argv[0]) */
		void optProgName(char* progname)
		{
			opt_program_name = short_progname(progname);
		}


		char *	short_progname(char *pname)
		{
			char *p;
			s32	len;

			/*
			* inelegant routine
			* returns the shortened name of the program named [pname].
			* It (recursively) strips off leading directory or drive number
			* and trailing ".EXE" in the case of MS-DOS executable
			*/

			p = pname;
			while( *p != '/' && *p!='\\' && *p!=':' && *p!='\0' )
				++p;

			if( *p=='\0') 
			{	/* remove .EXE if it exists */
				len = x_strlen(pname);
				if( len > 4 )
				{
					if( ( 0==x_strcmp(pname+len-4,".EXE") || 0==x_strcmp(pname+len-4,".exe") ) )
						pname[len-4]='\0';
				}
				return( pname );
			}
			else
			{
				return( short_progname(++p) );
			}
		}
		/* ----------------------------------------------------------------- */

		/* ------- */
		/* process */
		/* ------- */

		/****************************************************************
		* opt_process(): 
		*	INPUT:
		*		argc, argv;
		*
		*	this is the central receiving facility for the options package.
		*	opt_process takes an argument list given by (argc,argv) and
		*	processes it,
		*	Although the input may come from either of
		*		system command line
		*		options file
		*		interactive command line
		*	the behavior is slightly different if the global opt_menuflag is set
		*/

		xargv * opt_process(s32 argc, char **argv)
		{
			char	c;
			s32		iword;
			xargv	*ag;

			/*
			* convert the argument vector (argc,argv) into
			* an xargv structure, to ease manipulations
			*/

			if( argc==0 )
				return (xargv *)NULL;

			ag = ag_new(argc,argv);

			//if (DEBUG)
			//{
			//	ag_fprint(stderr,ag);
			//}

			/* Loop through the options in the argument vector */

			while( !ag_end(ag) ) 
			{	
				if( ag_eow(ag)	)		/* if necessary...      */
					ag_w_advance(ag);	        /* advance to next word */

				if (DEBUG)
				{
					if( ag->ic != 0 )		/* this should never happen */
						opt_warning("opt_proc: not on first character");
				}

				c=ag_c_advance(ag);		/* first character of word */

				/* ------ */
				/* switch */
				/* ------ */

				switch( c ) 
				{
				case DELIM:
				case ALTDELIM:
					if( ag_eow(ag) )
					{
						/* unadorned "-" will be treated as a positional option */
						/* Try to process it as an positional option */
						ag_backspace(ag);
						if( opt_parse_positional(ag) == 0) 
						{
							/* if not successful...then stop processing */
							return ag;
						}
					}
					else if( ag_c(ag) == c )
					{
						ag_c_advance(ag);          /* eat the '-' */
						if (ag_eow(ag))
						{          /* if unadorned "--" */
							ag_w_advance(ag);      /* eat the entire "--" */
							return ag;             /* and stop processing */

							/* if "--something" */                    
						} 
						else
						{
							/* Special cases: --help, --optVersion, --menu, etc.. */
							if (0==x_strcmp("help",ag_s(ag))) 
							{ 
								long_usage();

								/*We remove the menu part*/

								//if (!opt_menuflag)
								{
									argnext(ag);
									break;	//optAbortRun();
								}
							}
							else if (0==x_strcmp("xml-opt-interface",ag_s(ag)))
							{
								//opt_write_xml_file(stdout);
								//if (!opt_menuflag)
								//	optAbortRun();
							} 
							else if (0==x_strcmp("optVersion",ag_s(ag))) 
							{
	#ifdef OPT_VERSION                        
								opt_mess_1("OPT Version %s\n",OPT_VERSION);
	#endif
								//if (!opt_menuflag)
								{
									argnext(ag);
									
									break;//optAbortRun();
								}
							} 
// 							else if (0==x_strcmp("menu",ag_s(ag)))
// 							{ /* --menu */
// 								if (!opt_menu_enabled) 
// 								{
// 									opt_warning("menu not available for this application");
// 									ag_w_advance(ag);
// 								}
// 								else
// 								{
// 									if ( !opt_menuflag )      /* If not called already */
// 										ag_w_advance(ag); /* Swallow the '--menu' */
// 									//opt_menu();       /* and call the menu */	
// 								}
// 							}
							else if (0==x_strcmp("version",ag_s(ag)) && !ISEMPTYSTRING(opt_pkg_version))
							{
								optWriteVersion();
								//if (!opt_menuflag) 
								{
 									argnext(ag);
 									break;	//optAbortRun();
								}
							} 
							else 
							{
								/* if a generic "--longoption" */
								opt_parse_longdelim(ag);
							}
						}
						/* if "-something" */
					}
					else 
					{
						iword = ag_w_number(ag);
						while( iword==ag_w_number(ag) && !ag_eow(ag) )
							opt_parse_delim(ag);
					}
					break;

				case OPTFROMFILE:
					//if (! opt_fromfname(argnext(ag)))
					//{
					//	opt_warn_1("Cannot open options file [%s]\n",opt_filename);
					//}
					break;

				case OPTTOFILE:
					//opt_tofname(argnext(ag));
					//if ( !opt_menuflag )
					//	opt_quit();
					break;

				case HELPCH:
					if( !opt_fileflag)
						opt_help(argnext(ag));
					break;

				case INTERACT:
					//if ( !opt_menuflag )		/* If not called already */
					//	opt_menu();				/* Call the menu */	
					//else {						/* otherwise */ 
					//	opt_menuflag=OPT_FALSE;			/* turn off menu */
					//	ag_clear(ag);
					//}
					break;
				case IGNOREEOL:
					ag_clear(ag);
					break;

				case RUN:
					/* If we're on the command line then treat this as a positional option */
					if (!(opt_menuflag || opt_fileflag))
						goto POSITIONAL;    /* I used goto! Hey, that felt ... good */

					/* Give precedence to optMain, then optRun */
					if ( opt_main_fcn != NULL ) 
					{
						/*
						s32 retval;
						s32 ac;
						char **av;
						ac = ag_argc(ag);
						av = ag_argv(ag);
						ag_clear(ag);
						retval = optBeginMain(opt_main_fcn,ac,av);
						if (retval == OPT_ABORT)
							optAbortMain();
						if (retval == OPT_QUIT)
							opt_quit();
						if (retval == OPT_EXIT)
							exit(opt_exit_number);
						*/
					}
					else if ( opt_run_fcn != NULL ) 
					{
						/*
						s32 retval;
						retval = optBeginRun(opt_run_fcn);
						if (retval == OPT_ABORT)
							optAbortRun();
						if (retval == OPT_QUIT)
							opt_quit();
						if (retval == OPT_EXIT)
							exit(opt_exit_number);
						*/
					} 
					else 
					{
						/* if opt_run_fcn is not set, and 'RUN' is called
						* from the menu, then exit the menu, so that
						* the routine is run, and then program will exit
						*/
						if ( opt_menuflag )
						{
							opt_menuflag=OPT_FALSE;			/* turn off menu */
							ag_clear(ag);
						}
						else
						{
							/* RUN called from a file or the command line:
							* There is no reason to be doing this.
							*/
							opt_warning("No Run Function has been registered");
						}
						break;

						/* Note that the QUITCH '.' is quite a common positional option! */
						/* So we only treat it as a quit signal if using the menu */
				case QUITCH:
					goto POSITIONAL;   /* There I go again... */

				case BANG:
					/* If there is a BANG as the first character of a
					* menuline, then we shouldn't even get here.
					*/
					if (!opt_menuflag)
						goto POSITIONAL;     /* Can't ... stop ... */
					else {
						opt_warn_1("Shell to system invalid unless \'%c\' is the first character in the line\n",BANG);
					}

				case ' ':		/* blanks and empty characters, ignore */
				case '\t':
				case '\0':
					break;

	POSITIONAL:
				default:
					/* in the default case, option may be positional */
					/* ---------------------------------------------- */
					//if(DEBUG) 
					//	ag_fprint(stderr,ag);

					ag_backspace(ag);
					
					if(DEBUG)
					{
						opt_mess_1("opt_proc: in default section [%s]\n",ag_s(ag));
						//ag_fprint(stderr,ag);
					}
					
					/* Try to process it as an positional option */
					if( opt_parse_positional(ag) == 0)
					{
						/* if not successful...then stop processing */
						//if (DEBUG)
						//	ag_fprint(stderr,ag);
						return ag;
					}
					break;
					/* last case */
					}
				}
			}
			ag_free(ag);
			return (xargv *)NULL;
		}/*opt_process*/

		/* opt_lineprocess()
		* process a string, by converting it first into an argument vector
		* then calling opt_process
		* return the number of processed arguments
		*/

		s32 opt_lineprocess(char *line)
		{
			s32 i,nargc;
			char **nargv;

			if( line==NULL || *line=='\0') return(0);

			line2argv(&nargc,&nargv,line);
			opt_process(nargc,nargv);

			/* Now free nargv */
			for (i=0; i<nargc; ++i)
			{
				get_opt_allocator()->deallocate(nargv[i]);
			}
			get_opt_allocator()->deallocate(nargv);    
			return(nargc);
		}
		/*	line2argv(): converts a one-line string into an argument vector */

		static s32 line2argv(s32 *pargc, char ***pargv, char *line)
		{
			char *lincp;
			if(line==NULL)
			{	
				*pargc = 0;
				**pargv = NULL;
				return(0);
			}
			/*
			* First thing to do is copy the line into
			* a buffer ("lincp") so that input line will
			* not be corrupted -- also so that input line
			* doesn't have to be double null terminated
			*/
			s32 lincps = (x_strlen(line)+2)*sizeof(char);
			lincp = (char *)get_opt_allocator()->allocate(lincps, 4);
			x_strcpy(lincp,lincps,line);
			/*
			* Next step is to double null terminate 
			* the copied line
			*/
			lincp[x_strlen(line)+1]='\0';
			/* count words in line */
			*pargc = l2a(lincp,NULL);
			/* allocate array for arg vector */
			*pargv = (char **)get_opt_allocator()->allocate(((*pargc)+1)*sizeof(char *),4);
			/* fill arg vector with words in line */
			l2a(lincp,*pargv);
			/* we are done with lincp, free it... */
			get_opt_allocator()->deallocate(lincp);

			return(*pargc);
		}

		#define	QUOTE	'\"'
		#define	BKSLASH	'\\'

		/* l2a */
		/*	l2a() is slave routine to line2argv()           */
		/*	if argv==NULL then count the words in the line	*/
		/*	if argv!=NULL then put those words into argv[]  */
		/* 	WARNINGS:
		*	l2a assumes that input line is double-null terminated!!
		*	the line buffer is pointed to by argv[]'s so do not
		*	free the input line buffer
		*/
		/* quoted material counts as a single word */
		/* so that -s"string with spaces"-g is parsed */
		/* into 
		-s
		string with spaces
		-g
		*/
		/* Comment: should be able to escape with backslash */

		static s32 l2a(char *line, char **argv)
		{
			s32 inquote=OPT_FALSE;	/* flag: inside quotation */
			s32 iarg;

			for(iarg=0; *line != '\0'; ++iarg) 
			{
				if(!inquote) 
				{
					while( x_isspace(*line) )
						++line;	/* skip leading blank spaces */

					if( *line == QUOTE ) 
					{
						inquote=OPT_TRUE;
						++line;	/* skip past leading quote */
					}
				}

				if(argv!=NULL)			/* point to */
					argv[iarg] = line;	/* first character of line */

				while( !break_word(inquote,line) )
					++line;

				if( *line==QUOTE )
				{		/* toggle quote */ 
					inquote = (inquote==OPT_TRUE ? OPT_FALSE : OPT_TRUE );
					if(argv==NULL)	++line;	/* skip past quote */
				}

				if(argv!=NULL)
				{
					*line='\0';	/* Null terminate string */
					++line;		/* and go to next character */
				}
				else 
				{
					while( x_isspace(*line) )
						++line;		/* skip trailing blanks */
				}
			}

			/* We do the following to make the line free-able */
			if (argv != NULL)
			{
				s32 i;
				for (i=0; i<iarg; ++i)
				{
					argv[i] = opt_strdup(argv[i]);
				}
			}

			/* Of course we will still need to free the argv's... */

			return(iarg);
		}

		static s32 break_word(s32 inquote, char *line)
		{
			if( *line == '\0' || *line==QUOTE )
				return(OPT_TRUE);
			if( !inquote && x_isspace(*line) )
				return(OPT_TRUE);

			return(OPT_FALSE);
		}


		/* --------------------------- */
		/* write out a usage statement */
		/* --------------------------- */

		/* A global function which can be used to programmatically */
		/* print the usage. */
		void optPrintUsage()
		{
			long_usage();
		}

		void short_usage(void)
		{
			opt_message(opt_titlestring);
			opt_mess_1("Usage: %s ",opt_program_name);
			opt_mess_1("%s\n", opt_usagestring ? opt_usagestring : "[options] ");
			if (opt_menu_enabled)
				opt_mess_1("To invoke the menu, type:\n\t%s --menu\n", opt_program_name);
		}

		void long_usage(void)
		{
			short_usage();
			opt_usage();
		}


		/* ------------------- */
		/* basic help function */
		/* ------------------- */

		void	opt_help(char *s)
		{
			if(s==NULL || *s=='\0')
			{
				opt_mess_2("\t%c %-20s\n",DELIM,"Options delimiter"); 
				opt_mess_2("\t%c %-20s\n",HELPCH,"Help");
				opt_mess_2("\t%c %-20s\n",RUN,"Run program and return to menu");
				opt_mess_2("\t%c %-20s\n",BANG,"Shell to Operating System"); 
				if (opt_menuflag)
					opt_mess_2("\t%c %-20s\n",INTERACT,"Exit menu"); 
				else
					opt_mess_2("\t%c %-20s\n",INTERACT,"Invoke menu"); 
				opt_mess_2("\t%c %-20s\n",ADDITIONAL_OPTS,"Additional options");
				opt_mess_3("\t%c<filename> %-20s [%s]\n",OPTFROMFILE,
					"Get options from file",opt_filename);
				opt_mess_4("\t%c%c %-2s [%s]\n",OPTFROMFILE,OPTFROMFILE,
					"Get options from file",opt_filename);
				opt_mess_2("\t%c<filename> %-20s\n",OPTTOFILE,
					"Put options in file");
				opt_mess_4("\t%c%c %-2s [%s]\n",OPTTOFILE,OPTTOFILE,
					"Put options in file",opt_filename);
				opt_mess_2("\t%c %-20s\n",QUITCH,"Quit");
			} 
			else 
			{
				if( s[1]=='\0')
				{
					opt_get_help(s[0]); 
				}
				else
				{
					opt_mess_1("Help unavailable for: [%s]\n",s);
				}
			}
		}

	}
}
