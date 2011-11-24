#include "xbase\x_types.h"
#include "xbase\x_allocator.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_proc.h"

#include "xunittest\xunittest.h"
using namespace xcore;
using namespace xcmdline;
extern xcore::x_iallocator* gSystemAllocator;

UNITTEST_SUITE_BEGIN(xcmdline_array_register)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(optrega_array)
		{
			s32 uArrayNum[6] = {0,0,0,0,0,0};
			u8*   arrayU8			=	NULL;						
			u16*	arrayU16		=	NULL;
			u32*	arrayU32		=	NULL;
			u64*	arrayU64		=	NULL;

			s32 sArrayNum[6] = {0,0,0,0,0,0};
			s8*			arrayS8			=	NULL;
			s16*		arrayS16		=	NULL;
			s32*		arrayS32		=	NULL;
			s64*		arrayS64		=	NULL;

			s32 fArrayNum[2] = {0,0};
			f32*		arrayf32		=	NULL;
			f64*		arrayf64		=	NULL;

			s32 charArrayNum = 0;
			xtchar* arrayXtchar	=	NULL;

			xcmdline::Opt_Proc opt_proc;

			opt_proc.optrega_array(	&uArrayNum[0],	&arrayU8,	OPT_UBYTE,	'c',	"ubyte",		"unsigned byte");
			opt_proc.optrega_array(	&uArrayNum[1],	&arrayU16,	OPT_USHORT,	's',	"ushort",		"unsigned short");
			opt_proc.optrega_array(	&uArrayNum[2],	&arrayU32,	OPT_UINT,	'i',	"uint",			"unsigned int");
			opt_proc.optrega_array(	&uArrayNum[3],	&arrayU64,	OPT_ULONG,	'l',	"ulong",		"unsigned long");
			opt_proc.optrega_array(	&sArrayNum[0],	&arrayS8,	OPT_BYTE,	'u',	"byte",			"signed byte");
			opt_proc.optrega_array(	&sArrayNum[1],	&arrayS16,	OPT_SHORT,	'm',	"sshort",		"signed short");
			opt_proc.optrega_array(	&sArrayNum[2],	&arrayS32,	OPT_INT,	'x',	"sint",			"signed int");
 			opt_proc.optrega_array(	&sArrayNum[3],	&arrayS64,	OPT_LONG,	'b',	"slong",		"signed long");
			opt_proc.optrega_array(	&fArrayNum[0],	&arrayf32,	OPT_FLOAT,	'f',	"float",		"float 32");
			opt_proc.optrega_array(	&fArrayNum[1],	&arrayf64,	OPT_DOUBLE,	'd',	"double",		"double 64");
			opt_proc.optrega_array(	&charArrayNum,	&arrayXtchar,OPT_CHAR,	'q',	"char",			"xtchar");
			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-c",
				"12,34",
				"-s",
				"56,23,64",
				"-i",
				"5000,10000",
				"--ulong",
				"4000000000,1,2",
				"-u",
				"34,56",
				"--sshort",
				"-5,-3,-4",
				"-x",
				"-5000,234",
				"-b",
				"4000000000,-400",
				"-f",
				"43.5,23.6",
				"-d",
				"0.003,12.234",
				"-q",
				"e,t,q"
			};

			xcore::s32 argc = sizeof(argv)/sizeof(char*);
			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			opt_proc.opt(&argc, argvv);
			

			CHECK_EQUAL(2,uArrayNum[0]);
			CHECK_NOT_NULL(arrayU8);
			CHECK_EQUAL(12,arrayU8[0]);
			CHECK_EQUAL(34,arrayU8[1]);

			CHECK_EQUAL(3,uArrayNum[1]);
			CHECK_NOT_NULL(arrayU16);
			CHECK_EQUAL(56,arrayU16[0]);
			CHECK_EQUAL(23,arrayU16[1]);
			CHECK_EQUAL(64,arrayU16[2]);

			CHECK_EQUAL(2,uArrayNum[2]);
			CHECK_NOT_NULL(arrayU32);
			CHECK_EQUAL(5000,arrayU32[0]);
			CHECK_EQUAL(10000,arrayU32[1]);

			CHECK_EQUAL(3,uArrayNum[3]);
			CHECK_NOT_NULL(arrayU64);
			CHECK_EQUAL(4000000000,arrayU64[0]);
			CHECK_EQUAL(1,arrayU64[1]);
			CHECK_EQUAL(2,arrayU64[2]);

			CHECK_EQUAL(2,sArrayNum[0]);
			CHECK_NOT_NULL(arrayS8);
			CHECK_EQUAL(34,arrayS8[0]);
			CHECK_EQUAL(56,arrayS8[1]);

			CHECK_EQUAL(3,sArrayNum[1]);
			CHECK_NOT_NULL(arrayS16);
			CHECK_EQUAL(-5,arrayS16[0]);
			CHECK_EQUAL(-3,arrayS16[1]);
			CHECK_EQUAL(-4,arrayS16[2]);

			CHECK_EQUAL(2,sArrayNum[2]);
			CHECK_NOT_NULL(arrayS32);
			CHECK_EQUAL(-5000,arrayS32[0]);
			CHECK_EQUAL(234,arrayS32[1]);

			CHECK_EQUAL(2,sArrayNum[3]);
			CHECK_NOT_NULL(arrayS64);
			CHECK_EQUAL(4000000000,arrayS64[0]);
			CHECK_EQUAL((s64)-400,arrayS64[1]);

			CHECK_EQUAL(2,fArrayNum[0]);
			CHECK_NOT_NULL(arrayf32);
			CHECK_EQUAL((f32)43.5,arrayf32[0]);
			CHECK_EQUAL((f32)23.6,arrayf32[1]);

			CHECK_EQUAL(2,fArrayNum[1]);
			CHECK_NOT_NULL(arrayf64);
			CHECK_EQUAL((f64)0.003,arrayf64[0]);
			CHECK_EQUAL((f64)12.234,arrayf64[1]);

			CHECK_EQUAL(3,charArrayNum);
			CHECK_NOT_NULL(arrayXtchar);
			CHECK_EQUAL('e',arrayXtchar[0]);
			CHECK_EQUAL('t',arrayXtchar[1]);
			CHECK_EQUAL('q',arrayXtchar[2]);

			opt_proc.opt_free();
/*
*		Other functions like optregc_array just call the optrega_array and modify one parameter to NULL
*		So we needn't to test these functions if we have tested the optrega_array
*/
		}
	}
}
UNITTEST_SUITE_END
