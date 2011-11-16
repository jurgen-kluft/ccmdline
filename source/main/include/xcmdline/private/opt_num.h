#ifndef __XCMDLINE_OPT_NUM_H__
#define __XCMDLINE_OPT_NUM_H__
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase\x_types.h"


namespace xcore
{
	namespace xcmdline
	{
		
		#define OPT_NUM_VALID   0x1
		#define OPT_NUM_FLOAT   0x2
		#define OPT_NUM_NEG     0x4

		class Opt_Num
		{
		public:
			static f64		opt_atof(char *);
			static s64		opt_atoi(char *);
			static u64		opt_atou(char *);
			static s32		opt_isvalidnumber(char *);
		};

		
	}
};


#endif	///< __XCMDLINE_OPT_NUM_H__
