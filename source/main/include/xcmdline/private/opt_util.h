#ifndef __XCMDLINE_OPT_UTIL_H__
#define __XCMDLINE_OPT_UTIL_H__
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
		class Opt_Util
		{
		public:
			static void		opt_message(char *);
			static void		opt_warning(char *);
			static void		opt_fatal(char *);

			static void		opt_setstring(char **, char *);
			static char		*opt_justify(char* s, s32 width, s32 indent, s32 initial, char* prefix);
		};

		//< shouldn't be fixed length, but it is!
		#define OPT_ERRMAXSTRLEN 1024 

		#define opt_snprintf_1(buf,n,fmt,v1)           x_sprintf(buf,n-1,fmt,v1)
		#define opt_snprintf_2(buf,n,fmt,v1,v2)        x_sprintf(buf,n-1,fmt,v1,v2)
		#define opt_snprintf_3(buf,n,fmt,v1,v2,v3)     x_sprintf(buf,n-1,fmt,v1,v2,v3)
		#define opt_snprintf_4(buf,n,fmt,v1,v2,v3,v4)  x_sprintf(buf,n-1,fmt,v1,v2,v3,v4)

		#define opt_warn_1(fmt,var1) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_1(gstr,OPT_ERRMAXSTRLEN,fmt,var1); \
			Opt_Util::opt_warning(gstr); } while(0)

		#define opt_warn_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			Opt_Util::opt_warning(gstr); } while(0)

		#define opt_warn_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			Opt_Util::opt_warning(gstr); } while(0)

		#define opt_fatal_1(fmt,var1) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_1(gstr,OPT_ERRMAXSTRLEN,fmt,var1); \
			Opt_Util::opt_fatal(gstr); } while(0)

		#define opt_fatal_2(fmt,var1,var2) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_2(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2); \
			Opt_Util::opt_fatal(gstr); } while(0)

		#define opt_fatal_3(fmt,var1,var2,var3) do { \
			char gstr[OPT_ERRMAXSTRLEN]; \
			opt_snprintf_3(gstr,OPT_ERRMAXSTRLEN,fmt,var1,var2,var3); \
			Opt_Util::opt_fatal(gstr); } while(0)


	}
};


#endif	///< __XCMDLINE_OPT_UTIL_H__
