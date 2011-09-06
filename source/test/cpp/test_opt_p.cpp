#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_p.h"

#include "xunittest\xunittest.h"
#include "xcmdline\private\ag.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(xcmdline_tests_opt_p)
{
	UNITTEST_FIXTURE(reg_opt_p)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}


		UNITTEST_TEST(regopt_p)
		{
			s32    s32Var	=	0;
			s64    s64Var	=	0;
			s8     s8Var	=	0;

			xcmdline::optrega(&s32Var,xcmdline::OPT_INT,'a',"s32Var","Register a s32 var");
			xcmdline::opthelp(&s32Var,"This is the help for s32Var.");
			xcmdline::optmode(&s32Var,xcmdline::OPT_POSITIONAL);

			xcmdline::optrega(&s64Var,xcmdline::OPT_LONG,'b',"s64Var","Register a s64 var");
			xcmdline::optmode(&s64Var,xcmdline::OPT_POSITIONAL);

			xcmdline::optrega(&s8Var,xcmdline::OPT_BYTE,'c',"s8Var","Register a s8 var");


			xcmdline::optTitle("Test_setTitle_opt_p");


			char*	argv[] =
			{
				"xcmdline_test_TestDebug_Win32",
//				"-a",									because the type of the variables are OPT_POSITIONAL,
//														so we can modify the value just by the order of the input.
				"12",
				"10"
			};


			xcore::s32 argc = sizeof(argv)/sizeof(char*);

			char** argvp = argv;
			char*** argvv = (char***)(&argvp);
			xcmdline::opt(&argc, argvv);

			xcmdline::opt_get_help('a');
			char* title = xcmdline::optgetTitle();
			bool result = x_strcmp("Test_setTitle_opt_p",title);

			s32 n = xcmdline::opt_char_number('b');
			char* m = xcmdline::optstrval(0);
			bool result_2 = x_strcmp("12",m);

			char* k = xcmdline::optstrtyp(2);
			bool result_3 = x_strcmp("<s8>",k);

			xcmdline::short_usage();
			xcmdline::long_usage();

			char* string1Temp;
			string1Temp = (char* )xcmdline::get_opt_allocator()->allocate(sizeof("aaa"),4);
			string1Temp[0] = 'a';
			string1Temp[1] = 'a';
			string1Temp[2] = 'a';
			char** string1 = &string1Temp;
			char* string2 = "string2";
 
 			xcmdline::opt_setstring(string1,string2);
 			bool result_4 = x_strcmp("string2",*string1);

			char* justifyTest = "Today is Monday.\n      Now I want to sleep.";
 			char* justifyResult = xcmdline::opt_justify(justifyTest,10,5,5,"aaa");
// 
 			xcmdline::opt_free();
 
			CHECK_EQUAL(0,result);
			CHECK_EQUAL(1,n);
			CHECK_EQUAL(0,result_2);
			CHECK_EQUAL(0,result_3);
			CHECK_EQUAL(12,s32Var);
			CHECK_EQUAL(10,s64Var);

			CHECK_EQUAL(0,result_4);

			xcmdline::get_opt_allocator()->deallocate(*string1);
			xcmdline::get_opt_allocator()->deallocate(justifyResult);
			
		}

		UNITTEST_TEST(parse_test)
		{
			s32    testParsePos	   =	0;
			s64    testParseDelim  =	0;
			s32    testParseLongDelim	=	0;

			xcmdline::optrega(&testParsePos,xcmdline::OPT_INT,'d',"testParsePos","Register a s32 var");
			xcmdline::optmode(&testParsePos,xcmdline::OPT_POSITIONAL);

			xcmdline::optrega(&testParseDelim,xcmdline::OPT_LONG,'e',"testParseDelim","Register a s32 var");

			xcmdline::optrega(&testParseLongDelim,xcmdline::OPT_INT,'f',"testParseLongDelim","Register a s32 var");

			char*	ag[] = 
			{
				"14"
			};

			xcmdline::xargv* forTestPos = xcmdline::ag_new(1,ag);

			char*	ag_2[] = 
			{
				"e25"
			};

			xcmdline::xargv* forTestDelim = xcmdline::ag_new(1,ag_2);

			char*	ag_3[] = 
			{
				"testParseLongDelim=78"
			};

			xcmdline::xargv* forTestLongDelim = xcmdline::ag_new(1,ag_3);

			xcmdline::opt_parse_positional(forTestPos);
 			xcmdline::opt_parse_delim(forTestDelim);
 			xcmdline::opt_parse_longdelim(forTestLongDelim);

			CHECK_EQUAL(14,testParsePos);
 			CHECK_EQUAL(25,testParseDelim);
 			CHECK_EQUAL(78,testParseLongDelim);


			xcmdline::opt_free();

			xcmdline::ag_free(forTestPos);
 			xcmdline::ag_free(forTestDelim);
 			xcmdline::ag_free(forTestLongDelim);


		}


		UNITTEST_TEST(aton_test)
		{
			char*	atofTest	=	"38.324";
			char*	atoiTest	=	"32432";
			char*	atouTest	=	"232";
			char*	isvalidTest	=	" 5 e";

			s64	   atoiResult	=	xcmdline::opt_atoi(atoiTest);
			f64    atofResult	=	xcmdline::opt_atof(atofTest);
			u64    atouResult	=	xcmdline::opt_atou(atouTest);

			s32	   isvalidResult	=	xcmdline::opt_isvalidnumber(isvalidTest);

			CHECK_EQUAL(38.324,atofResult);
			CHECK_EQUAL(32432,atoiResult);
			CHECK_EQUAL(232,atouResult);
			CHECK_EQUAL(0,isvalidResult);

		}


		UNITTEST_TEST(opt_process_test)
		{
			s32	   optProcessTestA	=	0;
			s8     optProcessTestB	=	0;
			f64    optProcessTestC	=	0.0;

			xcmdline::optrega(&optProcessTestA,xcmdline::OPT_INT,'a',"ProcessTestA","Register a s32 var");
			xcmdline::optrega(&optProcessTestB,xcmdline::OPT_BYTE,'b',"ProcessTestB","Register a s8 var");
			xcmdline::optrega(&optProcessTestC,xcmdline::OPT_DOUBLE,'c',"ProcessTestC","Register a f64 var");
		
			char*	ag[] =
			{
				//"xcmdline_test_TestDebug_Win32",
				"-a98",
				"-b-9"
			};

			xcmdline::opt_process(2,ag);

			char programNameTest[] = "xcmdline_test.exe";
			char* programName = xcmdline::short_progname(programNameTest);
			bool result = x_strcmp("xcmdline_test",programName);

			char* lineProcessTest = "--ProcessTestC=3.324";
			xcmdline::opt_lineprocess(lineProcessTest);

			xcmdline::opt_help(NULL);
			xcmdline::opt_usage();

// 
 			xcmdline::opt_free();

			CHECK_EQUAL(98,optProcessTestA);
			CHECK_EQUAL(-9,optProcessTestB);
			CHECK_EQUAL(3.324,optProcessTestC);
			CHECK_EQUAL(0,result);
		}
	}
}
UNITTEST_SUITE_END
