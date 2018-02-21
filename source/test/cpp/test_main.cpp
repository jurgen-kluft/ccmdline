#include "xbase/x_target.h"
#include "xbase/x_allocator.h"

#include "xunittest/xunittest.h"


UNITTEST_SUITE_LIST(xCmdlineUnitTest);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, test_x_cmdline);

class UnitTestAllocator : public UnitTest::Allocator
{
public:
	xcore::x_iallocator*	mAllocator;
	int						mNumAllocations;

	UnitTestAllocator(xcore::x_iallocator* allocator)
		: mNumAllocations(0)
	{
		mAllocator = allocator;
	}

	void*	Allocate(size_t size)
	{
		++mNumAllocations;
		return mAllocator->allocate(size, 4);
	}
	void	Deallocate(void* ptr)
	{
		--mNumAllocations;
		mAllocator->deallocate(ptr);
	}
	void	Release()
	{
		mAllocator->release();
	}
};

bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	UnitTestAllocator unittestAllocator( xcore::gCreateSystemAllocator() );
	UnitTest::SetAllocator(&unittestAllocator);

	int r = UNITTEST_SUITE_RUN(reporter, xCmdlineUnitTest);
	if (unittestAllocator.mNumAllocations!=0)
	{
		reporter.reportFailure(__FILE__, __LINE__, "xunittest", "memory leaks detected!");
		r = -1;
	}

	unittestAllocator.Release();
	return r==0;
}
