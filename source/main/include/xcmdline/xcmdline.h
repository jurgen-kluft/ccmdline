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

	namespace xcmdline
	{
		struct Opt_Proc;
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

		/*free memory*/
		void			clearRegisteredOption();

		enum EOptType
		{
			OPT_NUL,
			OPT_INT,        OPT_SHORT,      OPT_LONG,       OPT_CHAR,
			OPT_UINT,       OPT_USHORT,     OPT_ULONG,      OPT_UCHAR,

			// Add this two types to deal with the s8 and u8
			OPT_BYTE,       OPT_UBYTE,

			OPT_FLOAT,      OPT_DOUBLE,
			OPT_TOGGLE,     OPT_BOOL,
			OPT_NEGTOGGLE,  OPT_NEGBOOL,    OPT_INTLEVEL,       
			OPT_STRING,     
		};

		enum EOptMode
		{
			OPT_POSITIONAL,
			OPT_DELIMITED,
			OPT_FLEXIBLE
		};

		s32				reg_opt(char *v, char c, char *n=0, char *B=0);
		s32				reg_opt(char *v, char *n, char *B=0);
		s32				reg_opt(char *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(char *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(char *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(char *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(char *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(char *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s16 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(s16 *v, char *n, char *B=0);
		s32				reg_opt(s16 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(s16 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(s16 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s16 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s16 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s16 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u16 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(u16 *v, char *n, char *B=0);
		s32				reg_opt(u16 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(u16 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(u16 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u16 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u16 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u16 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s32 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(s32 *v, char *n, char *B=0);
		s32				reg_opt(s32 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(s32 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(s32 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s32 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s32 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s32 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u32 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(u32 *v, char *n, char *B=0);
		s32				reg_opt(u32 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(u32 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(u32 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u32 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u32 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u32 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(f64 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(f64 *v, char *n, char *B=0);
		s32				reg_opt(f64 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(f64 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(f64 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(f64 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(f64 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(f64 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s64 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(s64 *v, char *n, char *B=0);
		s32				reg_opt(s64 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(s64 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(s64 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s64 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(s64 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(s64 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u64 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(u64 *v, char *n, char *B=0);
		s32				reg_opt(u64 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(u64 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(u64 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u64 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(u64 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(u64 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(char **v, char c, char *n=0, char *B=0);
		s32				reg_opt(char **v, char *n, char *B=0);
		s32				reg_opt(char **v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(char **v, EOptType o, char *n, char *B=0);
		s32				reg_opt(char **v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(char **v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(char **v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(char **v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(f32 *v, char c, char *n=0, char *B=0);
		s32				reg_opt(f32 *v, char *n, char *B=0);
		s32				reg_opt(f32 *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(f32 *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(f32 *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(f32 *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(f32 *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(f32 *v, EOptType o, EOptMode m, char *n, char *B=0);
		s32				reg_opt(unsigned char *v, char c, char *n=0, char *B=0);
		s32				reg_opt(unsigned char *v, char *n, char *B=0);
		s32				reg_opt(unsigned char *v, EOptType o, char c, char *n=0, char *B=0);
		s32				reg_opt(unsigned char *v, EOptType o, char *n, char *B=0);
		s32				reg_opt(unsigned char *v, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(unsigned char *v, EOptMode m, char *n, char *B=0);
		s32				reg_opt(unsigned char *v, EOptType o, EOptMode m, char c, char *n=0, char *B=0);
		s32				reg_opt(unsigned char *v, EOptType o, EOptMode m, char *n, char *B=0);

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

		xcmdline::Opt_Proc*	mOptProc;

		/*clear the argv allocated by changeParamToArgv()*/
		void			argvClear();

		/*Change the param to argv,argc in order to call opt()*/
		void			changeParamToArgv();

		/*Remember what argv is automatically registered*/
		void			setRegList(s32 regNum);

		s32				findVariable(char*	variableName);

		/*change "true" "false" into 1,0*/
		xbool			JudgeTrueOrFalse(char* _string);



		/*Since we use opt() to process the argv, we don't need to write 
		  the setvalue. So this two functions are not used now.*/
		void			setValueForOpt(xcmdline::xparams*	parameters);
		void			setValue(s32	optNum,	char* optValue);

	};


	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__