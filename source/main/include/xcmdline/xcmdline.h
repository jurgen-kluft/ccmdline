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

	class x_cmdline
	{
	public:
		bool			parse(const char* cmdline);
		bool			parse(s32 argc, const char** argv);

		enum EOptType
		{
			OPT_NUL,
			OPT_INT,        OPT_SHORT,      OPT_LONG,       OPT_CHAR,
			OPT_UINT,       OPT_USHORT,     OPT_ULONG,      OPT_UCHAR,
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
	};


	//==============================================================================
	// END xcore namespace
	//==============================================================================
};


#endif    /// __XCORE_CMDLINE_H__