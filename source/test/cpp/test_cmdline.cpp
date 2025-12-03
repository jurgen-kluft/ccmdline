#include "ccore/c_target.h"
#include "ccore/c_memory.h"
#include "cbase/c_runes.h"
#include "ccmdline/c_cmdline.h"
#include "cunittest/cunittest.h"

using namespace ncore;

extern ncore::alloc_t* gHeapAllocator;

UNITTEST_SUITE_BEGIN(test_cmdline)
{
    UNITTEST_FIXTURE(test_parse_reg)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(test_parse_int)
        {
            s32              prop_month = 0;
            s32              prop_day   = 0;
            s32              prop_year  = 0;
            ncore::cli::argv argv[]     = {ncore::cli::argv("m", "month", "Month", ncore::cli::Required, va_r_t(&prop_month)), ncore::cli::argv("d", "day", "Day", ncore::cli::Required, va_r_t(&prop_day)),
                                           ncore::cli::argv("y", "year", "Year", ncore::cli::Required, va_r_t(&prop_year)), ncore::cli::argv::nil};

            CHECK_TRUE(parse(argv, "--month 12 --day 30 -y 2011"));

            CHECK_EQUAL(12, prop_month);
            CHECK_EQUAL(30, prop_day);
            CHECK_EQUAL(2011, prop_year);
        }

        UNITTEST_TEST(test_parse_float)
        {
            f32              prop_test = 0.0f;
            ncore::cli::argv argv[]    = {ncore::cli::argv("t", "test", "Test", ncore::cli::Required, va_r_t(&prop_test)), ncore::cli::argv::nil};

            CHECK_TRUE(parse(argv, "--test 12.352 -t 3.14"));
            CHECK_EQUAL(3.14f, prop_test);
        }

        UNITTEST_TEST(test_parse_char)
        {
            ascii::rune      prop_chars[8];
            ncore::cli::argv argv[] = {ncore::cli::argv("c", "charVar", "Character variable", ncore::cli::Required, va_r_t(prop_chars, 8)), ncore::cli::argv::nil};

            CHECK_TRUE(parse(argv, "-c \"eee\"") == true);
            CHECK_EQUAL('e', prop_chars[0]);
        }

        UNITTEST_TEST(test_parse_string)
        {
            ascii::rune prop_str[64];

            ncore::cli::argv argv[] = {ncore::cli::argv("s", "stringVar", "Character variable", ncore::cli::Required, va_r_t(prop_str, 64)), ncore::cli::argv::nil};

            const char* parse_str = "-s \"String\" --stringVar \"Today is 9.30\"";
            CHECK_TRUE(parse(argv, parse_str));
            CHECK_EQUAL(prop_str, "Today is 9.30");
        }

        UNITTEST_TEST(test_parse_bool)
        {
            bool prop_bool_a = true;
            bool prop_bool_b = false;
            bool prop_bool_c = true;

            ncore::cli::argv argv[] = {ncore::cli::argv("a", "boolVarA", "A boolean variable", ncore::cli::Required, va_r_t(&prop_bool_a)), ncore::cli::argv("b", "boolVarB", "A boolean variable", ncore::cli::Required, va_r_t(&prop_bool_b)),
                                       ncore::cli::argv("c", "boolVarC", "A boolean variable", ncore::cli::Required, va_r_t(&prop_bool_c)), ncore::cli::argv::nil};

            CHECK_TRUE(parse(argv, "-a false --boolVarB true -c false"));
            CHECK_EQUAL(false, prop_bool_a);
            CHECK_EQUAL(true, prop_bool_b);
            CHECK_EQUAL(false, prop_bool_c);
        }

        UNITTEST_TEST(test_parse_other)
        {
            s32         prop_int;
            ascii::rune prop_str[64];
            //g_memset(prop_str, 0, sizeof(prop_str));

            ncore::cli::argv argv[] = {ncore::cli::argv("t", "testHelp", "A integer variable", ncore::cli::Required, va_r_t(&prop_int)), ncore::cli::argv("v", "version", "A string variable", ncore::cli::Required, va_r_t(prop_str, 64)),
                                       ncore::cli::argv::nil};

            CHECK_TRUE(parse(argv, "--testHelp 325 --version \"v1.0\""));
            CHECK_EQUAL(prop_str, "v1.0");
        }

        UNITTEST_TEST(test_another_parse)
        {
            s32         prop_year  = 0;
            s32         prop_month = 0;
            s32         prop_day   = 0;
            ascii::rune prop_who[128];
            ascii::rune prop_what[128];
            bool        prop_birthday = true;

            ncore::cli::argv argv[] = {ncore::cli::argv("y", "year", "Year", ncore::cli::Required, va_r_t(&prop_year)),
                                       ncore::cli::argv("m", "month", "Month", ncore::cli::Required, va_r_t(&prop_month)),
                                       ncore::cli::argv("d", "day", "Day", ncore::cli::Required, va_r_t(&prop_day)),
                                       ncore::cli::argv("who", "who", "Who", ncore::cli::Required, va_r_t(prop_who, 128)),
                                       ncore::cli::argv("w", "what", "What", ncore::cli::Optional, va_r_t(prop_what, 128)),
                                       ncore::cli::argv("b", "isBirthday", "Is it a birthday", ncore::cli::Optional, va_r_t(&prop_birthday)),
                                       ncore::cli::argv::nil};

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
            const char* cargv = "-y 2011 --month 12 --day 30.22 -who \"Jurgen\" -w 'J' --isBirthday false";

            CHECK_TRUE(parse(argv, cargv));
            CHECK_EQUAL(prop_who, ("Jurgen"));
            CHECK_EQUAL(prop_what, ("J"));
            CHECK_FALSE(prop_birthday);
        }

        UNITTEST_TEST(test_argL_argV_1)
        {
            s32  prop_count = 0;
            bool prop_force = false;

            ncore::cli::argv argv[] = {ncore::cli::argv("c", "count", "Number of items", ncore::cli::Required, va_r_t(&prop_count)), ncore::cli::argv("f", "force", "Force illegal items", ncore::cli::Required, va_r_t(&prop_force)), ncore::cli::argv::nil};

            ncore::cli::argl argl[] = {ncore::cli::argl((const char*)"add", argv), ncore::cli::argl((const char*)"remove", argv), ncore::cli::argl((const char*)"replace", argv), ncore::cli::argl::nil};

            ncore::cli::cmds cmds = {argl};

            parse(cmds, "remove --count 3 --force --items \"item { name : Book }\" \"item { name : Car }\" \"item { name : Pen }\"");

            CHECK_NOT_EQUAL(-1, cmds.m_index);
            CHECK_EQUAL(1, cmds.m_index);
        }
    }
}
UNITTEST_SUITE_END
