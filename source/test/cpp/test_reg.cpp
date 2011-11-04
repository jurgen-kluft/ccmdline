#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"

#include "xunittest\xunittest.h"


using namespace xcore;


UNITTEST_SUITE_BEGIN(xcmdline_tests_reg)
{
	UNITTEST_FIXTURE(reg_all_types)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}




		//test all optreg for s32.
		UNITTEST_TEST(regs32)
		{
			s32	   regaTest_s32	   =	0;
			s32    regTest_s32	   =	0;
			s32    regcTest_s32	   =	0;
			s32    regcbTest_s32   =	0;	
			s32    regsTest_s32    =	0;
			s32    regsbTest_s32   =	0;
			s32    regfTest_s32	   =	0;
			s32    regpTest_s32	   =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_s32,xcmdline::OPT_INT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_s32,xcmdline::OPT_INT,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_s32,xcmdline::OPT_INT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_s32,xcmdline::OPT_INT,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_s32,xcmdline::OPT_INT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_s32,xcmdline::OPT_INT,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_s32,xcmdline::OPT_INT,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_s32,xcmdline::OPT_INT,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			//If we want to compare two values, we should convert them to the
			//same type, or it may cause some dangerous bugs
			//So we should explicitly convert them.
			//So does below.
			CHECK_EQUAL((s32)10,regaTest_s32);
			CHECK_EQUAL((s32)8,regTest_s32);
			CHECK_EQUAL((s32)-29,regcTest_s32);
			CHECK_EQUAL((s32)325,regcbTest_s32);
			CHECK_EQUAL((s32)-3549,regsTest_s32);
			CHECK_EQUAL((s32)-23,regsbTest_s32);
			CHECK_EQUAL((s32)55,regfTest_s32);
			CHECK_EQUAL((s32)67,regpTest_s32);

		}


		//test all optreg for s64.
		UNITTEST_TEST(regs64)
		{
			s64	   regaTest_s64	   =	0;
			s64    regTest_s64	   =	0;
			s64    regcTest_s64	   =	0;
			s64    regcbTest_s64   =	0;	
			s64    regsTest_s64	   =	0;
			s64    regsbTest_s64   =	0;
			s64    regfTest_s64    =	0;
			s64    regpTest_s64    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_s64,xcmdline::OPT_LONG,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_s64,xcmdline::OPT_LONG,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_s64,xcmdline::OPT_LONG,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_s64,xcmdline::OPT_LONG,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_s64,xcmdline::OPT_LONG,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_s64,xcmdline::OPT_LONG,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_s64,xcmdline::OPT_LONG,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_s64,xcmdline::OPT_LONG,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d9999999",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=63"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((s64)10,regaTest_s64);
			CHECK_EQUAL((s64)8,regTest_s64);
			CHECK_EQUAL((s64)-29,regcTest_s64);
			CHECK_EQUAL((s64)9999999,regcbTest_s64);
			CHECK_EQUAL((s64)-3549,regsTest_s64);
			CHECK_EQUAL((s64)-23,regsbTest_s64);
			CHECK_EQUAL((s64)55,regfTest_s64);
			CHECK_EQUAL((s64)63,regpTest_s64);

		}


		//test all optreg for s16.
		UNITTEST_TEST(regs16)
		{
			s16	   regaTest_s16	   =	0;
			s16    regTest_s16     =	0;
			s16    regcTest_s16    =	0;
			s16    regcbTest_s16   =	0;	
			s16    regsTest_s16    =	0;
			s16    regsbTest_s16   =	0;
			s16    regfTest_s16    =	0;
			s16    regpTest_s16    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_s16,xcmdline::OPT_SHORT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_s16,xcmdline::OPT_SHORT,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_s16,xcmdline::OPT_SHORT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_s16,xcmdline::OPT_SHORT,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_s16,xcmdline::OPT_SHORT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_s16,xcmdline::OPT_SHORT,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_s16,xcmdline::OPT_SHORT,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_s16,xcmdline::OPT_SHORT,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d456",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=63"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((s16)10,regaTest_s16);
			CHECK_EQUAL((s16)8,regTest_s16);
			CHECK_EQUAL((s16)-29,regcTest_s16);
			CHECK_EQUAL((s16)456,regcbTest_s16);
			CHECK_EQUAL((s16)-3549,regsTest_s16);
			CHECK_EQUAL((s16)-23,regsbTest_s16);
			CHECK_EQUAL((s16)55,regfTest_s16);
			CHECK_EQUAL((s16)63,regpTest_s16);

		}


		//test all optreg for s8.
		UNITTEST_TEST(regs8)
		{
			s8    regaTest_s8    =	0;
			s8    regTest_s8     =	0;
			s8    regcTest_s8    =	0;
			s8    regcbTest_s8   =	0;	
			s8    regsTest_s8    =	0;
			s8    regsbTest_s8   =	0;
			s8    regfTest_s8    =	0;
			s8    regpTest_s8    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_s8,xcmdline::OPT_BYTE,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_s8,xcmdline::OPT_BYTE,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_s8,xcmdline::OPT_BYTE,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_s8,xcmdline::OPT_BYTE,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_s8,xcmdline::OPT_BYTE,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_s8,xcmdline::OPT_BYTE,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_s8,xcmdline::OPT_BYTE,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_s8,xcmdline::OPT_BYTE,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((s8)10,regaTest_s8);
			CHECK_EQUAL((s8)8,regTest_s8);
			CHECK_EQUAL((s8)-29,regcTest_s8);
			CHECK_EQUAL((s8)325,regcbTest_s8);
			CHECK_EQUAL((s8)-3549,regsTest_s8);
			CHECK_EQUAL((s8)-23,regsbTest_s8);
			CHECK_EQUAL((s8)55,regfTest_s8);
			CHECK_EQUAL((s8)67,regpTest_s8);

		}


		//test all optreg for u16.
		UNITTEST_TEST(regu16)
		{
			u16	   regaTest_u16   =	   0;
			u16    regTest_u16    =    0;
			u16    regcTest_u16   =    0;
			u16    regcbTest_u16  =	   0;	
			u16    regsTest_u16   =    0;
			u16    regsbTest_u16  =	   0;
			u16    regfTest_u16   =    0;
			u16    regpTest_u16   =    0;

			xcmdline::Opt_Reg::optrega(&regaTest_u16,xcmdline::OPT_USHORT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_u16,xcmdline::OPT_USHORT,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_u16,xcmdline::OPT_USHORT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_u16,xcmdline::OPT_USHORT,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_u16,xcmdline::OPT_USHORT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_u16,xcmdline::OPT_USHORT,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_u16,xcmdline::OPT_USHORT,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_u16,xcmdline::OPT_USHORT,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c29",
				"-d456",
				"--regs=3549",
				"--regsb",
				"23",
				"-e",
				"-55",
				"--regp=63"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((u16)10,regaTest_u16);
			CHECK_EQUAL((u16)8,regTest_u16);
			CHECK_EQUAL((u16)29,regcTest_u16);
			CHECK_EQUAL((u16)456,regcbTest_u16);
			CHECK_EQUAL((u16)3549,regsTest_u16);
			CHECK_EQUAL((u16)23,regsbTest_u16);
			CHECK_EQUAL((u16)-55,regfTest_u16);
			CHECK_EQUAL((u16)63,regpTest_u16);

		}


		//test all optreg for u32.
		UNITTEST_TEST(regu32)
		{
			u32    regaTest_u32    =	0;
			u32    regTest_u32     =	0;
			u32    regcTest_u32    =	0;
			u32    regcbTest_u32   =	0;	
			u32    regsTest_u32    =	0;
			u32    regsbTest_u32   =	0;
			u32    regfTest_u32    =	0;
			u32    regpTest_u32    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_u32,xcmdline::OPT_UINT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_u32,xcmdline::OPT_UINT,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_u32,xcmdline::OPT_UINT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_u32,xcmdline::OPT_UINT,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_u32,xcmdline::OPT_UINT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_u32,xcmdline::OPT_UINT,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_u32,xcmdline::OPT_UINT,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_u32,xcmdline::OPT_UINT,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-999929",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((u32)10,regaTest_u32);
			CHECK_EQUAL((u32)8,regTest_u32);
			CHECK_EQUAL((u32)-999929,regcTest_u32);
			CHECK_EQUAL((u32)325,regcbTest_u32);
			CHECK_EQUAL((u32)-3549,regsTest_u32);
			CHECK_EQUAL((u32)-23,regsbTest_u32);
			CHECK_EQUAL((u32)55,regfTest_u32);
			CHECK_EQUAL((u32)67,regpTest_u32);

		}


		//test all optreg for u64.
		UNITTEST_TEST(regu64)
		{
			u64    regaTest_u64    =	0;
			u64    regTest_u64     =	0;
			u64    regcTest_u64    =	0;
			u64    regcbTest_u64   =	0;	
			u64    regsTest_u64    =	0;
			u64    regsbTest_u64   =	0;
			u64    regfTest_u64    =	0;
			u64    regpTest_u64    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_u64,xcmdline::OPT_ULONG,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_u64,xcmdline::OPT_ULONG,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_u64,xcmdline::OPT_ULONG,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_u64,xcmdline::OPT_ULONG,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_u64,xcmdline::OPT_ULONG,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_u64,xcmdline::OPT_ULONG,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_u64,xcmdline::OPT_ULONG,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_u64,xcmdline::OPT_ULONG,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d9999999",
				"--regs=-35499999",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=63"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((u64)10,regaTest_u64);
			CHECK_EQUAL((u64)8,regTest_u64);
			CHECK_EQUAL((u64)-29,regcTest_u64);
			CHECK_EQUAL((u64)9999999,regcbTest_u64);
			CHECK_EQUAL((u64)-35499999,regsTest_u64);
			CHECK_EQUAL((u64)-23,regsbTest_u64);
			CHECK_EQUAL((u64)55,regfTest_u64);
			CHECK_EQUAL((u64)63,regpTest_u64);

		}

		//test all optreg for u8.
		UNITTEST_TEST(regu8)
		{
			u8    regaTest_u8    =	  0;
			u8    regTest_u8     =	  0;
			u8    regcTest_u8    =	  0;
			u8    regcbTest_u8   =	  0;	
			u8    regsTest_u8    =	  0;
			u8    regsbTest_u8   =	  0;
			u8    regfTest_u8    =    0;
			u8    regpTest_u8    =    0;

			xcmdline::Opt_Reg::optrega(&regaTest_u8,xcmdline::OPT_UBYTE,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_u8,xcmdline::OPT_UBYTE,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_u8,xcmdline::OPT_UBYTE,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_u8,xcmdline::OPT_UBYTE,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_u8,xcmdline::OPT_UBYTE,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_u8,xcmdline::OPT_UBYTE,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_u8,xcmdline::OPT_UBYTE,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_u8,xcmdline::OPT_UBYTE,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10",
				"-b8",
				"-c-29",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((u8)10,regaTest_u8);
			CHECK_EQUAL((u8)8,regTest_u8);
			CHECK_EQUAL((u8)-29,regcTest_u8);
			CHECK_EQUAL((u8)325,regcbTest_u8);
			CHECK_EQUAL((u8)-3549,regsTest_u8);
			CHECK_EQUAL((u8)-23,regsbTest_u8);
			CHECK_EQUAL((u8)55,regfTest_u8);
			CHECK_EQUAL((u8)67,regpTest_u8);

		}

		//test all optreg for f32.
		UNITTEST_TEST(regf32)
		{
			f32    regaTest_f32    =	0.0f;
			f32    regTest_f32     =	0.0f;
			f32    regcTest_f32    =	0.0f;
			f32    regcbTest_f32   =	0.0f;
			f32    regsTest_f32    =	0.0f;
			f32    regsbTest_f32   =	0.0f;
			f32    regfTest_f32    =	0.0f;
			f32    regpTest_f32    =	0.0f;

			xcmdline::Opt_Reg::optrega(&regaTest_f32,xcmdline::OPT_FLOAT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_f32,xcmdline::OPT_FLOAT,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_f32,xcmdline::OPT_FLOAT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_f32,xcmdline::OPT_FLOAT,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_f32,xcmdline::OPT_FLOAT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_f32,xcmdline::OPT_FLOAT,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_f32,xcmdline::OPT_FLOAT,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_f32,xcmdline::OPT_FLOAT,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10.6",
				"-b8.57",
				"-c-999929.43",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55.8999",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((f32)10.6f,regaTest_f32);
			CHECK_EQUAL((f32)8.57f,regTest_f32);
			CHECK_EQUAL((f32)-999929.43f,regcTest_f32);
			CHECK_EQUAL((f32)325.0f,regcbTest_f32);
			CHECK_EQUAL((f32)-3549.0f,regsTest_f32);
			CHECK_EQUAL((f32)-23.0f,regsbTest_f32);
			CHECK_EQUAL((f32)55.8999f,regfTest_f32);
			CHECK_EQUAL((f32)67.0f,regpTest_f32);

		}

		//test all optreg for f64.
		UNITTEST_TEST(regf64)
		{
			f64    regaTest_f64    =	0.0;
			f64    regTest_f64     =	0.0;
			f64    regcTest_f64    =	0.0;
			f64    regcbTest_f64   =	0.0;
			f64    regsTest_f64    =	0.0;
			f64    regsbTest_f64   =	0.0;
			f64    regfTest_f64    =	0.0;
			f64    regpTest_f64    =	0.0;

			xcmdline::Opt_Reg::optrega(&regaTest_f64,xcmdline::OPT_DOUBLE,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_f64,xcmdline::OPT_DOUBLE,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_f64,xcmdline::OPT_DOUBLE,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_f64,xcmdline::OPT_DOUBLE,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_f64,xcmdline::OPT_DOUBLE,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_f64,xcmdline::OPT_DOUBLE,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_f64,xcmdline::OPT_DOUBLE,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_f64,xcmdline::OPT_DOUBLE,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a10.6",
				"-b8.57",
				"-c-999929.43",
				"-d325",
				"--regs=-3549",
				"--regsb",
				"-23",
				"-e",
				"55.8999",
				"--regp=67"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL((f64)10.6,regaTest_f64);
			CHECK_EQUAL((f64)8.57,regTest_f64);
			CHECK_EQUAL((f64)-999929.43,regcTest_f64);
			CHECK_EQUAL((f64)325,regcbTest_f64);
			CHECK_EQUAL((f64)-3549,regsTest_f64);
			CHECK_EQUAL((f64)-23,regsbTest_f64);
			CHECK_EQUAL((f64)55.8999,regfTest_f64);
			CHECK_EQUAL((f64)67,regpTest_f64);

		}

		//test all optreg for xbool.
		//use xbool instead of bool, PS3 may not support bool.
		UNITTEST_TEST(regxbool)
		{
			xbool    regaTest_xbool	   =	0;
			xbool    regTest_xbool     =	0;
			xbool    regcTest_xbool    =	0;
			xbool    regcbTest_xbool   =	0;
			xbool    regsTest_xbool    =	0;
			xbool    regsbTest_xbool   =	0;
			xbool    regfTest_xbool    =	0;
			xbool    regpTest_xbool    =	0;

			xcmdline::Opt_Reg::optrega(&regaTest_xbool,xcmdline::OPT_BOOL,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_xbool,xcmdline::OPT_BOOL,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_xbool,xcmdline::OPT_BOOL,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_xbool,xcmdline::OPT_BOOL,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_xbool,xcmdline::OPT_BOOL,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_xbool,xcmdline::OPT_BOOL,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_xbool,xcmdline::OPT_BOOL,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_xbool,xcmdline::OPT_BOOL,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a1",
				"-b0",
				"-c",
				"-d",
				"--regs=1",
				"--regsb=0",
				"-e",
				"--regp=0"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_TRUE(regaTest_xbool);
			CHECK_FALSE(regTest_xbool);
			CHECK_TRUE(regcTest_xbool);
			CHECK_TRUE(regcbTest_xbool);
			CHECK_TRUE(regsTest_xbool);
			CHECK_FALSE(regsbTest_xbool);
			CHECK_TRUE(regfTest_xbool);
			CHECK_FALSE(regpTest_xbool);

		}

		//test all optreg for xtchar.
		UNITTEST_TEST(regxtchar)
		{
			xtchar    regaTest_xtchar    =	'a';
			xtchar    regTest_xtchar     =	'a';
			xtchar    regcTest_xtchar    =  'a';
			xtchar    regcbTest_xtchar   =	'a';	
			xtchar    regsTest_xtchar    =  'a';
			xtchar    regsbTest_xtchar   =  'a';
			xtchar    regfTest_xtchar    =  'a';
			xtchar    regpTest_xtchar    =  'a';

			xcmdline::Opt_Reg::optrega(&regaTest_xtchar,xcmdline::OPT_UCHAR,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_xtchar,xcmdline::OPT_UCHAR,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_xtchar,xcmdline::OPT_UCHAR,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_xtchar,xcmdline::OPT_UCHAR,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_xtchar,xcmdline::OPT_UCHAR,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_xtchar,xcmdline::OPT_UCHAR,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_xtchar,xcmdline::OPT_UCHAR,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_xtchar,xcmdline::OPT_UCHAR,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-ab",
				"-be",
				"-cu",
				"-dh",
				"--regs=t",
				"--regsb=o",
				"-el",
				"--regp=y"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL('b',regaTest_xtchar);
			CHECK_EQUAL('e',regTest_xtchar);
			CHECK_EQUAL('u',regcTest_xtchar);
			CHECK_EQUAL('h',regcbTest_xtchar);
			CHECK_EQUAL('t',regsTest_xtchar);
			CHECK_EQUAL('o',regsbTest_xtchar);
			CHECK_EQUAL('l',regfTest_xtchar);
			CHECK_EQUAL('y',regpTest_xtchar);

		}

		//test all optreg for char*.
		UNITTEST_TEST(regchar)
		{
			char*    regaTest_char   =	  NULL;
			char*    regTest_char    =	  NULL;
			char*    regcTest_char   =	  NULL;
			char*    regcbTest_char  =    NULL;	
			char*    regsTest_char   =    NULL;
			char*    regsbTest_char  =    NULL;
			char*    regfTest_char   =    NULL;
			char*    regpTest_char   =	  NULL;

			xcmdline::Opt_Reg::optrega(&regaTest_char,xcmdline::OPT_STRING,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest_char,xcmdline::OPT_STRING,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest_char,xcmdline::OPT_STRING,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest_char,xcmdline::OPT_STRING,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest_char,xcmdline::OPT_STRING,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest_char,xcmdline::OPT_STRING,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest_char,xcmdline::OPT_STRING,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&regpTest_char,xcmdline::OPT_STRING,"regp","optregp");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a",
				"I'm a",
				"-b",
				"I'm b",
				"--regs=I'm regs",
				"--regsb=I'm regsb",
				"-e",
				"I'm e"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_NOT_NULL(regaTest_char);
			CHECK_NOT_NULL(regTest_char);
			CHECK_NULL(regcTest_char);
			CHECK_NULL(regcbTest_char);
			CHECK_NOT_NULL(regsTest_char);
			CHECK_NOT_NULL(regsbTest_char);
			CHECK_NOT_NULL(regfTest_char);
			CHECK_NULL(regpTest_char);

			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regaTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regcTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regcbTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regsTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regsbTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regfTest_char);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regpTest_char);

		}
	}
}
UNITTEST_SUITE_END
