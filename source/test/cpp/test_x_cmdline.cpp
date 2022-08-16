#include "xbase/x_target.h"
#include "xbase/x_runes.h"
#include "xcmdline/xcmdline.h"
#include "xunittest/xunittest.h"

using namespace ncore;

typedef ncore::cli::cmdline_t cmdline;
extern ncore::alloc_t* gHeapAllocator;

UNITTEST_SUITE_BEGIN(test_x_cmdline)
{
	UNITTEST_FIXTURE(test_parse_reg)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(test_parse_int)
		{
			s32 prop_month = 0;
			s32 prop_day = 0;
			s32 prop_year = 0;
			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("m", "month", "Month", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_month)),
				ncore::cli::argv_t("d", "day", "Day", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_day)),
				ncore::cli::argv_t("y", "year", "Year", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_year)),
				ncore::cli::argv_t::nil
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
			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("t", "test", "Test", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_test)),
				ncore::cli::argv_t::nil
			};
			
			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--test 12.352 -t 3.14"));
			CHECK_EQUAL(3.14f, prop_test);
		}

		UNITTEST_TEST(test_parse_char)
		{
			runez_t<ascii::rune, 8> prop_chars;
			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("c", "charVar", "Character variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_chars)),
				ncore::cli::argv_t::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-c \"eee\"")==true);
			CHECK_EQUAL('e', prop_chars.m_ascii.m_str[0]);
		}

		UNITTEST_TEST(test_parse_string)
		{
			runez_t<ascii::rune, 64> prop_str;

			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("s", "stringVar", "Character variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_str)),
				ncore::cli::argv_t::nil
			};

			const char* parse_str = "-s \"String\" --stringVar \"Today is 9.30\"";
			cmdline	c;
			CHECK_TRUE(c.parse(argv, parse_str));
			CHECK_TRUE(prop_str == crunes_t("Today is 9.30"));
		}

		UNITTEST_TEST(test_parse_bool)
		{
			bool prop_bool_a = true;
			bool prop_bool_b = false;
			bool prop_bool_c = true;

			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("a", "boolVarA", "A boolean variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_bool_a)),
				ncore::cli::argv_t("b", "boolVarB", "A boolean variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_bool_b)),
				ncore::cli::argv_t("c", "boolVarC", "A boolean variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_bool_c)),
				ncore::cli::argv_t::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "-a false --boolVarB true -c false"));
			CHECK_EQUAL(false, prop_bool_a);
			CHECK_EQUAL(true, prop_bool_b);
			CHECK_EQUAL(false, prop_bool_c);
		}

		UNITTEST_TEST(test_parse_other)
		{
			s32 prop_int;
			runez_t<ascii::rune, 64> prop_str;

			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("t", "testHelp", "A integer variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_int)),
				ncore::cli::argv_t("v", "version", "A string variable", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_str)),
				ncore::cli::argv_t::nil
			};

			cmdline	c;
			CHECK_TRUE(c.parse(argv, "--testHelp 325 --version \"v1.0\""));
			CHECK_TRUE(prop_str == crunes_t("v1.0"));
		}

		UNITTEST_TEST(test_another_parse)
		{
			s32 prop_year = 0;
			s32 prop_month = 0;
			s32 prop_day = 0;
			runez_t<ascii::rune, 128> prop_who;
			runez_t<ascii::rune, 128> prop_what;
			bool prop_birthday = false;

			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("y", "year", "Year", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_year)),
				ncore::cli::argv_t("m", "month", "Month", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_month)),
				ncore::cli::argv_t("d", "day", "Day", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_day)),
				ncore::cli::argv_t("who", "who", "Who", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_who)),
				ncore::cli::argv_t("w", "what", "What", ncore::cli::eOPT_OPTIONAL, va_r_t(&prop_what)),
				ncore::cli::argv_t("b", "isBirthday", "Is it a birthday", ncore::cli::eOPT_OPTIONAL, va_r_t(&prop_birthday)),
				ncore::cli::argv_t::nil
			};

//			const char*	cli_argv[] =
//			{
//				"-y",
//				"2011",
//				"--month",
//				"12",
//				"--day",
//				"30.22",
//				"-who",
//				"\"Jurgen\"",
//				"-w",
//				"'J'",
//				"--isBirthday",
//				"false"
//			};
			const char*	cargv = "-y 2011 --month 12 --day 30.22 -who \"Jurgen\" -w 'J' --isBirthday false";

			cmdline	c;
			CHECK_TRUE(c.parse(argv, cargv));
			CHECK_TRUE(prop_who == crunes_t("Jurgen"));
			CHECK_TRUE(prop_what == crunes_t("J"));
		}

		UNITTEST_TEST(test_argL_argV_1)
		{
			s32 prop_count = 0;
			bool prop_force = false;

			ncore::cli::argv_t argv[] = {
				ncore::cli::argv_t("c", "count", "Number of items", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_count)),
				ncore::cli::argv_t("f", "force", "Force illegal items", ncore::cli::eOPT_REQUIRED, va_r_t(&prop_force)),
				ncore::cli::argv_t::nil
			};

			ncore::cli::argl_t argl[] = {
				ncore::cli::argl_t((const char*)"add", argv),
				ncore::cli::argl_t((const char*)"remove", argv),
				ncore::cli::argl_t((const char*)"replace", argv),
				ncore::cli::argl_t::nil
			};

			ncore::cli::cmds_t cmds = {
				argl
			};

			cmdline	c;
 			c.parse(cmds, "remove --count 3 --force --items \"item { name : Book }\" \"item { name : Car }\" \"item { name : Pen }\"");

			CHECK_NOT_EQUAL(-1, cmds.m_index);
			CHECK_EQUAL(1, cmds.m_index);
		}
	}
}
UNITTEST_SUITE_END
