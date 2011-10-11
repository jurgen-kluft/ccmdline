#include "xbase\x_types.h"
#include "xbase\x_string.h"
#include "xbase\x_string_std.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"

#include "xunittest\xunittest.h"

#include <stdio.h>

using namespace xcore;


UNITTEST_SUITE_BEGIN(test_x_cmdline)
{
	UNITTEST_FIXTURE(test_parse_reg)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(test_parse_int)
		{
			x_cmdline	c;
			c.parse("--month 12 --day 30 -y 2011");
			c.regAndSetValue();
 			c.parse("--month 9");
 			c.regAndSetValue();
 
			void* y	=	c.getValue("y");
			void* m	=	c.getValue("month");
			void* d	=	c.getValue("day"); 

			s32 year	=	OPT_GETVALUE(s32,y);
			s32	month	=	OPT_GETVALUE(s32,m);
			s32	day	=	OPT_GETVALUE(s32,d);
 
 			c.clearRegisteredOption();
 
 			xcmdline::opt_free();

			CHECK_EQUAL((s32)2011, year);
			CHECK_EQUAL((s32)9, month);
			CHECK_EQUAL((s32)30, day);
		}

		UNITTEST_TEST(test_parse_float)
		{
			x_cmdline	c;
			c.parse("-t 3.14 --test 12.352");
			c.regAndSetValue();

			f32	test	=	OPT_GETVALUE(f32,c.getValue("t"));
			f32	test2	=	OPT_GETVALUE(f32,c.getValue("test"));

			c.clearRegisteredOption();

			xcmdline::opt_free();

			CHECK_EQUAL((f32)3.14f, test);
			CHECK_EQUAL((f32)12.352f, test2);
		}

		UNITTEST_TEST(test_parse_char)
		{
			x_cmdline	c;
			c.parse("-c 'a' --charVar 'e'");
			c.regAndSetValue();

			char	test	=	OPT_GETVALUE(char,c.getValue("c"));
			char	test2	=	OPT_GETVALUE(char,c.getValue("charVar"));

			c.clearRegisteredOption();

			xcmdline::opt_free();

			CHECK_EQUAL('a', test);
			CHECK_EQUAL('e', test2);
		}

		UNITTEST_TEST(test_parse_string)
		{
			x_cmdline	c;
			c.parse("-s \"String\" --stringVar \"Today is 9.30\"");
			c.regAndSetValue();
 			c.parse("-s \"Jurgen\" --stringVar \"Today is 10.30\"");
 			c.regAndSetValue();

			char*	test	=	OPT_GETVALUE(char*,c.getValue("s"));
			char*	test2	=	OPT_GETVALUE(char*,c.getValue("stringVar"));

			xbool result1	=	x_strcmp(test, "Jurgen");
			xbool result2	=	x_strcmp(test2, "Today is 10.30");

			c.clearRegisteredOption();

			xcmdline::opt_free();


			CHECK_FALSE(result1);
			CHECK_FALSE(result2);
		}

  		UNITTEST_TEST(test_parse_bool)
  		{
  			x_cmdline	c;
  			c.parse("-a false --boolVar true -c false");
  			c.regAndSetValue();
  
  			xbool	test	=	OPT_GETVALUE(xbool,c.getValue("a"));
  			xbool	test2	=	OPT_GETVALUE(xbool,c.getValue("boolVar"));
  			xbool	test3	=	OPT_GETVALUE(xbool,c.getValue("c"));
  
  			c.clearRegisteredOption();
  
  			xcmdline::opt_free();
  
  			CHECK_FALSE(test);
  			CHECK_TRUE(test2);
  			CHECK_FALSE(test3);
  		}
  
		UNITTEST_TEST(test_parse_other)
		{
			s32	   testHelp	=	0;

			xcmdline::optrega(&testHelp,xcmdline::OPT_INT,'b',"testHelp","testHelp");
			xcmdline::opthelp(&testHelp,"This is the testHelp help");
			xcmdline::optVersion("1.0");

			x_cmdline	c;
			c.parse("--testHelp 325 --version ?b");
			c.regAndSetValue();

			void* y	=	c.getValue("testHelp");

			c.clearRegisteredOption();

			xcmdline::opt_free();

			CHECK_EQUAL((s32)325, testHelp);
		}

		UNITTEST_TEST(test_another_parse)
		{
			const char*	argv[] =
			{
				"-y",
				"2011",
				"--month",
				"12",
				"--day",
				"30.22",
				"-who",
				"\"Jurgen\"",
				"-w",
				"'J'",
				"--isBirthday",
				"false"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			const char** argvp = argv;

			x_cmdline	c;
			c.parse(argc,argvp);
			c.regAndSetValue();

			s32	year	=	OPT_GETVALUE(s32,c.getValue("y"));
			s32	month	=	OPT_GETVALUE(s32,c.getValue("month"));
			f32	day	=	OPT_GETVALUE(f32,c.getValue("day"));
			char* who	=	OPT_GETVALUE(char*,c.getValue("who"));
			char	whoShort	=	OPT_GETVALUE(char,c.getValue("w"));
			xbool	isBirthday	=	OPT_GETVALUE(xbool,c.getValue("isBirthday"));

			CHECK_EQUAL((s32)2011, year);
			CHECK_EQUAL((s32)12, month);
			CHECK_EQUAL((f32)30.22f, day);
			CHECK_NOT_NULL(who);
			CHECK_EQUAL('J',whoShort);
			CHECK_FALSE(isBirthday);

			c.clearRegisteredOption();

			xcmdline::opt_free();
		}
	}
}
UNITTEST_SUITE_END
