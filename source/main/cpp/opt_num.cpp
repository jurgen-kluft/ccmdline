#include "xbase\x_string_std.h"

#include "xcmdline\private\opt.h"
#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_util.h"
#include "xcmdline\private\opt_num.h"

namespace xcore
{
	namespace xcmdline
	{
		#define IsSpace(t) x_isspace((s32)(t))
		#define IsDigit(t) x_isdigit((s32)(t))
		#define IsSign(t)  ((t)=='+' || (t)=='-')

		s32 Opt_Num::opt_isvalidnumber(char *t)
		{
			s32 status = OPT_NUM_VALID; /* innocent until proven guilty */

			if (!t) return xFALSE;       /* null string is not a number */

			while( *t != '\0' && IsSpace(*t) ) ++t;   /* leading white space */

			if   ( *t != '\0' && IsSign(*t)  )
			{      /* leading sign */
				if ( *t == '-' ) status |= OPT_NUM_NEG;
				++t;  
			}

			if   (*t=='e' || *t=='E' ) return xFALSE;  /* 'e5' not allowed! */

			while( *t != '\0' && IsDigit(*t) ) ++t;   /* many digits */
			if   ( *t != '\0' && ( *t=='.' ) ) 
			{      /* at most one decimal point */
				status |= OPT_NUM_FLOAT;
				++t;
			}
			while( *t != '\0' && IsDigit(*t) ) ++t;   /* more digits */

			/* check for exponent */
			if   (*t=='e' || *t=='E' )
			{
				++t;
				if ( *t == '\0' ) return xFALSE;  /* '5e' isn't a number! */
				if ( *t != '\0' && IsSign(*t) )
				{  
					if ( *t == '-' ) status |= OPT_NUM_FLOAT;  /* '1e-3' = 0.001 is FLOAT */
					++t;
				}
				while( *t !=  '\0' && IsDigit(*t) ) ++t;
			}

			/* trailing space is okay */
			while( *t != '\0' && IsSpace(*t) ) ++t; 

			/* if made it to the end then its a real */
			return (*t=='\0') ? status : xFALSE;

		}

		/* return numeric value of string: basically a wrapper on atof() */

		f64 Opt_Num::opt_atof(char *s)
		{
			s32 valid;
			f64 x;
			x = x_atof64(s);	/* Call atof() whether or not string is valid,
								* because some strings, eg '15x' can still be
								* interpreted as numeric.  But give a warning
								* unless the string really is valid
								*/
			valid = opt_isvalidnumber(s);
			if (!valid) 
			{
				opt_warn_2("String [%s] is not a valid number, will use [%g]",s,x);
			}
			return x;
		}
		s64 Opt_Num::opt_atoi(char *s)
		{
			s32 valid;
			s64 x;
			x = (s64)x_atoi64(s);	/* Call atof() whether or not string is valid,
									* because some strings, eg '15x' can still be
									* interpreted as numeric.  But give a warning
									* unless the string really is valid
									*/

			valid = opt_isvalidnumber(s);
			if (!valid || (valid & OPT_NUM_FLOAT))
			{
				opt_warn_2("String [%s] is not a valid integer, will use [%ld]",s,x);
			}
			return x;
		}

		u64 Opt_Num::opt_atou(char *s)
		{
			s32 valid;
			u64 x;
			x = (u64)x_atoi64(s);
			valid = opt_isvalidnumber(s);
			if (!valid || (valid & OPT_NUM_FLOAT) || (valid & OPT_NUM_NEG))
			{
				opt_warn_2("String [%s] is not a valid positive integer, will use [%lu]",s,x);
			}
			return x;
		}
	}
}