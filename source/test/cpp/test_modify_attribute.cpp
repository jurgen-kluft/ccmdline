#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"

#include "xunittest\xunittest.h"

//#include <iostream>


using namespace xcore;

extern xcore::x_iallocator* gSystemAllocator;

UNITTEST_SUITE_BEGIN(xcmdline_tests_attribute)
{
	UNITTEST_FIXTURE(Modifying_attribute)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(test_modify_name)
		{
			s32	   testSwitch	=	0;
			s32    longNameTest	=	30;
			s32	   longNameTest_n	=	30;
			s32    shortNameTest	=	30;
			s32    shortNameTest_n	=	30;

			s16*	x	=	NULL;
			s32    nx	=	0;

			xcmdline::Opt_Reg::optrega_array(&nx,&x,xcmdline::OPT_SHORT,'x',"xarray","Array for test");
			xbool boolVar	=	1;


//			test the opthelp function
			xcmdline::Opt_Reg::optrega(&boolVar,xcmdline::OPT_BOOL,'b',"boolVar","boolVar");
			xcmdline::Opt_Reg::opthelp(&boolVar,"This is the boolVar help");

			xcmdline::Opt_Reg::optreg(&longNameTest,xcmdline::OPT_INT,'l',"SetLongNameTest");
			xcmdline::Opt_Reg::optlongname(&longNameTest,"longName");

			s32 n = xcmdline::Opt_Reg::optreg(&longNameTest_n,xcmdline::OPT_INT,'n',"SetLongNameTest_n");
			xcmdline::Opt_Reg::optlongname_n(n,"nLongName");

			xcmdline::Opt_Reg::optregs(&shortNameTest,xcmdline::OPT_INT,"shortName");
			xcmdline::Opt_Reg::optchar(&shortNameTest,'s');

			n = xcmdline::Opt_Reg::optregsb(&shortNameTest_n,xcmdline::OPT_INT,"shortName","SetShortNameTest_n");
			xcmdline::Opt_Reg::optchar_n(n,'h');

			xcmdline::Opt_Reg::optrega(&testSwitch,xcmdline::OPT_INT,'t',"testSwitch","Just test the optMode");
			xcmdline::Opt_Reg::optmode(&testSwitch,xcmdline::OPT_POSITIONAL);


			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"--boolVar=0",
 				"--xarray",
 				"10,20,30",
  				"?b",
 				"--longName",
 				"100",
 				"--nLongName=80",
 				"-s10",
 				"-h",
 				"15",
 				"--testSwitch=1"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Proc::opt_get_help('b');
			
			CHECK_EQUAL(0, boolVar);
			CHECK_EQUAL(100,longNameTest);
			CHECK_EQUAL(80,longNameTest_n);
			CHECK_EQUAL(10,shortNameTest);
			CHECK_EQUAL(15,shortNameTest_n);
			CHECK_EQUAL(3,nx);
			CHECK_NOT_NULL(x);
			CHECK_EQUAL(10,x[0]);
			CHECK_EQUAL(20,x[1]);
			CHECK_EQUAL(30,x[2]);
			CHECK_EQUAL(1,testSwitch);

			xcmdline::Opt_Reg::opt_free();
		}

		s32	   desTest    =    0;
		s32    desTest_n  =    0;
		s32    hookTest   =	   0;
		s32    hookTest_n =    0;

		s32 check(void *v)
		{
			if (hookTest > 10)
			{
				hookTest = 30;
			}

			if (hookTest_n > 20)
			{
				hookTest_n = 20;
			}
			return OPT_OK;
		}

		UNITTEST_TEST(test_des_hook)
		{
			xcmdline::Opt_Reg::optrega(&desTest,xcmdline::OPT_INT,'a',"desTest","Test the optdescript function");
			xcmdline::Opt_Reg::optdescript(&desTest,"Modify desTest des");

			s32 n = xcmdline::Opt_Reg::optrega(&desTest_n,xcmdline::OPT_INT,'b',"desTest_n","Test the optdescript_n function");
			xcmdline::Opt_Reg::optdescript_n(n,"Modify desTest des_n");

			xcmdline::Opt_Reg::optrega(&hookTest,xcmdline::OPT_INT,'c',"hookTest","Test opthook");
			xcmdline::Opt_Reg::opthook(&hookTest,check);

			n = xcmdline::Opt_Reg::optrega(&hookTest_n,xcmdline::OPT_INT,'d',"hookTest_n","Test opthook_n");
			xcmdline::Opt_Reg::opthook_n(n,check);

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-c300",
				"-d300"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL(30,hookTest);
			CHECK_EQUAL(20,hookTest_n);
		}

		s32 MoreInfo(void)
		{
			//Just for test
			//std::cout<< "More Info about Usage." << std::endl;
			return OPT_OK;
		}

		UNITTEST_TEST(test_optForWholeProgram)
		{
			s32    defaultTest    =    0;

			xcmdline::Opt_Reg::optUsage("Message for optUsage");
			xcmdline::Opt_Reg::optTitle("Test_setTitle");
			xcmdline::Opt_Reg::optProgName("Test_setProgName");
			xcmdline::Opt_Reg::optVersion("1.0");
			xcmdline::Opt_Reg::optEnvVarName("OPT");
			xcmdline::Opt_Reg::optDefaultString("?a");
			xcmdline::Opt_Reg::optAdditionalUsage(MoreInfo);

			xcmdline::Opt_Reg::optrega(&defaultTest,xcmdline::OPT_INT,'t',"defaultTest","Test the optDefualtString");
			xcmdline::Opt_Reg::opthelp(&defaultTest,"Test the optDefualtString");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"--version",
				"--defaultTest=10"
			};

			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::Opt_Proc::opt(&argc, argvv);
			xcmdline::Opt_Proc::optPrintUsage();
			s32 n = xcmdline::Opt_Proc::optinvoked(&defaultTest);
			xcmdline::Opt_Reg::opt_free();

			CHECK_EQUAL(1,n); 
			CHECK_EQUAL(10,defaultTest);
		}
	}
}
UNITTEST_SUITE_END
