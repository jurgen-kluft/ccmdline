#include "xbase\x_target.h"
#include "xbase\x_string_ascii.h"

#include "xcmdline\xcmdline.h"

#include "xunittest\xunittest.h"

using namespace xcore;

typedef xcore::cli::instance cmdline;
extern xcore::x_iallocator* gHeapAllocator;

UNITTEST_SUITE_BEGIN(test_x_cmdline)
{
	UNITTEST_FIXTURE(test_parse_reg)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(test_parse_int)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va((s32)1)),
				xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va((s32)1)),
				xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va((s32)1)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--month 12 --day 30 -y 2011"));
		}

		UNITTEST_TEST(test_parse_float)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("t", "test", "Test", xcore::cli::eOPT_REQUIRED, x_va((f32)1.0f)),
				xcore::cli::argV::nil
			};
			
			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-t 3.14 --test 12.352"));
		}

		UNITTEST_TEST(test_parse_char)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("c", "charVar", "Character variable", xcore::cli::eOPT_REQUIRED, x_va((char)'x')),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-c 'a' --charVar 'e'"));
		}

		UNITTEST_TEST(test_parse_string)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("s", "stringVar", "Character variable", xcore::cli::eOPT_REQUIRED, x_va("")),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-s \"String\" --stringVar \"Today is 9.30\""));
		}

		UNITTEST_TEST(test_parse_bool)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("a", "boolVarA", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va(true)),
				xcore::cli::argV("b", "boolVarB", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va(true)),
				xcore::cli::argV("c", "boolVarC", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va(true)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-a false --boolVar true -c false"));
		}

		UNITTEST_TEST(test_parse_other)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("t", "testHelp", "A integer variable", xcore::cli::eOPT_REQUIRED, x_va((s32)0)),
				xcore::cli::argV("v", "version", "A string variable", xcore::cli::eOPT_REQUIRED, x_va("")),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--testHelp 325 --version \"v1.0\""));
		}

		UNITTEST_TEST(test_another_parse)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va((s32)0)),
				xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va((s32)0)),
				xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va((s32)0)),
				xcore::cli::argV("who", "who", "Who", xcore::cli::eOPT_REQUIRED, x_va("")),
				xcore::cli::argV("w", "what", "What", xcore::cli::eOPT_OPTIONAL, x_va('?')),
				xcore::cli::argV("b", "isBirthday", "Is it a birthday", xcore::cli::eOPT_OPTIONAL, x_va(false)),
				xcore::cli::argV::nil
			};

			const char*	cli_argv[] =
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
			xcore::s32 argc = sizeof(cli_argv) / sizeof(char*);

			const char** argvp = cli_argv;

			cmdline	c;
			CHECK_TRUE(c.parse(argv, argc, argvp));
		}

		UNITTEST_TEST(test_argL_argV_1)
		{
			xcore::cli::argV argv[] = {
				xcore::cli::argV("c", "count", "Number of items", xcore::cli::eOPT_REQUIRED, x_va((s32)1)),
				xcore::cli::argV("f", "force", "Force illegal items", xcore::cli::eOPT_REQUIRED, x_va(true)),
				xcore::cli::argV::nil
			};

			xcore::cli::argL argl[] = {
				xcore::cli::argL((const char*)"add", argv),
				xcore::cli::argL((const char*)"remove", argv),
				xcore::cli::argL((const char*)"replace", argv),
				xcore::cli::argL::nil
			};

			xcore::cli::cmds cmds = {
				argl
			};

			cmdline	c;
			c.parse(cmds, "add --count 3 --force --items \"item { name : Book }\" \"item { name : Car }\" \"item { name : Pen }\"");
		}
	}
}
UNITTEST_SUITE_END
