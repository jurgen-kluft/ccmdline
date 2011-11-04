#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"

#include "xunittest\xunittest.h"


using namespace xcore;

UNITTEST_SUITE_BEGIN(xcmdline_tests_reg_all)
{
	UNITTEST_FIXTURE(reg_fun)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		

		UNITTEST_TEST(reg)
		{
			u64	   regcbTest    =	0;	// =8 bytes
			char*  posVar		=	NULL;
			s32    regaTest		=	0;
			f64	   regTest		=	0.0;
			s16	   regcTest		=	0;
			f32	   regsTest		=	0.0f;
			char   regsbTest	=	'a';
			char*  regfTest		=	NULL;

			xcmdline::Opt_Reg::optrega(&regaTest,xcmdline::OPT_INT,'a',"rega","optrega");
			xcmdline::Opt_Reg::optreg(&regTest,xcmdline::OPT_DOUBLE,'b',"optreg");
			xcmdline::Opt_Reg::optregc(&regcTest,xcmdline::OPT_SHORT,'c');
			xcmdline::Opt_Reg::optregcb(&regcbTest,xcmdline::OPT_LONG,'d',"optregcb");
			xcmdline::Opt_Reg::optregs(&regsTest,xcmdline::OPT_FLOAT,"regs");
			xcmdline::Opt_Reg::optregsb(&regsbTest,xcmdline::OPT_CHAR,"regsb","optregsb");
			xcmdline::Opt_Reg::optregf(&regfTest,xcmdline::OPT_STRING,'e',"regf","optregf");
			xcmdline::Opt_Reg::optregp(&posVar,xcmdline::OPT_STRING,"inputVar","Read data from txt");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"--inputVar=1.txt",
				"-a10",
				"-b3.5",
				"-c2",
				"-d700",
				"--regs=8.8",
				"--regsb=b",
 				"-e",
 				"AAA"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_NOT_NULL(posVar);
			CHECK_EQUAL(10,regaTest);
			CHECK_EQUAL(3.5,regTest);
			CHECK_EQUAL(2,regcTest);
			CHECK_EQUAL(700,regcbTest);
			CHECK_EQUAL((f32)8.8f,regsTest);
			CHECK_EQUAL('b',regsbTest);
			CHECK_NOT_NULL(regfTest);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(posVar);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(regfTest);

		}
	}
}
UNITTEST_SUITE_END
