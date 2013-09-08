#include "xbase\x_types.h"
#include "xbase\x_string.h"
#include "xbase\x_string_std.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_proc.h"

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
			
		}

		UNITTEST_TEST(test_parse_float)
		{
			x_cmdline	c;
			c.parse("-t 3.14 --test 12.352");
			
		}

		UNITTEST_TEST(test_parse_char)
		{
			x_cmdline	c;
			c.parse("-c 'a' --charVar 'e'");
			
		}

		UNITTEST_TEST(test_parse_string)
		{
			x_cmdline	c;
			c.parse("-s \"String\" --stringVar \"Today is 9.30\"");
			
		}

  		UNITTEST_TEST(test_parse_bool)
  		{
  			x_cmdline	c;
  			c.parse("-a false --boolVar true -c false");
  			
  		}
  
		UNITTEST_TEST(test_parse_other)
		{
			s32	   testHelp	=	0;
			x_cmdline	c;
			
			c.parse("--testHelp 325 --version ?b");

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

		}
	}
}
UNITTEST_SUITE_END
