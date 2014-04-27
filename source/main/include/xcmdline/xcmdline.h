#ifndef __XCORE_CMDLINE_H__
#define __XCORE_CMDLINE_H__
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

//==============================================================================
// INCLUDES
//==============================================================================
#include "xbase\x_types.h"

//==============================================================================
// xcore namespace
//==============================================================================
namespace xcore
{
	// Forward declares
	class x_iallocator;

	namespace xcmdline
	{
		struct xargv;
		struct xparams;
	}

	class x_cmdline
	{
	public:
							x_cmdline(x_iallocator* allocator);
							~x_cmdline();

		/*parse the cmdline and store them in xparams*/
		xbool				parse(const char* cmdline);
		xbool				parse(s32 argc, const char** argv);

		/*interface to get variable's value by giving its name*/
		void*				getValue(char const* name);

	private:
		xbool				parse();

		x_iallocator*		mAllocator;
		xcmdline::xparams*	mParameter;
		char*				mCmdline;
	};


	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__