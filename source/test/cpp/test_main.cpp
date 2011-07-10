#include "xbase\x_target.h"
#include "xbase\x_types.h"
#include "xbase\x_string.h"
#include "xbase\x_allocator.h"

#include "xcmdline\private\opt.h"

#include "xunittest\xunittest.h"

UNITTEST_SUITE_LIST(xCmdlineUnitTest);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests);


namespace xcore
{
	class TestHeapAllocator : public x_iallocator
	{
	public:
		TestHeapAllocator(xcore::x_iallocator* allocator)
			: mAllocator(allocator)
			, mNumAllocations(0)
		{
		}

		xcore::x_iallocator*	mAllocator;
		s32						mNumAllocations;

		virtual const char*	name() const
		{
			return "xcmdline unittest test heap allocator";
		}

		virtual void*		allocate(u32 size, u32 alignment)
		{
			++mNumAllocations;
			return mAllocator->allocate(size, alignment);
		}

		virtual void*		reallocate(void* mem, u32 size, u32 alignment)
		{
			return mAllocator->reallocate(mem, size, alignment);
		}

		virtual void		deallocate(void* mem)
		{
			if (mem==0)
				return;

			--mNumAllocations;
			mAllocator->deallocate(mem);
		}

		virtual void		release()
		{
		}
	};
}

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

	virtual void*	Allocate(int size)
	{
		++mNumAllocations;
		return mAllocator->allocate(size, 4);
	}
	virtual void	Deallocate(void* ptr)
	{
		--mNumAllocations;
		mAllocator->deallocate(ptr);
	}
};

bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	xcore::x_iallocator* systemAllocator = xcore::gCreateSystemAllocator();
	UnitTestAllocator unittestAllocator( systemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);

	xcore::TestHeapAllocator libHeapAllocator(systemAllocator);
	xcore::xstring::sSetAllocator(&libHeapAllocator);
	xcore::xcmdline::set_opt_allocator(&libHeapAllocator);
	
	int r = UNITTEST_SUITE_RUN(reporter, xCmdlineUnitTest);
	if (unittestAllocator.mNumAllocations!=0)
	{
		reporter.reportFailure(__FILE__, __LINE__, "xunittest", "memory leaks detected!");
		r = -1;
	}
	if (libHeapAllocator.mNumAllocations!=0)
	{
		reporter.reportFailure(__FILE__, __LINE__, "xcmdline", "memory leaks detected!");
		r = -1;
	}

	xcore::xcmdline::set_opt_allocator(NULL);
	xcore::xstring::sSetAllocator(NULL);

	UnitTest::SetAllocator(NULL);
	systemAllocator->release();
	return r==0;
}

