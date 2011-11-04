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

		OPT_PFI  Opt_Reg::opt_additional_usage_fcn=NULL;
		char	*Opt_Reg::opt_program_name = NULL;

		/* Global to this file */

		static s32   opt_menuflag = 0;
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

		static char	*append_string(char *, char *);
		static s32		line2argv(s32 *, char ***, char *);
		static s32		l2a(char *, char **);
		static s32		break_word(s32, char *);

		/**********************
		* free various strings
		*/

		void	Opt_Reg::opt_freestrings()
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
		void	Opt_Reg::optTitle(char *s)
		{
			Opt_Util::opt_setstring(&opt_titlestring,s);
		}
		
		/* Allow the user to explicitly set the program name */
		/* Note that this routine strips off the leading directory */
		/* so that you can safely use optProgName(argv[0]) */
		void Opt_Reg::optProgName(char* progname)
		{
			Opt_Reg::opt_program_name = Opt_Proc::short_progname(progname);
		}
		void	Opt_Reg::optVersion(char *s)
		{
			if (!ISEMPTYSTRING(s)) 
				Opt_Util::opt_setstring(&opt_pkg_version,s);
		}
		void	Opt_Reg::optUsage(char *s)
		{
			Opt_Util::opt_setstring(&opt_usagestring,s);
		}
		void	Opt_Reg::optEnvVarName(char *s)
		{
			Opt_Util::opt_setstring(&opt_envstring,s);
		}
		void	Opt_Reg::optDefaultString(char *s)
		{
			Opt_Util::opt_setstring(&opt_defstring,s);
		}
		void	Opt_Reg::optAdditionalUsage(OPT_PFI fcn)
		{
			Opt_Reg::opt_additional_usage_fcn = fcn;
		}
		void	optDefaultFile(char *s)
		{
			Opt_Util::opt_setstring(&opt_defaultfile,s);
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
		void	optDisableMenu()
		{
			opt_menu_enabled = OPT_FALSE;
		}


		static void	optWriteVersion()
		{
			char *opt_pkg;
			opt_pkg = Opt_Proc::optgetTitle();

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
		static s32 getopts(s32 argc, char **argv)
		{

			/*
			* Before processing, set the global variables
			* opt_program_name : name of routine that is running
			* opt_filename     : default name of options file
			*/
			if (ISEMPTYSTRING(Opt_Reg::getProgramName()))
				Opt_Reg::optProgName(argv[0]);
			opt_filename = append_string(Opt_Reg::getProgramName(),OPT_EXT);
			//opt_readline_init(opt_program_name);

			/* Begin options processing */
			/* ------------------------ */

			/* First process default string */
			Opt_Proc::opt_lineprocess(opt_defstring);

			/* Finally, parse the command line */
			Opt_Proc::opt_process(argc-1,argv+1);

			return(argc);
		}

		/*
		* opt: This is the routine that does all the work.  It is generally called
		* from main(), after all calls to optreg() have been made.
		* It returns nothing, but new argc/argv are returned via the arglist
		*/
		void	Opt_Proc::opt(s32 *argcptr, char ***argvptr)
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
			if (ISEMPTYSTRING(Opt_Reg::opt_program_name))
				Opt_Reg::opt_program_name = Opt_Proc::short_progname(argv[0]);

			//opt_filename = append_string(opt_program_name,OPT_EXT);
			//opt_readline_init(opt_program_name);

			/* Begin options processing */
			/* ------------------------ */

			/* First process default string */
			Opt_Proc::opt_lineprocess(opt_defstring);

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

			ag = Opt_Proc::opt_process(argc-1,argv+1);
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
				agnew = Ag_Func::ag_prepend_argv0(ag,argv[0]);
				*argcptr = Ag_Func::ag_argc(agnew);
				*argvptr = Ag_Func::ag_argv(agnew);
				Ag_Func::ag_free(agnew);         /* don't ag_freeall(agnew) because
										* that will ruin the new argv strings
										*/
			}
			Ag_Func::ag_free(ag);

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
			char *st = (char *)Opt_Allocator::get_opt_allocator()->allocate( sts, 4 );
			x_strcpy(st,sts,s);
			x_strcat(st,sts,t);
			return(st);
		}

		void	Opt_Proc::opt_get_help(char c)
		{
			s32 n;
			n = Opt_Proc::opt_char_number(c);
			if (OPT_isvalidnum(n) && Opt_Reg::optlist[n].help != NULL)
			{
				opt_mess_2("%c: %s\n",c,Opt_Reg::optlist[n].help);

				//test whether the result is right
				//std::cout<< c << "     " <<optlist[n].help<<std::endl;
			}
			else
				opt_mess_1("Help unavailable for \'%c\'\n",c);
		}

		char *	Opt_Proc::short_progname(char *pname)
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
				return( Opt_Proc::short_progname(++p) );
			}
		}

		char *	Opt_Proc::optgetTitle(void)
		{
			if (!ISEMPTYSTRING(opt_titlestring))
				return opt_titlestring;
			else
				return Opt_Reg::opt_program_name;
		}

		/* opt_XXX_number(): Get number corresponding to option name; return
		* a value of -1 if the option does not exist.
		*  opt_char_number(c) takes as input the single-character name;
		*  opt_longname_number(longname) takes as input the long name
		*  opt_number(void *) takes as input pointer to variable
		*/

		s32	Opt_Proc::opt_char_number(char c)
		{
			s32 i;                         /* see which registered option */
			for(i=0; i<Opt_Reg::opt_nreg; ++i)
			{
				if( c == Opt_Reg::optlist[i].name )
					return i;
			}
			return -1;
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

		char*	Opt_Proc::optstrval(s32 i)
		{
			return Opt_Reg::opt_getstrval(Opt_Reg::optlist[i].value,Opt_Reg::optlist[i].type);
		}

		/* optstrtyp:
		* returns the type of the ith registered option as a string
		* Thus if x = 12.6 is a f32 , this returns the string "<f32>"
		* The return value is a pointer to a static string which is
		* overwritten with each call
		*/
		char*	Opt_Proc::optstrtyp(s32 i)
		{
			static char	sttyp_buf[80];
			opt_TYPE o;

			o = (Opt_Reg::optlist[i].array) ? Opt_Reg::optlist[i].array->base_type : Opt_Reg::optlist[i].type;

			switch( o ) 
			{
			case OPT_INT:         x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<INT"); break;
			case OPT_UINT:        x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<UINT"); break;
			case OPT_SHORT:       x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<s16"); break;
			case OPT_LONG:        x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<LONG"); break;
			case OPT_USHORT:      x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<Us16"); break;
			case OPT_ULONG:       x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<ULONG"); break;
			
			// Add this two types to deal with the s8 and u8
			case OPT_BYTE:        x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<s8"); break;
			case OPT_UBYTE:       x_strcpy(sttyp_buf, sizeof(sttyp_buf), "<u8"); break;
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
				Opt_Util::opt_fatal("usage: undefined o-type");
			}

			/* If it's an array, then add " ARRAY" to the name of the type */
			if ( Opt_Reg::optlist[i].array )
			{
				x_strcat(sttyp_buf, sizeof(sttyp_buf), " ARRAY");
			}
			/* finish with closing bracket */
			x_strcat(sttyp_buf, sizeof(sttyp_buf), ">");
			return sttyp_buf;
		}

		s32	Opt_Proc::opt_parse_positional(xargv *ag)
		{
			s32     i;

			/* Find the first positional option that has not been invoked and
			* set that to the argument string.  Note that once a positional
			* option has been invoked, it cannot be un-invoked.
			*/

			for (i=0; i<Opt_Reg::opt_nreg; ++i) 
			{
				if (Opt_Reg::optlist[i].invoked || Opt_Reg::optlist[i].mode == OPT_DELIMITED) 
					continue;
				return Opt_Reg::opt_action(i,ag);
			}
			/* If we get here, we didn't find an un-invoked positional option */
			return 0;
		}

		/* ---------------------------------- */
		/* get and process a delimited option */
		/* ---------------------------------- */

		s32	Opt_Proc::opt_parse_delim(xargv *ag)
		{
			s32      i;
			char     c;

			c = Ag_Func::ag_c_advance(ag);		/* first character gives name of option */
			i = Opt_Proc::opt_char_number(c);		/* this is number of option w/ name 'c' */

			if(! OPT_isvalidnum(i))
			{	/* if invalid option name */
				opt_warn_1("%c not a registered option name\n",c);
				return 0;
			}
			return Opt_Reg::opt_action(i,ag);
		}

		s32	Opt_Proc::opt_parse_longdelim(xargv *ag)
		{
			int      i;
			char     c;
			char     *sc,*scptr;

			/* this code parses input options of the form --var=value */

			sc = Opt_Allocator::opt_strdup(Ag_Func::ag_s(ag));      /* long option name is a string */
			scptr = sc;
			while( !Ag_Func::ag_eow(ag) ) 
			{
				c=Ag_Func::ag_c_advance(ag);
				if (c == '=') 
				{
					*scptr = '\0';
					break;
				}
				++scptr;
			}

			i  = Opt_Reg::opt_longname_number(sc);   /* this is number of option w/ name `sc' */
			if (! OPT_isvalidnum(i))
			{
				opt_warn_1("%s not a registered option name\n",sc);
				return 0;
			}
			Opt_Allocator::get_opt_allocator()->deallocate((char *)sc); 
			return Opt_Reg::opt_action(i,ag);
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

		xargv * Opt_Proc::opt_process(s32 argc, char **argv)
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

			ag = Ag_Func::ag_new(argc,argv);

			//if (DEBUG)
			//{
			//	ag_fprint(stderr,ag);
			//}

			/* Loop through the options in the argument vector */

			while( !Ag_Func::ag_end(ag) ) 
			{	
				if( Ag_Func::ag_eow(ag)	)		/* if necessary...      */
					Ag_Func::ag_w_advance(ag);	        /* advance to next word */

				if (DEBUG)
				{
					if( ag->ic != 0 )		/* this should never happen */
						Opt_Util::opt_warning("opt_proc: not on first character");
				}

				c=Ag_Func::ag_c_advance(ag);		/* first character of word */

				/* ------ */
				/* switch */
				/* ------ */

				switch( c ) 
				{
				case DELIM:
				case ALTDELIM:
					if( Ag_Func::ag_eow(ag) )
					{
						/* unadorned "-" will be treated as a positional option */
						/* Try to process it as an positional option */
						Ag_Func::ag_backspace(ag);
						if( Opt_Proc::opt_parse_positional(ag) == 0) 
						{
							/* if not successful...then stop processing */
							return ag;
						}
					}
					else if( Ag_Func::ag_c(ag) == c )
					{
						Ag_Func::ag_c_advance(ag);          /* eat the '-' */
						if (Ag_Func::ag_eow(ag))
						{          /* if unadorned "--" */
							Ag_Func::ag_w_advance(ag);      /* eat the entire "--" */
							return ag;             /* and stop processing */

							/* if "--something" */                    
						} 
						else
						{
							/* Special cases: --help, --optVersion, --menu, etc.. */
							if (0==x_strcmp("help",Ag_Func::ag_s(ag))) 
							{ 
								Opt_Proc::long_usage();

								/*We remove the menu part*/

								//if (!opt_menuflag)
								{
									Ag_Func::argnext(ag);
									break;	//optAbortRun();
								}
							}
							else if (0==x_strcmp("xml-opt-interface",Ag_Func::ag_s(ag)))
							{
								//opt_write_xml_file(stdout);
								//if (!opt_menuflag)
								//	optAbortRun();
							} 
							else if (0==x_strcmp("optVersion",Ag_Func::ag_s(ag))) 
							{
	#ifdef OPT_VERSION                        
								opt_mess_1("OPT Version %s\n",OPT_VERSION);
	#endif
								//if (!opt_menuflag)
								{
									Ag_Func::argnext(ag);
									
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
							else if (0==x_strcmp("version",Ag_Func::ag_s(ag)) && !ISEMPTYSTRING(opt_pkg_version))
							{
								optWriteVersion();
								//if (!opt_menuflag) 
								{
 									Ag_Func::argnext(ag);
 									break;	//optAbortRun();
								}
							} 
							else 
							{
								/* if a generic "--longoption" */
								Opt_Proc::opt_parse_longdelim(ag);
							}
						}
						/* if "-something" */
					}
					else 
					{
						iword = Ag_Func::ag_w_number(ag);
						while( iword==Ag_Func::ag_w_number(ag) && !Ag_Func::ag_eow(ag) )
							Opt_Proc::opt_parse_delim(ag);
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
						Opt_Proc::opt_help(Ag_Func::argnext(ag));
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
					Ag_Func::ag_clear(ag);
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
							Ag_Func::ag_clear(ag);
						}
						else
						{
							/* RUN called from a file or the command line:
							* There is no reason to be doing this.
							*/
							Opt_Util::opt_warning("No Run Function has been registered");
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

					Ag_Func::ag_backspace(ag);
					
					if(DEBUG)
					{
						opt_mess_1("opt_proc: in default section [%s]\n",Ag_Func::ag_s(ag));
						//ag_fprint(stderr,ag);
					}
					
					/* Try to process it as an positional option */
					if( Opt_Proc::opt_parse_positional(ag) == 0)
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
			Ag_Func::ag_free(ag);
			return (xargv *)NULL;
		}/*opt_process*/

		/* opt_lineprocess()
		* process a string, by converting it first into an argument vector
		* then calling opt_process
		* return the number of processed arguments
		*/

		s32 Opt_Proc::opt_lineprocess(char *line)
		{
			s32 i,nargc;
			char **nargv;

			if( line==NULL || *line=='\0') return(0);

			line2argv(&nargc,&nargv,line);
			Opt_Proc::opt_process(nargc,nargv);

			/* Now free nargv */
			for (i=0; i<nargc; ++i)
			{
				Opt_Allocator::get_opt_allocator()->deallocate(nargv[i]);
			}
			Opt_Allocator::get_opt_allocator()->deallocate(nargv);    
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
			lincp = (char *)Opt_Allocator::get_opt_allocator()->allocate(lincps, 4);
			x_strcpy(lincp,lincps,line);
			/*
			* Next step is to double null terminate 
			* the copied line
			*/
			lincp[x_strlen(line)+1]='\0';
			/* count words in line */
			*pargc = l2a(lincp,NULL);
			/* allocate array for arg vector */
			*pargv = (char **)Opt_Allocator::get_opt_allocator()->allocate(((*pargc)+1)*sizeof(char *),4);
			/* fill arg vector with words in line */
			l2a(lincp,*pargv);
			/* we are done with lincp, free it... */
			Opt_Allocator::get_opt_allocator()->deallocate(lincp);

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
					argv[i] = Opt_Allocator::opt_strdup(argv[i]);
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
		void Opt_Proc::optPrintUsage()
		{
			Opt_Proc::long_usage();
		}

		void Opt_Proc::short_usage(void)
		{
			Opt_Util::opt_message(opt_titlestring);
			opt_mess_1("Usage: %s ",Opt_Reg::opt_program_name);
			opt_mess_1("%s\n", opt_usagestring ? opt_usagestring : "[options] ");
			if (opt_menu_enabled)
				opt_mess_1("To invoke the menu, type:\n\t%s --menu\n", Opt_Reg::opt_program_name);
		}

		void Opt_Proc::long_usage(void)
		{
			Opt_Proc::short_usage();
			Opt_Proc::opt_usage();
		}

		s32 Opt_Proc::optinvoked(void *v) 
		{ 
			return Opt_Reg::optinvoked_n( Opt_Reg::opt_number(v) );
		}
		/* ------------------- */
		/* basic help function */
		/* ------------------- */

		void	Opt_Proc::opt_help(char *s)
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
					Opt_Proc::opt_get_help(s[0]); 
				}
				else
				{
					opt_mess_1("Help unavailable for: [%s]\n",s);
				}
			}
		}

		/* opt_usage: Write a usage statement describing registered options.
		* This is what the program writes in response to '--help' on the
		* command line.
		*/
		#define OPTUSAGEDEBUG 0
		#define OPTDESCRIPTWIDTH  43
		#define OPTDESCRIPTOFFSET 38
		#define OPTMAXLONGNAMELEN 14

		void	Opt_Proc::opt_usage(void)
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
			for (i = 0; i < Opt_Reg::opt_nreg; ++i) 
			{
				if (Opt_Reg::optlist[i].mode != OPT_DELIMITED) 
				{
					if (initial) 
					{
						Opt_Util::opt_message("The arguments are:\n");
						initial = 0;
					}
					/* Get a suitably formatted version of the description  */
					/* descript may be NULL, but opt_justify always returns */
					/* a valid (perhaps empty) string.                      */
					justified_descript = Opt_Util::opt_justify(Opt_Reg::optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
					if (OPTUSAGEDEBUG) 
						x_printf("%2d u ",i);

					if (!ISEMPTYSTRING(Opt_Reg::optlist[i].descript))
						opt_mess_3(pformat,Opt_Reg::optlist[i].longname,Opt_Proc::optstrtyp(i),justified_descript);
					else
						opt_mess_2(uformat,Opt_Proc::optstrtyp(i),justified_descript);

#ifdef COUTDEBUG
					std::cout << "deallocate memory size" << sizeof(justified_descript) << std::endl;
#endif
					Opt_Allocator::get_opt_allocator()->deallocate(justified_descript);
				}
			}

			/* Second we print the delimited options */
			initial = 1;
			for(i=0; i<Opt_Reg::opt_nreg; ++i) 
			{
				if (Opt_Reg::optlist[i].mode != OPT_POSITIONAL && (Opt_Reg::optlist[i].name != '\0' || !ISEMPTYSTRING(Opt_Reg::optlist[i].longname))) 
				{
					if (initial) 
					{
						Opt_Util::opt_message("The options are:\n");
						initial = 0;
					} 
					/* Get a suitably formatted version of the description */
					/* If the option is positional, then refer to the argument description */
					if (Opt_Reg::optlist[i].mode == OPT_FLEXIBLE) 
					{
						justified_descript = Opt_Util::opt_justify("(See argument description above)", OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
					} 
					else if (Opt_Reg::optlist[i].longname && x_strlen(Opt_Reg::optlist[i].longname) > OPTMAXLONGNAMELEN) 
					{
						justified_descript = Opt_Util::opt_justify(Opt_Reg::optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET, OPTDESCRIPTOFFSET,0);
					} 
					else 
					{
						justified_descript = Opt_Util::opt_justify(Opt_Reg::optlist[i].descript, OPTDESCRIPTWIDTH, OPTDESCRIPTOFFSET,0,0);
					}

					if (Opt_Reg::optlist[i].name == '\0') 
					{
						if(OPTUSAGEDEBUG) 
							x_printf("a ");

						if (x_strlen(Opt_Reg::optlist[i].longname) > OPTMAXLONGNAMELEN) 
						{
							opt_mess_3(aformat,Opt_Reg::optlist[i].longname, Opt_Proc::optstrtyp(i),"");
							opt_mess_1("%s\n",justified_descript);
						} 
						else
						{
							opt_mess_3(aformat,Opt_Reg::optlist[i].longname, Opt_Proc::optstrtyp(i),justified_descript);
						}            
					}
					else
					{
						if (!ISEMPTYSTRING(Opt_Reg::optlist[i].longname)) 
						{
							if(OPTUSAGEDEBUG) 
								x_printf("l ");

							if (x_strlen(Opt_Reg::optlist[i].longname) > OPTMAXLONGNAMELEN) 
							{
								opt_mess_4(lformat,Opt_Reg::optlist[i].name,Opt_Reg::optlist[i].longname, Opt_Proc::optstrtyp(i),"");
								opt_mess_1("%s\n",justified_descript);
							} 
							else
							{
								opt_mess_4(lformat,Opt_Reg::optlist[i].name,Opt_Reg::optlist[i].longname, Opt_Proc::optstrtyp(i),justified_descript);
							}            
						} 
						else 
						{
							if (OPTUSAGEDEBUG) 
								x_printf("d ");
							opt_mess_3(dformat,Opt_Reg::optlist[i].name, Opt_Proc::optstrtyp(i),justified_descript);
						}
					}
#ifdef COUTDEBUG
					std::cout << "deallocate memory size" << sizeof(justified_descript) << std::endl;
#endif
					Opt_Allocator::get_opt_allocator()->deallocate(justified_descript);
				}
			}

			if (Opt_Reg::opt_additional_usage_fcn != NULL)
			{
				/* precede additional usage by -- to indicate that we
				* are finished with the options
				*/
				Opt_Util::opt_message(" --\n");
				/* return value is ignored */
				(*Opt_Reg::opt_additional_usage_fcn)();
			}
		}

	}
}
