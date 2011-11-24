#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_proc.h"

#include "xunittest\xunittest.h"
#include "xbase\x_string_std.h"

using namespace xcore;
using namespace xcmdline;

UNITTEST_SUITE_BEGIN(xcmdline_ag_tests)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		xcmdline::xargv* test_ag = NULL;

		char* argv[] = 
		{
			"--longname",
			"32",
			"-p",
			"90"
		};
		xcore::s32 argc = sizeof(argv)/sizeof(char*);
		char** argvp = argv;



		UNITTEST_TEST(ag_new_free)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			CHECK_NOT_NULL(test_ag);

			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_enstring)		//this function has not been implemented yet
		{
		}

		UNITTEST_TEST(ag_reset)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->iw = 3;
			test_ag->ic = 2;
			
			Ag_Func::ag_reset(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL(0,test_ag->iw);
			CHECK_EQUAL(0,test_ag->ic);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_w_number)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->iw = 3;

			xcore::s32 tempW = Ag_Func::ag_w_number(test_ag);

			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL(3,tempW);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_w_advance)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			Ag_Func::ag_w_advance(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('3',test_ag->v[test_ag->iw][test_ag->ic]);
			CHECK_EQUAL('2',test_ag->v[test_ag->iw][test_ag->ic+1]);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_eow)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->ic = 10;

			xcore::s32 test_bool = Ag_Func::ag_eow(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_TRUE(test_bool);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_end)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->iw = 3;
			test_ag->ic = 2;

			xcore::s32 test_bool = Ag_Func::ag_end(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_TRUE(test_bool);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_c)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			xtchar test_char = Ag_Func::ag_c(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('-',test_char);
			CHECK_EQUAL(0,test_ag->ic);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_c_next)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->ic = 2;

			xtchar test_char = Ag_Func::ag_c_next(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('o',test_char);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_cnn_next)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->iw = 1;
			test_ag->ic = 1;

			xtchar test_char = Ag_Func::ag_cnn_next(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('-',test_char);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_c_advance)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->ic = 3;

			xtchar test_char = Ag_Func::ag_c_advance(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('o',test_char);
			CHECK_EQUAL(4,test_ag->ic);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_backspace)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			xtchar test_char = Ag_Func::ag_backspace(test_ag);				/*ag will be reset*/
			test_ag->iw = 2;
			test_ag->ic = 0;
			xtchar test_char2 = Ag_Func::ag_backspace(test_ag);				/*ag will go back to the previous word and return the last character*/
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL('-',test_char);
			CHECK_EQUAL('2',test_char2);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_backword)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			/*Is there a problem here? If the current word of ag is 0*/
			Ag_Func::ag_backword(test_ag);
			/*And now ag->iw is -1. It's not correct in logical.
				Should we fix the ag->iw to 0 if the original iw is 0 and we call the ag_backword.*/
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL(-1,test_ag->iw);										/*something strange*/
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_s)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			char* test_string1 = Ag_Func::ag_s(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			char* test_string2 = Ag_Func::ag_s(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			char* test_string3 = Ag_Func::ag_s(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			char* test_string4 = Ag_Func::ag_s(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			char* test_string5 = Ag_Func::ag_s(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			CHECK_NOT_NULL(test_ag);
			xbool cmpResult1 = x_strcmp(test_string1,"--longname");
			xbool cmpResult2 = x_strcmp(test_string2,"32");
			xbool cmpResult3 = x_strcmp(test_string3,"-p");
			xbool cmpResult4 = x_strcmp(test_string4,"90");
			CHECK_FALSE(cmpResult1);
			CHECK_FALSE(cmpResult2);
			CHECK_FALSE(cmpResult3);
			CHECK_FALSE(cmpResult4);
			CHECK_NULL(test_string5);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_s_next)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->ic = 3;

			char* test_string1 = Ag_Func::ag_s_next(test_ag);
			xbool cmpResult1 = x_strcmp(test_string1,"ngname");
			test_ag->ic = 9;
			char* test_string2 = Ag_Func::ag_s_next(test_ag);
			xbool cmpResult2 = x_strcmp(test_string2,"32");
			CHECK_NOT_NULL(test_ag);
			CHECK_FALSE(cmpResult1);
			CHECK_FALSE(cmpResult2);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_s_advance)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			char* test_string = Ag_Func::ag_s_advance(test_ag);
			xbool cmpResult = x_strcmp(test_string,"--longname");
			CHECK_NOT_NULL(test_ag);
			CHECK_FALSE(cmpResult);
			CHECK_EQUAL(1,test_ag->iw);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_clear)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			Ag_Func::ag_clear(test_ag);
			xbool test_result = Ag_Func::ag_eow(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_TRUE(test_result);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_argc)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			Ag_Func::ag_w_advance(test_ag);
			Ag_Func::ag_w_advance(test_ag);
			xcore::s32 test_result = Ag_Func::ag_argc(test_ag);
			CHECK_NOT_NULL(test_ag);
			CHECK_EQUAL(2,test_result);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_argv)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			Ag_Func::ag_w_advance(test_ag);
			char** test_stringPoint = Ag_Func::ag_argv(test_ag);
			xbool cmpResult = x_strcmp(*test_stringPoint,"32");
			CHECK_NOT_NULL(test_ag);
			CHECK_FALSE(cmpResult);
			Ag_Func::ag_free(test_ag);
		}

		UNITTEST_TEST(ag_copy_freeall)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			xcmdline::xargv* new_ag = NULL;
			new_ag = Ag_Func::ag_copy(test_ag);
			xbool cmpResult = x_strcmp(*(new_ag->v),*(test_ag->v));
			CHECK_NOT_NULL(test_ag);
			CHECK_NOT_NULL(new_ag);
			CHECK_EQUAL(test_ag->c,new_ag->c);
			CHECK_EQUAL(test_ag->ic,new_ag->ic);
			CHECK_EQUAL(test_ag->iw,new_ag->iw);
			CHECK_FALSE(cmpResult);
			Ag_Func::ag_free(test_ag);
			Ag_Func::ag_freeall(new_ag);
		}

		UNITTEST_TEST(ag_prepend_argv0)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);
			test_ag->iw = 2;

			char* new_argv0 = "new argv0";
			xcmdline::xargv* new_ag = Ag_Func::ag_prepend_argv0(test_ag,new_argv0);
			xbool cmpResult = x_strcmp("new argv0",*(new_ag->v));
			CHECK_NOT_NULL(test_ag);
			CHECK_FALSE(cmpResult);
			CHECK_EQUAL(test_ag->c - test_ag->iw+1,new_ag->c);
			Ag_Func::ag_free(test_ag); 
			Opt_Allocator::get_opt_allocator()->deallocate(new_ag->v);
			Opt_Allocator::get_opt_allocator()->deallocate(new_ag);
		}

		UNITTEST_TEST(argnext)
		{
			test_ag = Ag_Func::ag_new(argc,argvp);

			char* test_string1 = Ag_Func::argnext(test_ag);
			xbool cmpResult1 = x_strcmp("--longname",test_string1);
			char* test_string2 = Ag_Func::argnext(test_ag);
			xbool cmpResult2 = x_strcmp("32",test_string2);
			char* test_string3 = Ag_Func::argnext(test_ag);
			xbool cmpResult3 = x_strcmp("-p",test_string3);
			char* test_string4 = Ag_Func::argnext(test_ag);
			xbool cmpResult4 = x_strcmp("90",test_string4);
			char* test_string5 = Ag_Func::argnext(test_ag);
			xbool cmpResult5 = x_strcmp("\0",test_string5);
			CHECK_NOT_NULL(test_ag);
			CHECK_FALSE(cmpResult1);
			CHECK_FALSE(cmpResult2);
			CHECK_FALSE(cmpResult3);
			CHECK_FALSE(cmpResult4);
			CHECK_FALSE(cmpResult5);
			Ag_Func::ag_free(test_ag);
		}
	}
}
UNITTEST_SUITE_END


