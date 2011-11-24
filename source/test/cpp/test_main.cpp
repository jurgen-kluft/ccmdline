#include "xbase\x_target.h"
#include "xbase\x_types.h"
#include "xbase\x_string.h"
#include "xbase\x_allocator.h"

#include "xcmdline\private\opt.h"
#include "xcmdline\private\opt_proc.h"

#include "xunittest\xunittest.h"




#ifdef	COUTDEBUG
#include <iostream>
using namespace std;
#endif


UNITTEST_SUITE_LIST(xCmdlineUnitTest);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests_attribute);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests_reg_all);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests_reg);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_tests_opt_p);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_ag_tests);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, xcmdline_array_register);
UNITTEST_SUITE_DECLARE(xCmdlineUnitTest, test_xcmdline_reg_opt);
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
#if TEST_ALLOCATIONS
			mAllocationIdx = 0;
			mDeallocationIdx = 0;
#endif
		}

		xcore::x_iallocator*	mAllocator;
		s32						mNumAllocations;

		virtual const char*	name() const
		{
			return "xcmdline unittest test heap allocator";
		}

#if TEST_ALLOCATIONS

		void*		mAllocations[100];
		int			mAllocationIdx;

		void*		mDeallocations[100];
		int			mDeallocationIdx;
#endif

		virtual void*		allocate(u32 size, u32 alignment)
		{
			++mNumAllocations;
			void * mem = mAllocator->allocate(size, alignment);

#ifdef COUTDEBUG
			cout << "allocate address:  " << mem << "  size:  " << size << "  number:  " << mNumAllocations << endl;
#endif

#if TEST_ALLOCATIONS
			mAllocations[mAllocationIdx++] = mem;
#endif
			return mem;
		}

		virtual void*		reallocate(void* mem, u32 size, u32 alignment)
		{
			void * new_mem = mAllocator->reallocate(mem, size, alignment);
#ifdef COUTDEBUG
			cout << "reallocate address:  " << new_mem << "  size:  " << size << "  number:  " << mNumAllocations << endl;
#endif

#if TEST_ALLOCATIONS

			if (new_mem != mem)
			{
				for (s32 i=0; i<mAllocationIdx; ++i)
				{
					if (mAllocations[i] == mem)
					{
						mAllocations[i] = new_mem;
						break;
					}
				}
			}
#endif
			return new_mem;
		}

		virtual void		deallocate(void* mem)
		{
			if (mem==0)
				return;
#ifdef COUTDEBUG
			cout << "deallocate address:  " << mem;
#endif


#if TEST_ALLOCATIONS
			// See if we are freeing memory which has been allocated through allocate()
			xbool was_allocated = false;
			for (s32 i=0; i<mAllocationIdx && !was_allocated; ++i)
			{
				was_allocated = (mAllocations[i] == mem);
			}
			ASSERT(was_allocated);

			// This is a history of deallocations, see if we are double freeing the same address
			for (s32 i=0; i<mDeallocationIdx; ++i)
			{
				if (mem == mDeallocations[i])
				{
					// Double free
					ASSERT(false);
				}
			}

			mDeallocations[mDeallocationIdx++] = mem;
#else
		mAllocator->deallocate(mem);
#endif
			--mNumAllocations;

#ifdef COUTDEBUG
			cout << "  number:  " << mNumAllocations << endl;
#endif
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
bool gRunUnitTest(UnitTest::TestReporter& reporter)
{
	gSystemAllocator = xcore::gCreateSystemAllocator();
	UnitTestAllocator unittestAllocator( gSystemAllocator );
	UnitTest::SetAllocator(&unittestAllocator);

	xcore::TestHeapAllocator libHeapAllocator(gSystemAllocator);
	xcore::xstring::sSetAllocator(&libHeapAllocator);
	xcore::xcmdline::Opt_Allocator::set_opt_allocator(&libHeapAllocator);
	
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

	xcore::xcmdline::Opt_Allocator::set_opt_allocator(NULL);
	xcore::xstring::sSetAllocator(NULL);

	UnitTest::SetAllocator(NULL);
	gSystemAllocator->release();
	gSystemAllocator=NULL;
	return r==0;
}

