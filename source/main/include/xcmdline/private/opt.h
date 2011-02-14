#ifndef _OPT_H
#define	_OPT_H 	/* Signal that this header file has been included */

/***
* Enable linking with C++
***/
namespace xcore
{
	typedef enum
	{
		OPT_NUL,
		OPT_INT,        OPT_SHORT,      OPT_LONG,       OPT_CHAR,
		OPT_UINT,       OPT_USHORT,     OPT_ULONG,      OPT_UCHAR,
		OPT_FLOAT,      OPT_DOUBLE,
		OPT_TOGGLE,     OPT_BOOL,
		OPT_NEGTOGGLE,  OPT_NEGBOOL,    OPT_INTLEVEL,       
		OPT_STRING,     
		/* now deprecated */
		OPT_UNDELIM, OPT_CSTRING,    OPT_UNDELIMC
	} opt_TYPE;

	/* define the deprecated typenames */
	#define OPT_FLAG OPT_TOGGLE
	#define OPT_ABSFLAG OPT_BOOL
	#define OPT_NEGFLAG OPT_NEGTOGGLE
	#define OPT_ABSNEGFLAG OPT_NEGBOOL
	#define OPT_VSTRING OPT_STRING
	#define OPT_UNSINT OPT_UINT
	#define OPT_UNDELIMV OPT_UNDELIM

	/* OPT_PFI is the ever popular: Pointer to Function returning an Integer */
	typedef	int (*OPT_PFI)(void);
	typedef int (*OPT_HOOK)(void *);
	typedef	int (*OPT_PFI_ARG)(int,char**);

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

	/* Possible values for the option mode attribute */
	typedef enum
	{
		OPT_POSITIONAL,
		OPT_DELIMITED,
		OPT_FLEXIBLE
	} opt_MODE;

	/* opt(&argc,&argv) is the main function call that does all the work.
	* it processes the options on the command line, setting variables,
	* calling hooks, etc. 
	*/

	extern void opt(int *,char ***);


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
	*  optregp: sets character name, string name, brief description and OPT_FLEXIBLE mode
	*  optexec: registers an 'exec function' (somthing that is run, then exits)
	*/

	/*    ARGUMENT LIST: &var    OPT_INT   'c'   "name"   "Brief Description" */
	extern int optrega(  void *, opt_TYPE, char,  char *, char * );
	extern int optreg(   void *, opt_TYPE, char,          char * );
	extern int optregc(  void *, opt_TYPE, char                  );
	extern int optregcb( void *, opt_TYPE, char,          char * );
	extern int optregs(  void *, opt_TYPE,        char *         );
	extern int optregsb( void *, opt_TYPE,        char *, char * );
	extern int optregp(  void *, opt_TYPE,        char *, char * );
	extern int optregf(  void *, opt_TYPE, char,  char *, char * );
	/*    ARGUMENT LIST: &var    OPT_INT   'c'   "name"   "Brief Description" */

	/*    ARGUMENTS:    "name"  hook      "Descript" */
	extern int optexec( char *, OPT_HOOK, char * );


	/* ARRAY OPTIONS, same as scalar except first argument points to
	* the size of the array. Not currently available for positionals */
	/*  ARGUMENT LIST:      &nelts, &array, OPT_INT, 'c'   "name"  "Descript" */
	extern int optrega_array(  int *, void *, opt_TYPE, char, char *, char *);
	extern int optreg_array(   int *, void *, opt_TYPE, char,         char *);
	extern int optregc_array(  int *, void *, opt_TYPE, char                );
	extern int optregcb_array( int *, void *, opt_TYPE, char,         char *);
	extern int optregs_array(  int *, void *, opt_TYPE,       char *        );
	extern int optregsb_array( int *, void *, opt_TYPE,       char *, char *);
	extern int optregp_array(  int *, void *, opt_TYPE,       char *, char *);
	extern int optregf_array(  int *, void *, opt_TYPE, char, char *, char *);

	/* Using the following routines, one can specify features (such as
	* char, longname, descript) of the variable that were left out in the
	* registration process, or one can add features (such as help, hook,
	* exec) that the registration routines do not permit.
	*/
	/* These functions use &var to identify which option */
	extern void optchar(     void *,char);
	extern void optlongname( void *,char *);
	extern void optmode(     void *,opt_MODE);
	extern void optdescript( void *,char *);
	extern void opthelp(     void *,char *);
	extern void opthook(     void *,OPT_HOOK);
	extern void optarraydelim(void *, char);
	/* These functions use option number to identify which option */
	extern void optchar_n(       int, char);
	extern void optlongname_n(   int, char *);
	extern void optmode_n(       int, opt_MODE);
	extern void optdescript_n(   int, char *);
	extern void opthelp_n(       int, char *);
	extern void opthook_n(       int, OPT_HOOK);
	extern void optarraydelim_n( int, char);  

	/* The following routines are used to set features (such as usage),
	* which are not specific to single variables, but apply to the whole
	* program.  In general, I try to use the convention optUpperCase()
	* for the registration of these more global features, while
	* optlowercase() refers to functions that apply to a specific
	* registered variable.
	*/
	extern void optUsage(char *);
	extern void optTitle(char *);
	extern void optProgName(char *);
	extern void optVersion(char *);
	extern void optEnvVarName(char *);
	extern void optDefaultString(char *);
	extern void optDefaultFile(char *);
	extern void optDisableMenu();

	extern void optAdditionalUsage(OPT_PFI);
	extern void optQuit(OPT_PFI);
	extern void optRun(OPT_PFI);
	extern void optMain(OPT_PFI_ARG);
	extern void optExitNumber(int);

	/* The following routines provide opt-related services that might
	* be useful after opt() has been set up and run.
	*/
	/* optPrintusage() writes the opt usage message to stdout
	*/
	extern void optPrintUsage();
	/* optinvoked(&var) returns the number of times the option was invoked
	* on the command line.  This function is not used in the registering
	* stage, but is used during execution of the code to see whether or
	* not a value was actually set (or if it's just using the default).
	*/
	extern int  optinvoked(void *);
	/* opt_free() is called when you are sure you won't be using any more
	* opt functions; usually, you could call it right after opt(), but
	* after opt_free(), you won't be able to use any other opt functions,
	* such as optinvoked
	*/
	extern void opt_free();


	/* The following prototypes arguably belong in opt_p.h because it is
	* not clear that the functions would ever be used in a program that
	* uses opt.  Certainly, opt itself makes great use of these, behind
	* the scenes.
	*/
	extern void optAbortRun(void);
	extern  int optBeginRun(OPT_PFI);
	extern void optAbortMain(void);
	extern  int optBeginMain(OPT_PFI_ARG, int, char **);

	extern void opt_message(char *);
	extern void opt_warning(char *);
	extern void opt_fatal(char *);

	/* BEGIN_AUTOGENERATED */
	extern int OptRegister(char *v, char c, char *n=0, char *B=0);
	extern int OptRegister(char *v, char *n, char *B=0);
	extern int OptRegister(char *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(char *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(char *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(char *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(char *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(char *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(short *v, char c, char *n=0, char *B=0);
	extern int OptRegister(short *v, char *n, char *B=0);
	extern int OptRegister(short *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(short *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(short *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(short *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(short *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(short *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned short *v, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned short *v, char *n, char *B=0);
	extern int OptRegister(unsigned short *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned short *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(unsigned short *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned short *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned short *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned short *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(int *v, char c, char *n=0, char *B=0);
	extern int OptRegister(int *v, char *n, char *B=0);
	extern int OptRegister(int *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(int *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(int *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(int *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(int *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(int *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned long *v, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned long *v, char *n, char *B=0);
	extern int OptRegister(unsigned long *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned long *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(unsigned long *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned long *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned long *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned long *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(double *v, char c, char *n=0, char *B=0);
	extern int OptRegister(double *v, char *n, char *B=0);
	extern int OptRegister(double *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(double *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(double *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(double *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(double *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(double *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(long *v, char c, char *n=0, char *B=0);
	extern int OptRegister(long *v, char *n, char *B=0);
	extern int OptRegister(long *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(long *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(long *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(long *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(long *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(long *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned int *v, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned int *v, char *n, char *B=0);
	extern int OptRegister(unsigned int *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned int *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(unsigned int *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned int *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned int *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned int *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(char **v, char c, char *n=0, char *B=0);
	extern int OptRegister(char **v, char *n, char *B=0);
	extern int OptRegister(char **v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(char **v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(char **v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(char **v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(char **v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(char **v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(float *v, char c, char *n=0, char *B=0);
	extern int OptRegister(float *v, char *n, char *B=0);
	extern int OptRegister(float *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(float *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(float *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(float *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(float *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(float *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned char *v, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned char *v, char *n, char *B=0);
	extern int OptRegister(unsigned char *v, opt_TYPE o, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned char *v, opt_TYPE o, char *n, char *B=0);
	extern int OptRegister(unsigned char *v, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned char *v, opt_MODE m, char *n, char *B=0);
	extern int OptRegister(unsigned char *v, opt_TYPE o, opt_MODE m, char c, char *n=0, char *B=0);
	extern int OptRegister(unsigned char *v, opt_TYPE o, opt_MODE m, char *n, char *B=0);

}

#endif /* _OPT_H */


