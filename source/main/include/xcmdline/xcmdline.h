#ifndef __XCORE_CMDLINE_H__
#define __XCORE_CMDLINE_H__
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

//==============================================================================
// INCLUDES
//==============================================================================
#include "xbase/x_target.h"
#include "xbase/x_va_list.h"

//==============================================================================
// xcore namespace
//==============================================================================
namespace xcore
{
	// Forward declares
	class xalloc;

	namespace cli
	{
		struct params;

		enum eoption
		{
			eOPT_OPTIONAL,
			eOPT_REQUIRED
		};

		struct argV
		{
			inline				argV(const char* sn, const char* ln, const char* de, eoption o, x_va_r v)
				: mShort(sn)
				, mLong(ln)
				, mDescription(de)
				, mOption(o)
				, mValue(v) 
			{
			}

			const char*			mShort;						// e.g. f
			const char*			mLong;						// e.g. force
			const char*			mDescription;				// "Force to add files"
			eoption				mOption;					// OPTIONAL
			x_va_r				mValue;						// false -> boolean
			static argV			nil;
		};

		struct argL
		{
			inline				argL(const char* name, argV* argv) 
				: mName(name)
				, mArgV(argv) 
			{
			}

			const char*			mName;						// "add"
			argV*				mArgV;

			static argL			nil;
		};

		struct cmds
		{
			inline				cmds() : mArgL(NULL) {}
			inline				cmds(argL* argl) : mArgL(argl) {}

			argL*				mArgL;
		};

		class instance
		{
		public:
			xbool				parse(argV * arg, const char* cmdline);
			xbool				parse(argV * arg, s32 argc, const char** argv);

			xbool				parse(cmds & c, const char* cmdline);
			xbool				parse(cmds & c, s32 argc, const char** argv);

		private:
			xbool				parse(cmds & c);
		};

	};

	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__
