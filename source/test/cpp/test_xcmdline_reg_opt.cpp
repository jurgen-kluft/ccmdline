#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"

#include "xunittest\xunittest.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(test_xcmdline_reg_opt)
{
	UNITTEST_FIXTURE(test_reg_opt)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(test_reg_opt_s32)
		{
			s32	regTest1	=	0;
			s32	regTest2	=	0;
			s32	regTest3	=	0;
			s32	regTest4	=	0;
			s32	regTest5	=	0;
			s32	regTest6	=	0;
			s32	regTest7	=	0;
			s32	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_INT,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_INT,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_INT,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_INT,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"21",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((s32)329,regTest1);
			CHECK_EQUAL((s32)-234,regTest2);
			CHECK_EQUAL((s32)66,regTest3);
			CHECK_EQUAL((s32)21,regTest4);
			CHECK_EQUAL((s32)12,regTest5);
			CHECK_EQUAL((s32)34,regTest6);
			CHECK_EQUAL((s32)56,regTest7);
			CHECK_EQUAL((s32)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_s16)
		{
			s16	regTest1	=	0;
			s16	regTest2	=	0;
			s16	regTest3	=	0;
			s16	regTest4	=	0;
			s16	regTest5	=	0;
			s16	regTest6	=	0;
			s16	regTest7	=	0;
			s16	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_SHORT,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_SHORT,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_SHORT,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_SHORT,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"21",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((s16)329,regTest1);
			CHECK_EQUAL((s16)-234,regTest2);
			CHECK_EQUAL((s16)66,regTest3);
			CHECK_EQUAL((s16)21,regTest4);
			CHECK_EQUAL((s16)12,regTest5);
			CHECK_EQUAL((s16)34,regTest6);
			CHECK_EQUAL((s16)56,regTest7);
			CHECK_EQUAL((s16)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_s64)
		{
			s64	regTest1	=	0;
			s64	regTest2	=	0;
			s64	regTest3	=	0;
			s64	regTest4	=	0;
			s64	regTest5	=	0;
			s64	regTest6	=	0;
			s64	regTest7	=	0;
			s64	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_LONG,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_LONG,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_LONG,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_LONG,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"21",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((s64)329,regTest1);
			CHECK_EQUAL((s64)-234,regTest2);
			CHECK_EQUAL((s64)66,regTest3);
			CHECK_EQUAL((s64)21,regTest4);
			CHECK_EQUAL((s64)12,regTest5);
			CHECK_EQUAL((s64)34,regTest6);
			CHECK_EQUAL((s64)56,regTest7);
			CHECK_EQUAL((s64)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_u16)
		{
			u16	regTest1	=	0;
			u16	regTest2	=	0;
			u16	regTest3	=	0;
			u16	regTest4	=	0;
			u16	regTest5	=	0;
			u16	regTest6	=	0;
			u16	regTest7	=	0;
			u16	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_USHORT,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_USHORT,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_USHORT,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_USHORT,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"21",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((u16)329,regTest1);
			CHECK_EQUAL((u16)-234,regTest2);
			CHECK_EQUAL((u16)66,regTest3);
			CHECK_EQUAL((u16)21,regTest4);
			CHECK_EQUAL((u16)12,regTest5);
			CHECK_EQUAL((u16)34,regTest6);
			CHECK_EQUAL((u16)56,regTest7);
			CHECK_EQUAL((u16)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_u32)
		{
			u32	regTest1	=	0;
			u32	regTest2	=	0;
			u32	regTest3	=	0;
			u32	regTest4	=	0;
			u32	regTest5	=	0;
			u32	regTest6	=	0;
			u32	regTest7	=	0;
			u32	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_UINT,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_UINT,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_UINT,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_UINT,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"21",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((u32)329,regTest1);
			CHECK_EQUAL((u32)-234,regTest2);
			CHECK_EQUAL((u32)66,regTest3);
			CHECK_EQUAL((u32)21,regTest4);
			CHECK_EQUAL((u32)12,regTest5);
			CHECK_EQUAL((u32)34,regTest6);
			CHECK_EQUAL((u32)56,regTest7);
			CHECK_EQUAL((u32)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_u64)
		{
			u64	regTest1	=	0;
			u64	regTest2	=	0;
			u64	regTest3	=	0;
			u64	regTest4	=	0;
			u64	regTest5	=	0;
			u64	regTest6	=	0;
			u64	regTest7	=	0;
			u64	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_ULONG,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_ULONG,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_ULONG,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_ULONG,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"219999999999",
				"12",
				"34",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((u64)329,regTest1);
			CHECK_EQUAL((u64)-234,regTest2);
			CHECK_EQUAL((u64)66,regTest3);
			CHECK_EQUAL((u64)219999999999,regTest4);
			CHECK_EQUAL((u64)12,regTest5);
			CHECK_EQUAL((u64)34,regTest6);
			CHECK_EQUAL((u64)56,regTest7);
			CHECK_EQUAL((u64)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_f32)
		{
			f32	regTest1	=	0.0f;
			f32	regTest2	=	0.0f;
			f32	regTest3	=	0.0f;
			f32	regTest4	=	0.0f;
			f32	regTest5	=	0.0f;
			f32	regTest6	=	0.0f;
			f32	regTest7	=	0.0f;
			f32	regTest8	=	0.0f;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_FLOAT,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_FLOAT,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_FLOAT,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_FLOAT,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329.456",
				"--regTest2=-234.5655",
				"-c66",
				"--regTest4",
				"21.0",
				"12.67",
				"34.33",
				"56.87987",
				"78.444"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((f32)329.456f,regTest1);
			CHECK_EQUAL((f32)-234.5655f,regTest2);
			CHECK_EQUAL((f32)66.0f,regTest3);
			CHECK_EQUAL((f32)21.0f,regTest4);
			CHECK_EQUAL((f32)12.67f,regTest5);
			CHECK_EQUAL((f32)34.33f,regTest6);
			CHECK_EQUAL((f32)56.87987f,regTest7);
			CHECK_EQUAL((f32)78.444f,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_f64)
		{
			f64	regTest1	=	0;
			f64	regTest2	=	0;
			f64	regTest3	=	0;
			f64	regTest4	=	0;
			f64	regTest5	=	0;
			f64	regTest6	=	0;
			f64	regTest7	=	0;
			f64	regTest8	=	0;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_DOUBLE,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_DOUBLE,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_DOUBLE,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_DOUBLE,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a329.4523",
				"--regTest2=-234",
				"-c66",
				"--regTest4",
				"219999999999.565656",
				"12",
				"34.888",
				"56",
				"78"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL((f64)329.4523,regTest1);
			CHECK_EQUAL((f64)-234,regTest2);
			CHECK_EQUAL((f64)66,regTest3);
			CHECK_EQUAL((f64)219999999999.565656,regTest4);
			CHECK_EQUAL((f64)12,regTest5);
			CHECK_EQUAL((f64)34.888,regTest6);
			CHECK_EQUAL((f64)56,regTest7);
			CHECK_EQUAL((f64)78,regTest8);
		}

		UNITTEST_TEST(test_reg_opt_char)
		{
			char	regTest1	=	'a';
			char	regTest2	=	'a';
			char	regTest3	=	'a';
			char	regTest4	=	'a';
			char	regTest5	=	'a';
			char	regTest6	=	'a';
			char	regTest7	=	'a';
			char	regTest8	=	'a';

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_CHAR,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_CHAR,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_CHAR,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_CHAR,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-ab",
				"--regTest2=d",
				"-ct",
				"x",
				"y",
				"z",
				"m"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL('b',regTest1);
			CHECK_EQUAL('d',regTest2);
			CHECK_EQUAL('t',regTest3);
			CHECK_EQUAL('a',regTest4);
			CHECK_EQUAL('x',regTest5);
			CHECK_EQUAL('y',regTest6);
			CHECK_EQUAL('z',regTest7);
			CHECK_EQUAL('m',regTest8);
		}

		UNITTEST_TEST(test_reg_opt_uchar)
		{
			xtchar	regTest1	=	'a';
			xtchar	regTest2	=	'a';
			xtchar	regTest3	=	'a';
			xtchar	regTest4	=	'a';
			xtchar	regTest5	=	'a';
			xtchar	regTest6	=	'a';
			xtchar	regTest7	=	'a';
			xtchar	regTest8	=	'a';

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_UCHAR,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_UCHAR,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_UCHAR,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_UCHAR,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-ab",
				"--regTest2=d",
				"-ct",
				"x",
				"y",
				"z",
				"m"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_EQUAL('b',regTest1);
			CHECK_EQUAL('d',regTest2);
			CHECK_EQUAL('t',regTest3);
			CHECK_EQUAL('a',regTest4);
			CHECK_EQUAL('x',regTest5);
			CHECK_EQUAL('y',regTest6);
			CHECK_EQUAL('z',regTest7);
			CHECK_EQUAL('m',regTest8);
		}

		UNITTEST_TEST(test_reg_opt_string)
		{
			char*	regTest1	=	NULL;
			char*	regTest2	=	NULL;
			char*	regTest3	=	NULL;
			char*	regTest4	=	NULL;
			char*	regTest5	=	NULL;
			char*	regTest6	=	NULL;
			char*	regTest7	=	NULL;
			char*	regTest8	=	NULL;

			x_cmdline c;
			c.reg_opt(&regTest1,'a');
			c.reg_opt(&regTest2,"regTest2");
			c.reg_opt(&regTest3,x_cmdline::OPT_STRING,'c');
			c.reg_opt(&regTest4,x_cmdline::OPT_STRING,"regTest4");
			c.reg_opt(&regTest5,x_cmdline::OPT_POSITIONAL,'e');
			c.reg_opt(&regTest6,x_cmdline::OPT_POSITIONAL,"regTest6");
			c.reg_opt(&regTest7,x_cmdline::OPT_STRING,x_cmdline::OPT_POSITIONAL,'g');
			c.reg_opt(&regTest8,x_cmdline::OPT_STRING,x_cmdline::OPT_POSITIONAL,"regTest8");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-a",
				"I'm a",
				"--regTest2=I'm regTest2",
				"--regTest4",
				"I'm regTest4",
				"RegTest5",
				"RegTest6",
				"RegTest7"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

			CHECK_NOT_NULL(regTest1);
			CHECK_NOT_NULL(regTest2);
			CHECK_NULL(regTest3);
			CHECK_NOT_NULL(regTest4);
			CHECK_NOT_NULL(regTest5);
			CHECK_NOT_NULL(regTest6);
			CHECK_NOT_NULL(regTest7);
			CHECK_NULL(regTest8);

			xcmdline::get_opt_allocator()->deallocate(regTest1);
			xcmdline::get_opt_allocator()->deallocate(regTest2);
			xcmdline::get_opt_allocator()->deallocate(regTest3);
			xcmdline::get_opt_allocator()->deallocate(regTest4);
			xcmdline::get_opt_allocator()->deallocate(regTest5);
			xcmdline::get_opt_allocator()->deallocate(regTest6);
			xcmdline::get_opt_allocator()->deallocate(regTest7);
			xcmdline::get_opt_allocator()->deallocate(regTest8);
		}

	}
}
UNITTEST_SUITE_END
