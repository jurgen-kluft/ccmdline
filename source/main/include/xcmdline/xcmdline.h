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
	struct xargv;
	class x_iallocator;

	namespace xcmdline
	{
		struct xparams;
	}

	class x_cmdline
	{
	public:
						x_cmdline();
						~x_cmdline();

		/*parse the cmdline and store them in xparams*/
		xbool			parse(const char* cmdline);
		xbool			parse(s32 argc, const char** argv);

		/*interface to get variable's value by give its name*/
		void*			getValue(char* name);

		/*Register variables and set value by xparams*/
		xbool			regAndSetValue();

		
	private:
		xargv*			mArgVector;
		char**			mArgv;
		s32				mArgc;
		xcmdline::xparams*	mParameter;
		s32*			mRegList;
		s32				mRegListNum;
		char*			mNewCmdline;
		char**			mStringList;
		int				mStringListNum;
		int				mTotalAllocArgvNum;
		const int		mAllocArgvBlockSize;

		x_iallocator*	mAllocator;

		/*change "true" "false" into 1,0*/
		xbool			JudgeTrueOrFalse(char* _string);

	};


	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__