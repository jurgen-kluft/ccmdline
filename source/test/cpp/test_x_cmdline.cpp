#include "xbase\x_target.h"
#include "xbase\x_string_ascii.h"
#include "xbase\x_string_utf.h"

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

		UNITTEST_TEST(test_str_to_bool)
		{
			CHECK_TRUE(StrToBool("yes"));
			CHECK_TRUE(StrToBool("yEs"));
			CHECK_FALSE(StrToBool("no"));
			CHECK_FALSE(StrToBool("nO"));

			CHECK_TRUE(StrToBool("true"));
			CHECK_TRUE(StrToBool("TRUE"));
			CHECK_FALSE(StrToBool("false"));
			CHECK_FALSE(StrToBool("FALSE"));

			CHECK_TRUE(StrToBool("on"));
			CHECK_TRUE(StrToBool("ON"));
			CHECK_FALSE(StrToBool("off"));
			CHECK_FALSE(StrToBool("OFF"));
		}

		UNITTEST_TEST(test_parse_int)
		{
			s32 prop_month = 0;
			s32 prop_day = 0;
			s32 prop_year = 0;
			xcore::cli::argV argv[] = {
				xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_month)),
				xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_day)),
				xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_year)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--month 12 --day 30 -y 2011"));
			
			CHECK_EQUAL(12, prop_month);
			CHECK_EQUAL(30, prop_day);
			CHECK_EQUAL(2011, prop_year);
 		}

		UNITTEST_TEST(test_parse_float)
		{
			f32 prop_test = 0.0f;
			xcore::cli::argV argv[] = {
				xcore::cli::argV("t", "test", "Test", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_test)),
				xcore::cli::argV::nil
			};
			
			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--test 12.352 -t 3.14"));
			CHECK_EQUAL(3.14f, prop_test);
		}

		UNITTEST_TEST(test_parse_char)
		{
			uchar32 prop_char = 'x';
			xcore::cli::argV argv[] = {
				xcore::cli::argV("c", "charVar", "Character variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_char)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-c 'a' --charVar 'e'"));
			CHECK_EQUAL('e', prop_char);
		}

		UNITTEST_TEST(test_parse_string)
		{
			char prop_str[64];
			xcore::cli::argV argv[] = {
				xcore::cli::argV("s", "stringVar", "Character variable", xcore::cli::eOPT_REQUIRED, x_va_r(prop_str)),
				xcore::cli::argV::nil
			};

			const char* parse_str = "-s \"String\" --stringVar \"Today is 9.30\"";
			cmdline	c;
			CHECK_TRUE(c.parse(argv, parse_str));
		}

		UNITTEST_TEST(test_parse_bool)
		{
			bool prop_bool_a = true;
			bool prop_bool_b = false;
			bool prop_bool_c = true;

			xcore::cli::argV argv[] = {
				xcore::cli::argV("a", "boolVarA", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_bool_a)),
				xcore::cli::argV("b", "boolVarB", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_bool_b)),
				xcore::cli::argV("c", "boolVarC", "A boolean variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_bool_c)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-a false --boolVarB true -c false"));
			CHECK_EQUAL(false, prop_bool_a);
			CHECK_EQUAL(true, prop_bool_b);
			CHECK_EQUAL(false, prop_bool_c);
		}

		UNITTEST_TEST(test_parse_other)
		{
			s32 prop_t = 0;
			xstring prop_str(gHeapAllocator, 64);
			xcore::cli::argV argv[] = {
				xcore::cli::argV("t", "testHelp", "A integer variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_t)),
				xcore::cli::argV("v", "version", "A string variable", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_str)),
				xcore::cli::argV::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--testHelp 325 --version \"v1.0\""));
		}

		UNITTEST_TEST(test_another_parse)
		{
			s32 prop_year = 0;
			s32 prop_month = 0;
			s32 prop_day = 0;
			char prop_who[128];
			char prop_what[128];
			bool prop_birthday = false;

			xcore::cli::argV argv[] = {
				xcore::cli::argV("y", "year", "Year", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_year)),
				xcore::cli::argV("m", "month", "Month", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_month)),
				xcore::cli::argV("d", "day", "Day", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_day)),
				xcore::cli::argV("who", "who", "Who", xcore::cli::eOPT_REQUIRED, x_va_r(prop_who)),
				xcore::cli::argV("w", "what", "What", xcore::cli::eOPT_OPTIONAL, x_va_r(prop_what)),
				xcore::cli::argV("b", "isBirthday", "Is it a birthday", xcore::cli::eOPT_OPTIONAL, x_va_r(&prop_birthday)),
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
			s32 prop_count = 0;
			bool prop_force = false;

			xcore::cli::argV argv[] = {
				xcore::cli::argV("c", "count", "Number of items", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_count)),
				xcore::cli::argV("f", "force", "Force illegal items", xcore::cli::eOPT_REQUIRED, x_va_r(&prop_force)),
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
