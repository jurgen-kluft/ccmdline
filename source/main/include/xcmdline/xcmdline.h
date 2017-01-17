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
#include "xbase\x_va_list.h"

//==============================================================================
// xcore namespace
//==============================================================================
namespace xcore
{
	// Forward declares
	class x_iallocator;

	namespace xcli
	{
		struct params;

		enum eoption
		{
			eOPT_OPTIONAL,
			eOPT_REQUIRED
		};

		struct argV
		{
			const char*			mShort;						// e.g. f
			const char*			mLong;						// e.g. force
			const char*			mDescription;				// "Force to add files"
			eoption				mOption;					// OPTIONAL
			x_va_r				mValue;						// false -> boolean
		};

		struct argL
		{
			const char*			mName;						// "add"
			s32					mArgC;
			argV*				mArgV;
		};

		struct cmds
		{
			s32					mArgC;
			argL*				mArgL;
		};

		class parser
		{
		public:
								parser(x_iallocator* allocator);
								~parser();

			/*parse the cmdline and store them in xparams*/
			xbool				parse(const char* cmdline);
			xbool				parse(s32 argc, const char** argv);

			xbool				compile(argL& args) const;
			xbool				compile(cmds& cmds) const;

		private:
			xbool				parse();

			x_iallocator*		mAllocator;
			params*				mParameter;
			char*				mCmdline;
		};

	};

	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__
