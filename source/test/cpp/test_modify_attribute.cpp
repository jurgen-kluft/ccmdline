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

			xcmdline::optrega_array(&nx,&x,xcmdline::OPT_SHORT,'x',"xarray","Array for test");
			xbool boolVar	=	1;


//			test the opthelp function
			xcmdline::optrega(&boolVar,xcmdline::OPT_BOOL,'b',"boolVar","boolVar");
			xcmdline::opthelp(&boolVar,"This is the boolVar help");

			xcmdline::optreg(&longNameTest,xcmdline::OPT_INT,'l',"SetLongNameTest");
			xcmdline::optlongname(&longNameTest,"longName");

			s32 n = xcmdline::optreg(&longNameTest_n,xcmdline::OPT_INT,'n',"SetLongNameTest_n");
			xcmdline::optlongname_n(n,"nLongName");

			xcmdline::optregs(&shortNameTest,xcmdline::OPT_INT,"shortName");
			xcmdline::optchar(&shortNameTest,'s');

			n = xcmdline::optregsb(&shortNameTest_n,xcmdline::OPT_INT,"shortName","SetShortNameTest_n");
			xcmdline::optchar_n(n,'h');

			xcmdline::optrega(&testSwitch,xcmdline::OPT_INT,'t',"testSwitch","Just test the optMode");
			xcmdline::optmode(&testSwitch,xcmdline::OPT_POSITIONAL);


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
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_get_help('b');
			
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

			xcmdline::opt_free();
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
			xcmdline::optrega(&desTest,xcmdline::OPT_INT,'a',"desTest","Test the optdescript function");
			xcmdline::optdescript(&desTest,"Modify desTest des");

			s32 n = xcmdline::optrega(&desTest_n,xcmdline::OPT_INT,'b',"desTest_n","Test the optdescript_n function");
			xcmdline::optdescript_n(n,"Modify desTest des_n");

			xcmdline::optrega(&hookTest,xcmdline::OPT_INT,'c',"hookTest","Test opthook");
			xcmdline::opthook(&hookTest,check);

			n = xcmdline::optrega(&hookTest_n,xcmdline::OPT_INT,'d',"hookTest_n","Test opthook_n");
			xcmdline::opthook_n(n,check);

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-c300",
				"-d300"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::opt(&argc, argvv);
			xcmdline::opt_free();

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

			xcmdline::optUsage("Message for optUsage");
			xcmdline::optTitle("Test_setTitle");
			xcmdline::optProgName("Test_setProgName");
			xcmdline::optVersion("1.0");
			xcmdline::optEnvVarName("OPT");
			xcmdline::optDefaultString("?a");
			xcmdline::optAdditionalUsage(MoreInfo);

			xcmdline::optrega(&defaultTest,xcmdline::OPT_INT,'t',"defaultTest","Test the optDefualtString");
			xcmdline::opthelp(&defaultTest,"Test the optDefualtString");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"--version",
				"--defaultTest=10"
			};

			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::opt(&argc, argvv);
			xcmdline::optPrintUsage();
			s32 n = xcmdline::optinvoked(&defaultTest);
			xcmdline::opt_free();

			CHECK_EQUAL(1,n); 
			CHECK_EQUAL(10,defaultTest);
		}
	}
}
UNITTEST_SUITE_END
