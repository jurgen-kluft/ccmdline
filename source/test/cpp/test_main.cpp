#include "xbase/x_base.h"
#include "xbase/x_allocator.h"
#include "xbase/x_console.h"

#include "xunittest/xunittest.h"
#include "xunittest/private/ut_ReportAssert.h"

UNITTEST_SUITE_LIST(xCmdlineUnitTest);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, test_x_cmdline);

namespace xcore
{
	// Our own assert handler
	class UnitTestAssertHandler : public xcore::asserthandler_t
	{
	public:
		UnitTestAssertHandler()
		{
			NumberOfAsserts = 0;
		}

		virtual xcore::bool	handle_assert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
		{
			UnitTest::reportAssert(exprString, fileName, lineNumber);
			NumberOfAsserts++;
			return false;
		}


		xcore::s32		NumberOfAsserts;
	};

	class UnitTestAllocator : public UnitTest::Allocator
	{
		xcore::alloc_t*	mAllocator;
	public:
						UnitTestAllocator(xcore::alloc_t* allocator)	{ mAllocator = allocator; }
		virtual void*	Allocate(xsize_t size)								{ return mAllocator->allocate((u32)size, sizeof(void*)); }
		virtual size_t	Deallocate(void* ptr)								{ return (size_t)mAllocator->deallocate(ptr); }
	};

	class TestAllocator : public alloc_t
	{
		alloc_t*		mAllocator;
	public:
							TestAllocator(alloc_t* allocator) : mAllocator(allocator) { }

		virtual void*		v_allocate(u32 size, u32 alignment)
		{
			UnitTest::IncNumAllocations();
			return mAllocator->allocate(size, alignment);
		}

		virtual u32			v_deallocate(void* mem)
		{
			UnitTest::DecNumAllocations();
			return mAllocator->deallocate(mem);
		}

		virtual void		v_release()
		{
			mAllocator->release();
			mAllocator = NULL;
		}
	};
}

xcore::alloc_t* gTestAllocator = NULL;
xcore::UnitTestAssertHandler gAssertHandler;


bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	xbase::x_Init();

#ifdef TARGET_DEBUG
	xcore::asserthandler_t::sRegisterHandler(&gAssertHandler);
#endif

	xcore::alloc_t* systemAllocator = xcore::alloc_t::get_system();
	xcore::UnitTestAllocator unittestAllocator( systemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);

	xcore::console->write("Configuration: ");
	xcore::console->writeLine(TARGET_FULL_DESCR_STR);

	xcore::TestAllocator testAllocator(systemAllocator);
	gTestAllocator = &testAllocator;

	int r = UNITTEST_SUITE_RUN(reporter, xCmdlineUnitTest);
	if (UnitTest::GetNumAllocations()!=0)
	{
		reporter.reportFailure(__FILE__, __LINE__, "xunittest", "memory leaks detected!");
		r = -1;
	}

	gTestAllocator->release();

	UnitTest::SetAllocator(NULL);

	xbase::x_Exit();
	return r==0;
}

