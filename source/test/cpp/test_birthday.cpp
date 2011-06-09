#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"
#include "xcmdline\private\opt.h"

#include "xunittest\xunittest.h"

using namespace xcore;

UNITTEST_SUITE_BEGIN(birthday)
{
	UNITTEST_FIXTURE(main)
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
			int m;
			/* fix whatever int variable v is pointing to */
			m = *((int *)v);
			if (m < 1 || m > 12) 
				m=1;
			*((int *)v) = m;
			return OPT_OK;
		} 
		UNITTEST_TEST(full)
		{
			xcmdline::optreg(&month,xcmdline::OPT_INT,'m',"Month");
			//xcmdline::optlongname(&month,"month");
			xcmdline::opthook(&month,fix_mon);

			xcmdline::optrega(&day,xcmdline::OPT_INT,'d',"day","Day");
			xcmdline::opthelp(&day,"Use day of month, should be less than 32");

			xcmdline::optreg(&year,xcmdline::OPT_INT,'y',"Year...");
			xcmdline::optreg(&year,xcmdline::OPT_INT,'Y',"Year");
			xcmdline::optdescript(&year,"What Year");
			xcmdline::opthook(&year,checkyear);

			xcmdline::optregs(&julian,xcmdline::OPT_BOOL,"julian");

			xcmdline::optregp(&who,xcmdline::OPT_STRING,"who","Who to say hello to");

		}
	}
}
UNITTEST_SUITE_END
