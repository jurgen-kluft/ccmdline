#include "xbase\x_target.h"
#include "xbase\x_types.h"
#include "xbase\x_allocator.h"

#include "xunittest\xunittest.h"


UNITTEST_SUITE_LIST(xCmdlineUnitTest);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, test_x_cmdline);


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
			void * mem = mAllocator->allocate(size, alignment);
			return mem;
		}

		virtual void*		reallocate(void* mem, u32 size, u32 alignment)
		{
			void * new_mem = mAllocator->reallocate(mem, size, alignment);
			return new_mem;
		}

		virtual void		deallocate(void* mem)
		{
			if (mem==0)
				return;
			mAllocator->deallocate(mem);
			--mNumAllocations;
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

xcore::x_iallocator* gSystemAllocator=NULL;
xcore::x_iallocator* gHeapAllocator=NULL;

bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	gSystemAllocator = xcore::gCreateSystemAllocator();

	UnitTestAllocator unittestAllocator( gSystemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);

	xcore::TestHeapAllocator libHeapAllocator(gSystemAllocator);
	gHeapAllocator = &libHeapAllocator;
	
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

	gHeapAllocator = NULL;
	UnitTest::SetAllocator(NULL);

	gSystemAllocator->release();
	gSystemAllocator=NULL;
	return r==0;
}

