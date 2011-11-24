#include "xbase\x_types.h"
#include "xbase\x_allocator.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_proc.h"

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

			xcmdline::Opt_Proc opt_proc;

			opt_proc.optrega_array(&nx,&x,xcmdline::OPT_SHORT,'x',"xarray","Array for test");
			xbool boolVar	=	1;


//			test the opthelp function
			opt_proc.optrega(&boolVar,xcmdline::OPT_BOOL,'b',"boolVar","boolVar");
			opt_proc.opthelp(&boolVar,"This is the boolVar help");

			opt_proc.optreg(&longNameTest,xcmdline::OPT_INT,'l',"SetLongNameTest");
			opt_proc.optlongname(&longNameTest,"longName");

			s32 n = opt_proc.optreg(&longNameTest_n,xcmdline::OPT_INT,'n',"SetLongNameTest_n");
			opt_proc.optlongname_n(n,"nLongName");

			opt_proc.optregs(&shortNameTest,xcmdline::OPT_INT,"shortName");
			opt_proc.optchar(&shortNameTest,'s');

			n = opt_proc.optregsb(&shortNameTest_n,xcmdline::OPT_INT,"shortName","SetShortNameTest_n");
			opt_proc.optchar_n(n,'h');

			opt_proc.optrega(&testSwitch,xcmdline::OPT_INT,'t',"testSwitch","Just test the optMode");
			opt_proc.optmode(&testSwitch,xcmdline::OPT_POSITIONAL);


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
			opt_proc.opt(&argc, argvv);
			opt_proc.opt_get_help('b');
			
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

			opt_proc.opt_free();
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
			xcmdline::Opt_Proc opt_proc;

			opt_proc.optrega(&desTest,xcmdline::OPT_INT,'a',"desTest","Test the optdescript function");
			opt_proc.optdescript(&desTest,"Modify desTest des");

			s32 n = opt_proc.optrega(&desTest_n,xcmdline::OPT_INT,'b',"desTest_n","Test the optdescript_n function");
			opt_proc.optdescript_n(n,"Modify desTest des_n");

			opt_proc.optrega(&hookTest,xcmdline::OPT_INT,'c',"hookTest","Test opthook");
			opt_proc.opthook(&hookTest,check);

			n = opt_proc.optrega(&hookTest_n,xcmdline::OPT_INT,'d',"hookTest_n","Test opthook_n");
			opt_proc.opthook_n(n,check);

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"-c300",
				"-d300"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			opt_proc.opt(&argc, argvv);
			opt_proc.opt_free();

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
			xcmdline::Opt_Proc opt_proc;

			opt_proc.optUsage("Message for optUsage");
			opt_proc.optTitle("Test_setTitle");
			opt_proc.optProgName("Test_setProgName");
			opt_proc.optVersion("1.0");
			opt_proc.optEnvVarName("OPT");
			opt_proc.optDefaultString("?a");
			opt_proc.optAdditionalUsage(MoreInfo);
			opt_proc.optrega(&defaultTest,xcmdline::OPT_INT,'t',"defaultTest","Test the optDefualtString");
			opt_proc.opthelp(&defaultTest,"Test the optDefualtString");

			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
				"--version",
				"--defaultTest=10"
			};

			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			opt_proc.opt(&argc, argvv);
			opt_proc.optPrintUsage();
			s32 n = opt_proc.optinvoked(&defaultTest);
			opt_proc.opt_free();

			CHECK_EQUAL(1,n); 
			CHECK_EQUAL(10,defaultTest);
		}
	}
}
UNITTEST_SUITE_END
