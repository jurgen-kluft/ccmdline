#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_proc.h"

#include "xunittest\xunittest.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(xcmdline_tests)
{
	UNITTEST_FIXTURE(birthday)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}
			
		s32 month=4;
		s32 day=24;
		s32 year=1993;
		s32 julian=0;
		char *who=NULL;
		s32 altversion=99;
		
		s32 checkyear(void *v)
		{
			if (year == 2000)
			{
				return OPT_ERROR;
			}
			return OPT_OK;
		}
		s32 fix_mon(void *v)
		{
			s32 m;
			/* fix whatever s32 variable v is pointing to */
			m = *((s32 *)v);
			if (m < 1 || m > 12) 
				m=1;
			*((s32 *)v) = m;
			return OPT_OK;
		} 
		UNITTEST_TEST(full)
		{	
			xcmdline::Opt_Proc opt_proc;

			opt_proc.optrega(&day,  xcmdline::OPT_INT, 'd', "day", "Day of month");
 			opt_proc.opthelp(&day,"Use day of month, should be less than 32");

			opt_proc.optrega(&month,xcmdline::OPT_INT,'m',"month","Month");
			opt_proc.opthook(&month,fix_mon);

			opt_proc.optreg(&year,xcmdline::OPT_INT,'y',"Year...");
			opt_proc.optreg(&year,xcmdline::OPT_INT,'Y',"Year");
			opt_proc.optdescript(&year,"What Year");
			opt_proc.opthook(&year,checkyear);

			opt_proc.optregs(&julian,xcmdline::OPT_BOOL,"julian");

			opt_proc.optregp(&who,xcmdline::OPT_STRING,"who","Who to say hello to");

			char*	argv[] =
			{
 				"xcmdline_test_TestDebug_Win32",
				"-y",
				"2011",
				"--month",
				"12",
				"--day",
				"30",
				"-who",
				"Jurgen"
				//"@bday.opt"
			};
			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = &argvp;
			opt_proc.opt(&argc, argvv);
			opt_proc.opt_free();

			CHECK_EQUAL(2011, year);
			CHECK_EQUAL(12, month);
			CHECK_EQUAL(30, day);
			CHECK_NOT_NULL(who);

			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(who);
		}
	}
}
UNITTEST_SUITE_END
