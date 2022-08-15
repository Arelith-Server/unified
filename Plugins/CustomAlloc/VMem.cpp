/*
	Copyright 2010 - 2017 PureDev Software Ltd. All Rights Reserved.

	This file is part of VMem.

	VMem is dual licensed. For use in open source software VMem can
	be used under the GNU license. For use in commercial applications a
	license can be purchased from PureDev Software.

	If used under the GNU license VMem is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

	Under the GNU Public License VMem is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with VMem. If not, see <http://www.gnu.org/licenses/>.

	VMem can only be used in commercial products if a commercial license has been purchased
	from PureDev Software. Please see http://www.puredevsoftware.com/vmem/License.htm
*/

// This file contains all cpp and hpp files in the VMem library.
// Use this file instead of the library for a quick and convenient way of adding VMem to your project.


#include "VMem.h"


//------------------------------------------------------------------------
//
// VMem_PCH.h
//
#include <stdlib.h>
#include <new>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef VMEM_OS_WIN
	#include <tchar.h>
#endif

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_WIN)
	#ifdef __UNREAL__
		#include "Windows/AllowWindowsPlatformTypes.h"
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <intrin.h>
	#ifdef __UNREAL__
		#include "Windows/HideWindowsPlatformTypes.h"
	#endif
#elif defined(VMEM_PLATFORM_XBOXONE)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_XBOX360)
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemCore.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#include <limits.h>

#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4668)
#endif
#include <stdint.h>
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

#include <memory.h>

#ifdef VMEM_OS_WIN
	#include <tchar.h>
#else
	#include <stdio.h>
#endif

#ifdef VMEM_PLATFORM_ANDROID
	#include <stdarg.h>
#endif

#ifdef VMEM_PLATFORM_PS4
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(disable:4127)	// conditional expression is constant
#endif

//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_POINTER_PREFIX "0x"
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
	#define VMEM_POINTER_PREFIX ""
#endif

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#define VMEM_DEBUG_BREAK __debugbreak()
#else
	#define VMEM_DEBUG_BREAK __builtin_trap()
#endif

//------------------------------------------------------------------------
#define VMEM_START_MULTI_LINE_MACRO	do {
#define VMEM_END_MULTI_LINE_MACRO } while (false)

//------------------------------------------------------------------------
#define VMEM_UNREFERENCED_PARAM(p) (void)(p)

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT(b, m)					\
		VMEM_START_MULTI_LINE_MACRO				\
		if(!(b))								\
		{										\
			if(!VMem::Break(_T(m) _T("\n")))	\
				VMEM_DEBUG_BREAK;				\
		}										\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT(b, m) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT2(b, m, a1, a2)					\
		VMEM_START_MULTI_LINE_MACRO						\
		if(!(b))										\
		{												\
			VMem::DebugWrite(_T(m) _T("\n"), a1, a2);	\
			if(!VMem::Break(_T("VMem Assert\n")))		\
				VMEM_DEBUG_BREAK;						\
		}												\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT1(b, m, a) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_MEM_CHECK(p_value, expected_value)	\
		VMEM_START_MULTI_LINE_MACRO					\
		if(*(p_value) != (expected_value))			\
		{											\
			VMem::DebugWrite(sizeof(expected_value) == 8 ? _T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value ") _T(VMEM_POINTER_PREFIX) _T("%p : expected value ") _T(VMEM_POINTER_PREFIX) _T("%p\n") : _T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value 0x%08x : expected value 0x%08x\n"), (p_value), *(p_value), (expected_value));	\
			if(!VMem::Break(_T("VMem Assert\n")))	\
				VMEM_DEBUG_BREAK;					\
		}											\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_MEM_CHECK(p_value, expected_value) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT_MEM(b, p_value)				\
		VMEM_START_MULTI_LINE_MACRO					\
		if(!(b))									\
		{											\
			VMem::DebugWrite(_T("Memory corruption at ") _T(VMEM_POINTER_PREFIX) _T("%p : value ") _T(VMEM_POINTER_PREFIX) _T("%p\n"), (p_value), *((void**)(p_value)));	\
			if(!VMem::Break(_T("VMem Assert\n")))	\
				VMEM_DEBUG_BREAK;					\
		}											\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_ASSERT_MEM(b, p_value) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	#define VMEM_BREAK(m)				\
		VMEM_START_MULTI_LINE_MACRO		\
		if(!VMem::Break(_T(m)))			\
			VMEM_DEBUG_BREAK;			\
		VMEM_END_MULTI_LINE_MACRO
#else
	#define VMEM_BREAK(m) ((void)0)
#endif

//------------------------------------------------------------------------
// compile time assert
#define VMEM_STATIC_ASSERT(expr, message) static_assert(expr, message)

//------------------------------------------------------------------------
// for code that is only needed when asserts are turned on
#ifdef VMEM_ASSERTS
	#define VMEM_ASSERT_CODE(e) e
#else
	#define VMEM_ASSERT_CODE(e) ((void)0)
#endif

//------------------------------------------------------------------------
// for code that is only needed when memsetting is turned on
#if defined(VMEM_ENABLE_MEMSET)
	#define VMEM_MEMSET(p, value, size) memset(p, value, size)
#elif defined(VMEM_MEMSET_ONLY_SMALL)
	#define VMEM_MEMSET(p, value, size) memset(p, value, VMem::VMin((size_t)size, (size_t)VMEM_MEMSET_ONLY_SMALL))
#else
	#define VMEM_MEMSET(p, value, size) VMEM_UNREFERENCED_PARAM(p)
#endif

//------------------------------------------------------------------------
// for code that is only needed when stats are turned on
#ifdef VMEM_ENABLE_STATS
	#define VMEM_STATS(e) e
#else
	#define VMEM_STATS(e) ((void)0)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_X64
	#define VMEM_X64_ONLY(s) s
#else
	#define VMEM_X64_ONLY(s)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_X64
	VMEM_STATIC_ASSERT(sizeof(void*) == sizeof(long long), "please undefine VMEM_X64 for x86 builds");
#else
	VMEM_STATIC_ASSERT(sizeof(void*) == sizeof(unsigned int), "please define VMEM_X64 for 64bit builds");
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#if VMEM_DEBUG_LEVEL == 0
		#define VMEM_FORCE_INLINE __forceinline
	#else
		#define VMEM_FORCE_INLINE inline
	#endif
	#define VMEM_NO_INLINE __declspec(noinline)
#else
	#define VMEM_FORCE_INLINE inline
	#define VMEM_NO_INLINE
#endif

//------------------------------------------------------------------------
// analysis macros
#if defined(VMEM_OS_WIN) &&  _MSC_FULL_VER >= 170060315
	#define VMEM_ASSUME(e) __analysis_assume(e)
	#define VMEM_ACQUIRES_LOCK(e) _Acquires_lock_(e)
	#define VMEM_RELEASES_LOCK(e) _Releases_lock_(e)
	#define VMEM_ACQUIRES_SHARED_LOCK(e) _Acquires_shared_lock_(e)
	#define VMEM_RELEASES_SHARED_LOCK(e) _Releases_shared_lock_(e)
	#define VMEM_ACQUIRES_EXCLUSIVE_LOCK(e) _Acquires_exclusive_lock_(e)
	#define VMEM_RELEASES_EXCLUSIVE_LOCK(e) _Releases_exclusive_lock_(e)
#else
	#define VMEM_ASSUME(e)
	#define VMEM_ACQUIRES_LOCK(e)
	#define VMEM_RELEASES_LOCK(e)
	#define VMEM_ACQUIRES_SHARED_LOCK(e)
	#define VMEM_RELEASES_SHARED_LOCK(e)
	#define VMEM_ACQUIRES_EXCLUSIVE_LOCK(e)
	#define VMEM_RELEASES_EXCLUSIVE_LOCK(e)
#endif

//------------------------------------------------------------------------
VMEM_STATIC_ASSERT(sizeof(size_t) == sizeof(void*), "VMEM_X64 define not set correctly");

//------------------------------------------------------------------------
#define VMEM_INTERNAL_ALIGNMENT sizeof(void*)

//------------------------------------------------------------------------
#if VMEM_DEBUG_LEVEL != 0 || defined(VMEM_ASSERTS)
	#define VMEM_SIMULATE_OOM 1
#else
	#define VMEM_SIMULATE_OOM 0
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	// used for MemPro stats
	enum VmemAllocatorType
	{
		vmem_BasicCoalesceHeap = 0,
		vmem_PageHeap,
		vmem_FSAHeap,
		vmem_CoalesceHeap,
		vmem_LargeHeap,
		vmem_End
	};

	//------------------------------------------------------------------------
	struct AlignedHeader
	{
		void* p;
	};

	//------------------------------------------------------------------------
	struct PhysicalAlloc
	{
		PhysicalAlloc()
		:	mp_Physical(0),
			m_Size(0),
			mp_Virtual(NULL),
			mp_OriginalMappedAddr(NULL)
		{
		}

		PhysicalAlloc(void* p_physical, size_t size, void* p_mapped_addr, void* p_original_mapped_addr)
		:	mp_Physical(p_physical),
			m_Size(size),
			mp_Virtual(p_mapped_addr),
			mp_OriginalMappedAddr(p_original_mapped_addr)
		{
		}

		void* mp_Physical;
		size_t m_Size;
		void* mp_Virtual;				// the virtual address that the physical address is mapped to  (can be NULL)
		void* mp_OriginalMappedAddr;	// the original virtual address that the physical address was mapped to before given to VMem
	};

	//------------------------------------------------------------------------
	class InternalAllocator
	{
	public:
		virtual void* Alloc(size_t size) = 0;
		virtual void Free(void* p, size_t size) = 0;
	};

	//------------------------------------------------------------------------
	void InitialiseCore();

	void UninitialiseCore();

	bool Break(const _TCHAR* p_message);

	void DebugWrite(const _TCHAR* p_message, ...);

	void SetDebugBreakFunction(void (*DebugBreakFn)(const _TCHAR* p_message));

	#if VMEM_SIMULATE_OOM
		int GetSimulateOOMCount();
		void SetSimulateOOMCount(int value);
		int GetInternalHeapSimulateOOMCount();
		void SetInternalHeapSimulateOOMCount(int value);
		int GetPhysicalOOMCount();
		void SetPhysicalOOMCount(int value);
	#endif

	#ifdef VMEM_ENABLE_STATS
		void WriteStats(const VMem::Stats& stats);
		void WriteStats(const VMemHeapStats& stats);
	#endif

	//------------------------------------------------------------------------
	typedef unsigned char byte;

	//------------------------------------------------------------------------
	typedef long long int64;
	typedef unsigned long long uint64;

	//------------------------------------------------------------------------
	typedef void (*MemProSendFn)(void*, int, void*);

	//------------------------------------------------------------------------
	#ifdef VMEM_OS_WIN
		#define VMEM_THREAD_LOCAL __declspec(thread)
	#else
		#define VMEM_THREAD_LOCAL __thread
	#endif

	//------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE T VMin(T a, T b)
	{
		return a < b ? a : b;
	}

	//------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE T VMax(T a, T b)
	{
		return a > b ? a : b;
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE void SendToMemPro(T& value, MemProSendFn send_fn, void* p_context)
	{
		send_fn(&value, sizeof(value), p_context);
	}

	//-----------------------------------------------------------------------------
	template<typename T>
	VMEM_FORCE_INLINE void SendEnumToMemPro(T value, MemProSendFn send_fn, void* p_context)
	{
		send_fn(&value, sizeof(value), p_context);
	}

	//-----------------------------------------------------------------------------
	VMEM_FORCE_INLINE bool IsPow2(unsigned int value)
	{
		return (value & (value-1)) == 0;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUp(int i, int alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignUp(size_t i, size_t alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignUp(size_t i, int alignment)
	{
		return (((i-1) / alignment) + 1) * alignment;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignUp(void* p, size_t alignment)
	{
		size_t i = (size_t)p;
		return (void*)((((i-1) / alignment) + 1) * alignment);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUpPow2(int i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		int mask = alignment - 1;
		return (i + mask) & ~mask;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t AlignSizeUpPow2(size_t i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		int mask = alignment - 1;
		return (i + mask) & ~mask;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignUpPow2(void* p, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		size_t i = (size_t)p;
		int mask = alignment - 1;
		return (void*)((i + mask) & ~mask);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignDownPow2(int i, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		return i & ~(alignment-1);
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void* AlignDownPow2(void* p, int alignment)
	{
		VMEM_ASSERT(IsPow2(alignment), "non-pow2 value passed to align function");
		size_t i = (size_t)p;
		return (void*)(i & ~(alignment-1));
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int AlignUpToNextPow2(int size)
	{
		VMEM_ASSERT(size > 1, "size must be > 0 in AlignUpToNextPow2");

		int s = 1;
		while(s < size)
		{
			VMEM_ASSERT(s != (1<<31), "size out of range in AlignUpToNextPow2");
			s <<= 1;
		}
		return s;
	}

	//------------------------------------------------------------------------
	void WriteAlloc(const void* p_alloc, size_t size);

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void SetGuards(void* p, int size)
	{
		VMEM_ASSERT((((size_t)p) & 3) == 0, "guard pointer misaligned");
		VMEM_ASSERT((size & 3) == 0, "guard pointer misaligned");

		unsigned int* p_guard = (unsigned int*)p;
		unsigned int* p_end_guard = (unsigned int*)((byte*)p + size);
		while(p_guard != p_end_guard)
			*p_guard++ = VMEM_GUARD_MEM;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE void CheckMemory(void* p, size_t size, unsigned int value)
	{
#ifdef VMEM_ASSERTS
		VMEM_ASSERT((((size_t)p) & 3) == 0, "CheckMemory pointer misaligned");
		VMEM_ASSERT((size & 3) == 0, "CheckMemory size misaligned");

		#ifdef VMEM_MEMSET_ONLY_SMALL
			size = VMem::VMin(size, (size_t)VMEM_MEMSET_ONLY_SMALL);
		#endif
		VMEM_ASSERT((size % sizeof(unsigned int)) == 0, "bad size");
		unsigned int* p_uint = (unsigned int*)p;
		unsigned int* p_end = (unsigned int*)((byte*)p + size);
		while(p_uint != p_end)
		{
			VMEM_MEM_CHECK(p_uint, value);
			++p_uint;
		}
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(value);
#endif
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE int ToInt(size_t size)
	{
		VMEM_ASSERT(size <= INT_MAX, "size out of range");
		return (int)size;
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE size_t ToSizeT(int value)
	{
		VMEM_ASSERT(value >= 0, "value out of range while converting to size_t");
		return (size_t)value;
	}

	//------------------------------------------------------------------------
	#ifndef VMEM_OS_WIN
		VMEM_FORCE_INLINE int _vstprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list argptr) { return vsprintf(buffer, format, argptr); }
	#endif

	//------------------------------------------------------------------------
	#ifndef VMEM_OS_WIN
		#define memcpy_s(p_dst, dst_size, p_src, src_size) memcpy(p_dst, p_src, src_size)
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_OS_WIN
		#define VMEM_RESTRICT __restrict
	#else
		#define VMEM_RESTRICT
	#endif
}

//------------------------------------------------------------------------
VMEM_STATIC_ASSERT(sizeof(VMem::byte) == 1, "sizeof(VMem::byte) == 1");
VMEM_STATIC_ASSERT(sizeof(int) == 4, "sizeof(int) == 4");
VMEM_STATIC_ASSERT(sizeof(unsigned int) == 4, "sizeof(unsigned int) == 4");
VMEM_STATIC_ASSERT(sizeof(long long) == 8, "sizeof(long long) == 8");
VMEM_STATIC_ASSERT(sizeof(VMem::uint64) == 8, "sizeof(VMem::uint64) == 8");

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemSys.h
//
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------

#if defined(VMEM_INC_INTEG_CHECK)
	#if defined(VMEM_PLATFORM_WIN)
		#include <intrin.h>
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#endif
#endif

//------------------------------------------------------------------------
namespace VMem
{
	void VMemSysCreate();

	void VMemSysDestroy();

	void* VirtualReserve(size_t size, int page_size, int reserve_flags);

	void VirtualRelease(void* p, size_t size);

	bool VirtualCommit(void* p, size_t size, int page_size, int commit_flags);

	void VirtualDecommit(void* p, size_t size, int page_size, int commit_flags);

	size_t GetReservedBytes();

	size_t GetCommittedBytes();

	void Break();

	void DebugWrite(const _TCHAR* p_str, ...);

	void DumpSystemMemory();

	void VMemSysSetLogFunction(void (*LogFn)(const _TCHAR* p_message));

	void VMemSysCheckIntegrity();

	void VMemSysWriteStats();

	//-----------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
	bool Committed(void* p, size_t size);
	bool Reserved(void* p, size_t size);
#endif

	//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_ALIGN_8
#else
	#define VMEM_ALIGN_8 __attribute__((aligned(8)))
#endif
	
	//-----------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	#if defined(VMEM_OS_WIN)
		VMEM_FORCE_INLINE void VMem_InterlockedIncrement(volatile long& value)
		{
			_InterlockedIncrement(&value);
		}
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#elif defined(VMEM_OS_LINUX)
		// can't gurantee platform has interlocked instructions, so use critical section.
		// Doesn't have to be particularly fast because it's only used in debug code
		class IncCritSec
		{
		public:
			IncCritSec()
			{
				pthread_mutexattr_t attr;
				pthread_mutexattr_init(&attr);
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
				pthread_mutex_init(&cs, &attr);
			}

			~IncCritSec() { pthread_mutex_destroy(&cs); }

			void Enter() { pthread_mutex_lock(&cs); }

			void Leave() { pthread_mutex_unlock(&cs); }

		private:
			pthread_mutex_t cs;
		};
		IncCritSec g_InterlockedAddCritSec;

		VMEM_FORCE_INLINE void VMem_InterlockedIncrement(volatile long& value)
		{
			g_InterlockedAddCritSec.Enter();
			++value;
			g_InterlockedAddCritSec.Leave();
		}
	#else
		#error platform not defined
	#endif
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// BasicFSA.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class BasicFSA
	{
	public:
		//------------------------------------------------------------------------
		BasicFSA(int page_size)
		:	m_PageSize(page_size),
			mp_FreeList(NULL),
			mp_BlockList(NULL),
			m_ItemsPerBlock(ToInt((m_PageSize - AlignUp(sizeof(void*), (size_t)ItemSize())) / ItemSize()))
		{
		}

		//------------------------------------------------------------------------
		~BasicFSA()
		{
			void* p_block = mp_BlockList;
			while(p_block)
			{
				void* p_next_block = *(void**)p_block;

				VMem::VirtualDecommit(p_block, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
				VMem::VirtualRelease(p_block, m_PageSize);

				VMEM_STATS(m_Stats.m_Reserved -= m_PageSize);
				VMEM_STATS(m_Stats.m_Overhead -= m_PageSize);

				p_block = p_next_block;
			}
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE T* Alloc()
		{
			T* p = mp_FreeList;

			if(p)
				mp_FreeList = *(T**)p;
			else
				p = AllocateBlock();

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0,"BasicFSA Alloc returned misaligned pointer");

			return p;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Free(T* p)
		{
			*(void**)p = mp_FreeList;
			mp_FreeList = p;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_Stats;
		}
#endif

	private:
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE static int ItemSize()
		{
			return ToInt(AlignSizeUpPow2(sizeof(T), VMEM_INTERNAL_ALIGNMENT));
		}

		//------------------------------------------------------------------------
		VMEM_NO_INLINE T* AllocateBlock()
		{
			void* p_block = VMem::VirtualReserve(m_PageSize, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p_block)
			{
				return NULL;
			}

			if(!VMem::VirtualCommit(p_block, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				VMem::VirtualRelease(p_block, m_PageSize);
				return NULL;
			}

			*(void**)p_block = mp_BlockList;
			mp_BlockList = p_block;

			T* p_first_item = (T*)((byte*)p_block + AlignUp(sizeof(void*), ItemSize()));

			T* p_item = p_first_item + 1;
			mp_FreeList = p_item;

			int count = m_ItemsPerBlock;
			for(int i=2; i<count; ++i, ++p_item)
				*(void**)p_item = p_item + 1;
			*(void**)p_item = NULL;

			VMEM_STATS(m_Stats.m_Reserved += m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead += m_PageSize);

			return p_first_item;
		}

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;

		T* mp_FreeList;

		void* mp_BlockList;
#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
		int m_ItemsPerBlock;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemCriticalSection.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_LINUX
	#include <pthread.h>
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
//	#define VMEM_USE_SLIM_READ_WRITE_LOCK
#endif

//-----------------------------------------------------------------------------
//#define VMEM_CHECK_FOR_RECURSIVE_LOCKS

//-----------------------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------------------
	// This is NOT re-entrant. Avoids having to have an extra interlocked increment.
	// Yields after 40 checks.
	class CriticalSection
	{
	public:
		//-----------------------------------------------------------------------------
		CriticalSection()
#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
		:	m_Locked(false)
#endif
		{
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)

			InitializeSRWLock(&m_CritSec);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)

			// disable debug info in release
			#if VMEM_DEBUG_LEVEL == 0
				int flags = CRITICAL_SECTION_NO_DEBUG_INFO;
			#else
				int flags = 0;
			#endif
			InitializeCriticalSectionEx(&m_CritSec, m_SpinCount, flags);

#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_LINUX)

			VMEM_ASSERT((((int64)this) % 8) == 0, "CriticalSection not aligned to 8 bytes");

			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&cs, &attr);

#else
			#error platform not defined
#endif
		}

		//-----------------------------------------------------------------------------
		~CriticalSection()
		{
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
			DeleteCriticalSection(&m_CritSec);
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_LINUX)
			pthread_mutex_destroy(&cs);
#else
			#error platform not defined
#endif
		}

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_ACQUIRES_LOCK(this->m_CritSec) VMEM_FORCE_INLINE void Enter()
		{
			EnterCriticalSection(&m_CritSec);

			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				VMEM_ASSERT(!m_Locked, "critical section locked recursively! Please contact slynch@puredevsoftware.com");
				m_Locked = true;
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_ACQUIRES_LOCK(this->m_CritSec) void Enter()
		{
			AcquireSRWLockExclusive(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_LINUX)
		VMEM_FORCE_INLINE void Enter()
		{
			#if defined(VMEM_PLATFORM_PS4)
				#error Please contact slynch@puredevsoftware.com for this platform
			#else
				pthread_mutex_lock(&cs);
			#endif

			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				VMEM_ASSERT(!m_Locked, "critical section locked recursively! Please contact slynch@puredevsoftware.com");
				m_Locked = true;
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				m_Locked = false;
			#endif

			ReleaseSRWLockExclusive(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
				m_Locked = false;
			#endif

			LeaveCriticalSection(&m_CritSec);
		}
#endif

		//-----------------------------------------------------------------------------
#if defined(VMEM_OS_LINUX)
		VMEM_FORCE_INLINE VMEM_RELEASES_LOCK(this->m_CritSec) void Leave()
		{
			#if defined(VMEM_PLATFORM_PS4)
				#error Please contact slynch@puredevsoftware.com for this platform
			#else
				pthread_mutex_unlock(&cs);
			#endif
		}
#endif

		//-----------------------------------------------------------------------------
	private:
		CriticalSection(const CriticalSection&);
		CriticalSection& operator=(const CriticalSection&);

		//-----------------------------------------------------------------------------
		// data
	private:
#if defined(VMEM_OS_WIN) && defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		SRWLOCK m_CritSec;
#elif defined(VMEM_OS_WIN) && !defined(VMEM_USE_SLIM_READ_WRITE_LOCK)
		static const int m_SpinCount = 4000;
		CRITICAL_SECTION m_CritSec;
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_LINUX)
		pthread_mutex_t cs;
#else
		#error platform not defined
#endif

#ifdef VMEM_CHECK_FOR_RECURSIVE_LOCKS
		bool m_Locked;
#endif
	};

	//-----------------------------------------------------------------------------
	class CriticalSectionScope
	{
	public:
		VMEM_FORCE_INLINE CriticalSectionScope(CriticalSection& critical_section) : m_CriticalSection(critical_section) { critical_section.Enter(); }
		VMEM_FORCE_INLINE ~CriticalSectionScope() { m_CriticalSection.Leave(); }

	private:
		CriticalSectionScope& operator=(const CriticalSectionScope& other);

		CriticalSection& m_CriticalSection;
	};

	//-----------------------------------------------------------------------------
	class ReadWriteLock
	{
	public:
		//-----------------------------------------------------------------------------
		ReadWriteLock()
		{
#if defined(VMEM_OS_WIN)
			InitializeSRWLock(&m_CritSec);
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_ACQUIRES_SHARED_LOCK(this->m_CritSec) void EnterShared()
		{
#if defined(VMEM_OS_WIN)
			AcquireSRWLockShared(&m_CritSec);
#else
			m_CritSec.Enter();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_RELEASES_SHARED_LOCK(this->m_CritSec) void LeaveShared()
		{
#if defined(VMEM_OS_WIN)
			ReleaseSRWLockShared(&m_CritSec);
#else
			m_CritSec.Leave();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_ACQUIRES_EXCLUSIVE_LOCK(this->m_CritSec) void EnterExclusive()
		{
#if defined(VMEM_OS_WIN)
			AcquireSRWLockExclusive(&m_CritSec);
#else
			m_CritSec.Enter();
#endif
		}

		//-----------------------------------------------------------------------------
		VMEM_RELEASES_EXCLUSIVE_LOCK(this->m_CritSec) void LeaveExclusive()
		{
#if defined(VMEM_OS_WIN)

			#ifdef VMEM_OS_WIN
				#pragma warning(push)
				#pragma warning(disable : 26110)	// disable release but not enter warning
			#endif

			ReleaseSRWLockExclusive(&m_CritSec);

			#ifdef VMEM_OS_WIN
				#pragma warning(pop)
			#endif
#else
			m_CritSec.Leave();
#endif
		}

		//-----------------------------------------------------------------------------
		// data
	private:
#if defined(VMEM_OS_WIN)
		SRWLOCK m_CritSec;
#else
		CriticalSection m_CritSec;
#endif
	};

	//-----------------------------------------------------------------------------
	class ReadLockScope
	{
	public:
		ReadLockScope(ReadWriteLock& read_write_lock)
		:	m_ReadWriteLock(read_write_lock)
		{
			read_write_lock.EnterShared();
		}

		~ReadLockScope()
		{
			m_ReadWriteLock.LeaveShared();
		}

	private:
		ReadLockScope(const ReadLockScope&);
		ReadLockScope& operator=(const ReadLockScope&);

	private:
		//-----------------------------------------------------------------------------
		// data
		ReadWriteLock& m_ReadWriteLock;
	};

	//-----------------------------------------------------------------------------
	class WriteLockScope
	{
	public:
		WriteLockScope(ReadWriteLock& read_write_lock)
		:	m_ReadWriteLock(read_write_lock)
		{
			read_write_lock.EnterExclusive();
		}

		~WriteLockScope()
		{
			m_ReadWriteLock.LeaveExclusive();
		}

	private:
		WriteLockScope(const WriteLockScope&);
		WriteLockScope& operator=(const WriteLockScope&);

	private:
		//-----------------------------------------------------------------------------
		// data
		ReadWriteLock& m_ReadWriteLock;
	};
}

//-----------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemHashMap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------
//#define VMEM_PROFILE_HASHMAP

//-----------------------------------------------------------------
#ifdef VMEM_PROFILE_HASHMAP
	#include <time.h>
#endif

//-----------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------
	template<typename TKey, typename TValue>
	class HashMap
	{
	public:
		//-----------------------------------------------------------------
		struct Pair
		{
			TKey m_Key;
			TValue m_Value;
		};

		//-----------------------------------------------------------------
		// Iterator used to iterate over the items of the set.
		// Must call MoveNext to move to the first element. To be used like
		// this:
		//	HashMap<TKey, TValue>::Iteartor iter(set);
		//	while(iter.MoveNext())
		//	{
		//		const HashMap<TKey, TValue>::Pair& pair = iter.GetValue();
		//		//...
		//	}
		class Iterator
		{
			friend class HashMap;

		public:
			//-----------------------------------------------------------------
			Iterator(const HashMap& hash_map)
			{
				Initialise(&hash_map);
			}

			//-----------------------------------------------------------------
			// if this iterator has reached the end do nothing and return false
			// otherwise move to the next item and return true.
			// The first time this is called moves to the first element.
			bool MoveNext()
			{
				const int capacity = mp_HashMap->m_Capacity;
				if(m_Index == capacity)
				{
					return false;
				}

				// move to the next non-empty item
				++m_Index;
				++mp_Pair;

				if(m_Index == capacity)
				{
					mp_Pair = NULL;
					return false;
				}

				while(!mp_HashMap->IsItemInUse(m_Index))
				{
					++m_Index;
					++mp_Pair;

					if(m_Index == capacity)
					{
						mp_Pair = NULL;
						return false;
					}
				}

				return true;
			}

			//-----------------------------------------------------------------
			const TKey& GetKey() const
			{
				VMEM_ASSERT(mp_Pair, "");		// Iterator not at a valid location
				VMEM_ASSUME(mp_Pair);
				return (*mp_Pair)->m_Key;
			}

			//-----------------------------------------------------------------
			TValue& GetValue()
			{
				VMEM_ASSERT(mp_Pair, "");		// Iterator not at a valid location
				VMEM_ASSUME(mp_Pair);
				return (*mp_Pair)->m_Value;
			}

		private:
			//-----------------------------------------------------------------
			void Initialise(const HashMap* p_hash_map)
			{
				mp_HashMap = p_hash_map;

				if(p_hash_map->m_Capacity)
				{
					m_Index = -1;
					mp_Pair = p_hash_map->mp_Table - 1;
				}
				else
				{
					m_Index = 0;
					mp_Pair = NULL;
				}
			}

			//-----------------------------------------------------------------
			// data
		private:
			const HashMap<TKey, TValue>* mp_HashMap;
			int m_Index;
			Pair** mp_Pair;
		};

		//-----------------------------------------------------------------
		// The default capacity of the set. The capacity is the number
		// of elements that the set is expected to hold. The set will resized
		// when the item count is greater than the capacity;
		HashMap(int page_size)
		:	m_PageSize(page_size),
			m_Capacity(0),
			mp_Table(NULL),
			m_Count(0),
			mp_ItemPool(NULL),
			mp_FreePair(NULL),
			m_AllocedMemory(0)
#ifdef VMEM_PROFILE_HASHMAP
			,m_IterAcc(0)
			,m_IterCount(0)
#endif
		{
		}

		//-----------------------------------------------------------------
		bool Initialise(int capacity=m_DefaultCapacity)
		{
			if(capacity)
			{
				if(!AllocTable(GetNextPow2((256 * capacity) / m_Margin)))
					return false;
			}

			return true;
		}

		//-----------------------------------------------------------------
		bool Initialised() const
		{
			return mp_Table != NULL;
		}

		//-----------------------------------------------------------------
		~HashMap()
		{
			Clear();
		}

		//-----------------------------------------------------------------
		void Clear()
		{
			if(mp_Table)
			{
				for(int i=0; i<m_Capacity; ++i)
				{
					Pair* p_pair = mp_Table[i];
					if(p_pair)
						p_pair->~Pair();
				}

				InternalFree(mp_Table, m_Capacity * sizeof(Pair*));
			}

			m_Capacity = 0;
			mp_Table = NULL;
			m_Count = 0;

			FreePools();
		}

		//-----------------------------------------------------------------
		// Add a value to this set.
		// If this set already contains the value does nothing.
		bool Add(const TKey& key, const TValue& value)
		{
			if(m_Capacity == 0 || m_Count == (m_Margin * m_Capacity) / 256)
			{
				if(!Grow())
					return false;
			}

			const int index = GetItemIndex(key);
			
			VMEM_ASSERT(mp_Table, "mp_Table is null");

			if(IsItemInUse(index))
			{
				mp_Table[index]->m_Value = value;
			}
			else
			{
				// make a copy of the value
				Pair* p_pair = AllocPair();
				if(!p_pair)
					return false;
				p_pair->m_Key = key;
				p_pair->m_Value = value;

				// add to table
				mp_Table[index] = p_pair;

				++m_Count;
			}

			return true;
		}

		//-----------------------------------------------------------------
		// if this set contains the value set value to the existing value and
		// return true, otherwise set to the default value and return false.
		bool TryGetValue(const TKey& key, TValue& value) const
		{
			if(!mp_Table)
				return false;

			const int index = GetItemIndex(key);
			if(IsItemInUse(index))
			{
				value = mp_Table[index]->m_Value;
				return true;
			}
			else
			{
				return false;
			}
		}

		//-----------------------------------------------------------------
		// If this set contains the specifed value remove it
		// and return true, otherwise do nothing and return false.
		TValue Remove(const TKey& key)
		{
			VMEM_ASSERT(mp_Table, "null table");
			VMEM_ASSUME(mp_Table);

			int remove_index = GetItemIndex(key);
			VMEM_ASSERT(IsItemInUse(remove_index), "item no in use");

			Pair* p_pair = mp_Table[remove_index];

			TValue value = p_pair->m_Value;

			// find first index in this array
			int srch_index = remove_index;
			int first_index = remove_index;
			if(!srch_index)
			{
				srch_index = m_Capacity;
			}
			--srch_index;
			while(IsItemInUse(srch_index))
			{
				first_index = srch_index;
				if(!srch_index)
				{
					srch_index = m_Capacity;
				}
				--srch_index;
			}

			bool found = false;
			for(;;)
			{
				// find the last item in the array that can replace the item being removed
				int i = (remove_index + 1) & (m_Capacity-1);

				int swap_index = m_InvalidIndex;
				while(IsItemInUse(i))
				{
					const unsigned int srch_hash_code = mp_Table[i]->m_Key.GetHashCode();
					const int srch_insert_index = srch_hash_code & (m_Capacity-1);

					if(InRange(srch_insert_index, first_index, remove_index))
					{
						swap_index = i;
						found = true;
					}

					i = (i + 1) & (m_Capacity-1);
				}

				// swap the item
				if(found)
				{
					mp_Table[remove_index] = mp_Table[swap_index];
					remove_index = swap_index;
					found = false;
				}
				else
				{
					break;
				}
			}

			// remove the last item
			mp_Table[remove_index] = NULL;

			// free this item
			FreePair(p_pair);

			--m_Count;
			return value;
		}

		//-----------------------------------------------------------------
		Iterator GetIterator() const
		{
			return Iterator(*this);
		}

		//-----------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//-----------------------------------------------------------------
		const TValue& operator[](const TKey& key) const
		{
			const int index = GetItemIndex(key);
			VMEM_ASSERT(IsItemInUse(index), "");
			return mp_Table[index]->m_Value;
		}

		//-----------------------------------------------------------------
		bool Resize(int new_capacity)
		{
			new_capacity = GetNextPow2(new_capacity);

			// keep a copy of the old table
			Pair** p_old_table = mp_Table;
			const int old_capacity = m_Capacity;

			// allocate the new table
			if(!AllocTable(new_capacity))
				return false;

			// copy the values from the old to the new table
			Pair** p_old_pair = p_old_table;
			for(int i=0; i<old_capacity; ++i, ++p_old_pair)
			{
				Pair* p_pair = *p_old_pair;
				if(p_pair)
				{
					const int index = GetItemIndex(p_pair->m_Key);
					mp_Table[index] = p_pair;
				}
			}

			if(p_old_table)
				InternalFree(p_old_table, old_capacity * sizeof(Pair*));

			return true;
		}

		//-----------------------------------------------------------------
		int GetAllocedMemory() const
		{
			return m_AllocedMemory;
		}

	private:
		//-----------------------------------------------------------------
		void* InternalAlloc(int size)
		{
			size = AlignUpPow2(size, m_PageSize);
			void* p = VMem::VirtualReserve(size, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p)
				return NULL;

			if(!VMem::VirtualCommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				VMem::VirtualRelease(p, size);
				return NULL;
			}

			m_AllocedMemory += size;

			return p;
		}

		//-----------------------------------------------------------------
		void InternalFree(void* p, int size)
		{
			size = AlignUpPow2(size, m_PageSize);
			VMem::VirtualDecommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
			VMem::VirtualRelease(p, size);
			m_AllocedMemory -= size;
		}

		//-----------------------------------------------------------------
		static int GetNextPow2(int value)
		{
			int p = 2;
			while(p < value)
				p *= 2;
			return p;
		}

		//-----------------------------------------------------------------
		bool AllocTable(const int capacity)
		{
			VMEM_ASSERT(capacity < m_MaxCapacity, "");

			// allocate a block of memory for the table
			if(capacity > 0)
			{
				const int size = capacity * sizeof(Pair*);
				Pair** p_table = (Pair**)InternalAlloc(size);
				if(!p_table)
					return false;

				mp_Table = p_table;
				memset(p_table, 0, size);
			}

			m_Capacity = capacity;

			return true;
		}

		//-----------------------------------------------------------------
		bool IsItemInUse(const int index) const
		{
			return mp_Table[index] != NULL;
		}

		//-----------------------------------------------------------------
		int GetItemIndex(const TKey& key) const
		{
			VMEM_ASSERT(mp_Table, "");
			VMEM_ASSUME(mp_Table);

			const unsigned int hash = key.GetHashCode();
			int srch_index = hash & (m_Capacity-1);
			while(IsItemInUse(srch_index) && !(mp_Table[srch_index]->m_Key == key))
			{
				srch_index = (srch_index + 1) & (m_Capacity-1);
#ifdef VMEM_PROFILE_HASHMAP
				++m_IterAcc;
#endif
			}

#ifdef VMEM_PROFILE_HASHMAP
			++m_IterCount;
			double average = m_IterAcc / (double)m_IterCount;
			if(average > 2.0)
			{
				static int last_write_time = 0;
				int now = clock();
				if(now - last_write_time > 1000)
				{
					last_write_time = now;
					DebugWrite("WARNING: HashMap average: %f\n", (float)average);
				}
			}
#endif
			return srch_index;
		}

		//-----------------------------------------------------------------
		// Increase the capacity of the table.
		bool Grow()
		{
			const int new_capacity = m_Capacity ? 2*m_Capacity : m_DefaultCapacity;
			return Resize(new_capacity);
		}

		//-----------------------------------------------------------------
		static bool InRange(
			const int index,
			const int start_index,
			const int end_index)
		{
			return (start_index <= end_index) ?
				index >= start_index && index <= end_index :
				index >= start_index || index <= end_index;
		}

		//-----------------------------------------------------------------
		void FreePools()
		{
			byte* p_pool = mp_ItemPool;
			while(p_pool)
			{
				byte* p_next_pool = *(byte**)p_pool;
				InternalFree(p_pool, m_PageSize);
				p_pool = p_next_pool;
			}
			mp_ItemPool = NULL;
			mp_FreePair = NULL;
		}

		//-----------------------------------------------------------------
		Pair* AllocPair()
		{
			if(!mp_FreePair)
			{
				// allocate a new pool and link to pool list
				byte* p_new_pool = (byte*)InternalAlloc(m_PageSize);
				if(!p_new_pool)
					return NULL;
				*(byte**)p_new_pool = mp_ItemPool;
				mp_ItemPool = p_new_pool;

				// link all items onto free list
				mp_FreePair = p_new_pool + sizeof(Pair);
				byte* p = (byte*)mp_FreePair;
				int item_count = m_PageSize / sizeof(Pair) - 2;	// subtract 2 for pool pointer and last item
				VMEM_ASSERT(item_count, "");
				for(int i=0; i<item_count; ++i, p+=sizeof(Pair))
				{
					*(byte**)p = p + sizeof(Pair);
				}
				*(byte**)p = NULL;
			}

			// take item off free list
			Pair* p_pair = (Pair*)mp_FreePair;
			mp_FreePair = *(byte**)mp_FreePair;

			// construct the pair
			new (p_pair)Pair;

			return p_pair;
		}

		//-----------------------------------------------------------------
		void FreePair(Pair* p_pair)
		{
			p_pair->~Pair();

			*(byte**)p_pair = mp_FreePair;
			mp_FreePair = (byte*)p_pair;
		}

		//-----------------------------------------------------------------
		// data
	private:
		static const int m_DefaultCapacity = 1024;
		static const int m_InvalidIndex = 0xffffffff;
		static const int m_MaxCapacity = 0x7fffffff;
		static const int m_Margin = (40 * 256) / 100;

		int m_PageSize;

		int m_Capacity;			// the current capacity of this set, will always be >= m_Margin*m_Count/256
		Pair** mp_Table;		// NULL for a set with capacity 0
		int m_Count;			// the current number of items in this set, will always be <= m_Margin*m_Count/256

		byte* mp_ItemPool;
		byte* mp_FreePair;

		int m_AllocedMemory;

#ifdef VMEM_PROFILE_HASHMAP
		mutable uint64 m_IterAcc;
		mutable uint64 m_IterCount;
#endif
	};

	//------------------------------------------------------------------------
	struct AddrKey
	{
		AddrKey() : m_Addr(0) {}

		AddrKey(void* p)
		:	m_Addr(p),
			m_HashCode(CalculateHashCode())
		{
		}

		bool operator==(const AddrKey& other) const { return m_Addr == other.m_Addr; }

		void* GetAddr() const { return m_Addr; }

		unsigned int GetHashCode() const { return m_HashCode; }

	private:
		unsigned int CalculateHashCode() const
		{
#ifdef VMEM_X64
			uint64 key = (uint64)m_Addr;
			key = (~key) + (key << 18);
			key = key ^ (key >> 31);
			key = key * 21;
			key = key ^ (key >> 11);
			key = key + (key << 6);
			key = key ^ (key >> 22);
			return (unsigned int)key;
#else
			unsigned int a = (unsigned int)m_Addr;
			a = (a ^ 61) ^ (a >> 16);
			a = a + (a << 3);
			a = a ^ (a >> 4);
			a = a * 0x27d4eb2d;
			a = a ^ (a >> 15);
			return a;
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
		void* m_Addr;
		unsigned int m_HashCode;
	};
}

//-----------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// AlignedLargeHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	struct LargeAlignedAlloc
	{
		size_t m_Size;
		size_t m_AlignedSize;
	};

	//------------------------------------------------------------------------
	class AlignedLargeHeap
	{
	public:
		AlignedLargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem);

		~AlignedLargeHeap();

		bool Initialise();

		void* Alloc(size_t size, size_t alignment);

		bool Free(void* p);

		size_t GetSize(void* p) const;

		bool Owns(void* p) const;

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
		size_t WriteAllocs();
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

		void AcquireLockShared() const;

		void ReleaseLockShared() const;

	private:
		AlignedLargeHeap(const AlignedLargeHeap&);
		void operator=(const AlignedLargeHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		mutable VMem::ReadWriteLock m_ReadWriteLock;

		HashMap<AddrKey, LargeAlignedAlloc> m_Allocs;

		int m_ReserveFlags;
		int m_CommitFlags;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// List.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#define VMEM_LIST_FULL_CHECKING 0

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<class NodeT>
	class List
	{
	public:
		//------------------------------------------------------------------------
		List()
		{
			VMEM_ASSERT_CODE(m_Count = 0);
			m_Node.mp_Prev = m_Node.mp_Next = &m_Node;
		}

		//------------------------------------------------------------------------
		bool Empty() const
		{
			return m_Node.mp_Next == &m_Node;
		}

		//------------------------------------------------------------------------
		NodeT* GetHead() const
		{
			return m_Node.mp_Next;
		}

		//------------------------------------------------------------------------
		NodeT* GetTail() const
		{
			return m_Node.mp_Prev;
		}

		//------------------------------------------------------------------------
		NodeT* GetIterEnd()
		{
			return &m_Node;
		}

		//------------------------------------------------------------------------
		const NodeT* GetIterEnd() const
		{
			return &m_Node;
		}

		//------------------------------------------------------------------------
		void AddHead(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
			#endif

			Insert(&m_Node, p_node);
		}

		//------------------------------------------------------------------------
		void AddTail(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
			#endif

			Insert(m_Node.mp_Prev, p_node);
		}

		//------------------------------------------------------------------------
		// insert p_node after p_prev
		void Insert(NodeT* p_prev, NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(!Contains(p_node), "VMem List has been corrupted");
				VMEM_ASSERT(p_prev == &m_Node || Contains(p_prev), "VMem List has been corrupted");
			#endif
			VMEM_ASSERT(!p_node->mp_Next && !p_node->mp_Prev, "VMem list has been corrupted");

			NodeT* p_next = p_prev->mp_Next;
			p_prev->mp_Next = p_node;
			p_node->mp_Prev = p_prev;
			p_node->mp_Next = p_next;
			p_next->mp_Prev = p_node;

			VMEM_ASSERT_CODE(++m_Count);
		}

		//------------------------------------------------------------------------
		void Remove(NodeT* p_node)
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
				VMEM_ASSERT(Contains(p_node), "VMem List has been corrupted");
			#endif

			NodeT* p_prev = p_node->mp_Prev;
			NodeT* p_next = p_node->mp_Next;

			p_prev->mp_Next = p_next;
			p_next->mp_Prev = p_prev;

			p_node->mp_Prev = p_node->mp_Next = NULL;

			VMEM_ASSERT_CODE(--m_Count);
		}

		//------------------------------------------------------------------------
		NodeT* RemoveHead()
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
			#endif
			VMEM_ASSERT(!Empty(), "VMem List had been corrupted");

			NodeT* p_node = m_Node.mp_Next;
			Remove(p_node);

			return p_node;
		}

		//------------------------------------------------------------------------
		NodeT* RemoveTail()
		{
			#if VMEM_LIST_FULL_CHECKING
				CheckIntegrity();
			#endif
			VMEM_ASSERT(!Empty(), "VMem List had been corrupted");

			NodeT* p_node = m_Node.mp_Prev;
			Remove(p_node);

			return p_node;
		}

		//------------------------------------------------------------------------
		void CheckIntegrity() const
		{
#ifdef VMEM_ASSERTS
			int count = 0;
			const NodeT* p_iter_end = &m_Node;
			for(const NodeT* p_iter = m_Node.mp_Next; p_iter != p_iter_end; p_iter = p_iter->mp_Next)
			{
				++count;
				VMEM_ASSERT(count <= m_Count, "VMem list has been corrupted");
			}

			VMEM_ASSERT(count == m_Count, "VMem list has been corrupted");
#endif
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ASSERTS
		int GetCount() const
		{
			return m_Count;
		}
#endif

	private:
		//------------------------------------------------------------------------
#if VMEM_LIST_FULL_CHECKING
		bool Contains(NodeT* p_node) const
		{
			const NodeT* p_iter_end = &m_Node;
			for(const NodeT* p_iter = m_Node.mp_Next; p_iter != p_iter_end; p_iter = p_iter->mp_Next)
			{
				if(p_iter == p_node)
					return true;
			}

			return false;
		}
#endif

		//------------------------------------------------------------------------
		// data
	private:
		NodeT m_Node;

		#ifdef VMEM_ASSERTS
			int m_Count;
		#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemRedBlackTree.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//--------------------------------------------------------------
namespace VMem
{
	//--------------------------------------------------------------
	template<typename T>
	class RedBlackTree
	{
		//--------------------------------------------------------------
		struct MemPool
		{
			MemPool* mp_Next;
			size_t m_Size;
		};

	public:
		//--------------------------------------------------------------
		struct Node
		{
			Node(Node* p_parent)
			:	mp_Parent(p_parent),
				m_Red(false)
			{
				mp_Child[0] = mp_Child[1] = NULL;
			}

			bool IsNull() const { return mp_Child[0] == NULL; }

			bool IsBlack() const { return !m_Red; }

			Node* mp_Parent;
			Node* mp_Child[2];
			T m_Value;
			bool m_Red;
		};

		//--------------------------------------------------------------
		RedBlackTree(InternalAllocator* p_allocator, int page_size)
		:	mp_Root(NULL),
			m_Count(0),
			m_NextMemPoolSize(page_size),
			mp_MemPools(NULL),
			mp_FreeNodeSlots(NULL),
			mp_Allocator(p_allocator)
		{
		}

		//--------------------------------------------------------------
		~RedBlackTree()
		{
			DeleteMemPools();
		}

		//--------------------------------------------------------------
		void Clear()
		{
			mp_Root = NULL;
			m_Count = 0;

			mp_FreeNodeSlots = NULL;
			DeleteMemPools();

			#ifdef VMEM_ENABLE_STATS
				m_Stats = Stats();
			#endif
		}

		//--------------------------------------------------------------
		bool IsFull() const
		{
			// always need at least 2 free nodes for an add
			return !mp_FreeNodeSlots || !*(void**)mp_FreeNodeSlots;
		}

		//--------------------------------------------------------------
		bool Insert(T value)
		{
			Node* p_insert_node = FindInsertNode(value);
			if(!p_insert_node)
				return false;		// oom

			if(!p_insert_node->IsNull() && p_insert_node->m_Value == value)
			{
				p_insert_node->m_Value = value;
			}
			else
			{
				Node* p_new_child1 = AllocNode(p_insert_node);
				if (!p_new_child1)
					return false;

				Node* p_new_child2 = AllocNode(p_insert_node);
				if (!p_new_child2)
				{
					FreeNode(p_new_child1);
					return false;
				}

				p_insert_node->m_Value = value;
				p_insert_node->mp_Child[0] = p_new_child1;
				p_insert_node->mp_Child[1] = p_new_child2;

				if(p_insert_node != mp_Root)
				{
					p_insert_node->m_Red = true;
					FixupTreeAfterInsert(p_insert_node);
				}

				++m_Count;
			}

			return true;
		}

		//--------------------------------------------------------------
		void Remove(T value)
		{
			Node* p_node = Find(value);
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			Remove(p_node);
		}

		//--------------------------------------------------------------
		void Remove(Node* p_node)
		{
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			bool child1_is_null = p_node->mp_Child[0]->IsNull();
			bool child2_is_null = p_node->mp_Child[1]->IsNull();

			// case where deleting the only node in the tree
			if(p_node == mp_Root && child1_is_null && child2_is_null)
			{
				mp_Root = p_node->mp_Child[0];
				mp_Root->mp_Parent = NULL;
				FreeNode(p_node->mp_Child[1]);
				FreeNode(p_node);
			}
			else
			{
				// case 1: convert to deleting a node with 0 or 1 non-null children
				if(!child1_is_null && !child2_is_null)
				{
					// find next
					Node* p_next = p_node->mp_Child[1];
					while(!p_next->mp_Child[0]->IsNull())
						p_next = p_next->mp_Child[0];

					// swap with next
					p_node->m_Value = p_next->m_Value;
					p_node = p_next;

					child1_is_null = p_node->mp_Child[0]->IsNull();
					child2_is_null = p_node->mp_Child[1]->IsNull();
				}

				// case 2: node is red so simply delete
				if(p_node->m_Red)
				{
					DeleteLeaf(p_node);
				}
				else
				{
					// case 3: black node with one red child. Swap with child and colour black
					if(!child1_is_null && child2_is_null && p_node->mp_Child[0]->m_Red)
					{
						p_node->m_Value = p_node->mp_Child[0]->m_Value;
						p_node->m_Red = false;
						DeleteLeaf(p_node->mp_Child[0]);
					}
					else if(!child2_is_null && child1_is_null && p_node->mp_Child[1]->m_Red)
					{
						p_node->m_Value = p_node->mp_Child[1]->m_Value;
						p_node->m_Red = false;
						DeleteLeaf(p_node->mp_Child[1]);
					}
					else
					{
						// delete the node and point to the new null node
						Node* p_new_null_node = p_node->mp_Child[0];
						DeleteLeaf(p_node);
						p_node = p_new_null_node;

						FixupDoubleBlackAfterRemove(p_node);
					}
				}
			}

			VMEM_ASSERT(m_Count > 0, "RedBlackTree has been corrupted");
			--m_Count;
		}

		//--------------------------------------------------------------
		Node* GetRoot()
		{
			return mp_Root;
		}

		//--------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			int count = ToInt(size / sizeof(Node) - 1);
			VMEM_ASSERT(count, "bad size passed to GiveMemory");
			p = VMem::AlignUp(p, sizeof(Node));

			void* p_first_slot = p;
			void* p_slot = p_first_slot;
			for(int i=0; i<count - 1; ++i)
			{
				void* p_next = (byte*)p_slot + sizeof(Node);
				*(void**)p_slot = p_next;
				p_slot = p_next;
			}
			*(void**)p_slot = mp_FreeNodeSlots;

			mp_FreeNodeSlots = p_first_slot;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_Stats;
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const
		{
			size_t size = 0;

			for(MemPool* p_mem_pool = mp_MemPools; p_mem_pool != NULL; p_mem_pool = p_mem_pool->mp_Next)
				size += p_mem_pool->m_Size;

			return size;
		}
#endif

	private:
		//--------------------------------------------------------------
		static bool BothChildrenBlack(Node* p_node)
		{
			return p_node->mp_Child[0]->IsBlack() && p_node->mp_Child[1]->IsBlack();
		}

		//--------------------------------------------------------------
		void FixupTreeAfterInsert(Node* p_node)
		{
			Node* p_parent = p_node->mp_Parent;
			if(p_parent->IsBlack())
				return;

			Node* p_grandparent = p_parent->mp_Parent;

			// case 1: re-colour
			Node* p_uncle = p_parent == p_grandparent->mp_Child[0] ? p_grandparent->mp_Child[1] : p_grandparent->mp_Child[0];
			while (p_parent->m_Red && p_uncle->m_Red)
			{
				p_parent->m_Red = false;
				p_uncle->m_Red = false;
				p_grandparent->m_Red = true;
				p_node = p_grandparent;
				p_parent = p_node->mp_Parent;

				if (!p_parent)
				{
					p_node->m_Red = false;
					return;
				}

				if(!p_parent->m_Red)
					return;

				p_grandparent = p_parent->mp_Parent;
				p_uncle = p_parent == p_grandparent->mp_Child[0] ? p_grandparent->mp_Child[1] : p_grandparent->mp_Child[0];
			}

			// parent to the left of grandparent
			if(p_parent == p_grandparent->mp_Child[0])
			{
				if(p_node == p_parent->mp_Child[1])
				{
					// case 2: rotate left
					RotateLeft(p_parent);
					p_node = p_parent;
					p_parent = p_node->mp_Parent;
				}

				// case 3: re-colour and rotate right
				p_parent->m_Red = false;
				p_grandparent->m_Red = true;
				RotateRight(p_grandparent);
			}
			// parent to the right of grandparent
			else
			{
				if (p_node == p_parent->mp_Child[0])
				{
					// case 2: rotate right
					RotateRight(p_parent);
					p_node = p_parent;
					p_parent = p_node->mp_Parent;
				}

				// case 3: re-colour and rotate left
				p_parent->m_Red = false;
				p_grandparent->m_Red = true;
				RotateLeft(p_grandparent);
			}
		}

		//--------------------------------------------------------------
		void FixupDoubleBlackAfterRemove(Node* p_node)
		{
			for (;;)
			{
				// case 1: double black root node
				Node* p_parent = p_node->mp_Parent;
				if (!p_parent)
					break;

				bool red_parent = p_parent->m_Red;

				// case 2
				int sibling_index = p_node == p_parent->mp_Child[0] ? 1 : 0;
				Node* p_sibling = p_parent->mp_Child[sibling_index];
				bool sibling_red = p_sibling->m_Red;
				bool sibling_children_black = BothChildrenBlack(p_sibling);

				if (!red_parent && sibling_red && sibling_children_black)
				{
					p_parent->m_Red = true;
					p_sibling->m_Red = false;

					if (sibling_index)		// if sibling to the right
						RotateLeft(p_parent);
					else
						RotateRight(p_parent);

					continue;
				}

				// case 3
				if (!red_parent && !sibling_red && sibling_children_black)
				{
					p_sibling->m_Red = true;
					p_node = p_parent;
					continue;	// p_sibling has changed, so go back to the start and get the parent and sibling again
				}

				// case 4
				if (red_parent && !sibling_red && sibling_children_black)
				{
					p_parent->m_Red = false;
					p_sibling->m_Red = true;
					break;
				}
				else
				{
					// case 5
					Node* p_sibling_child = p_sibling->mp_Child[sibling_index];
					Node* p_other_sibling_child = p_sibling->mp_Child[1 - sibling_index];
					bool sibling_child_red = p_sibling_child->m_Red;

					if (!sibling_red && !sibling_child_red && p_other_sibling_child->m_Red)
					{
						p_other_sibling_child->m_Red = false;
						p_sibling->m_Red = true;

						if (sibling_index)		// if sibling to the right
							RotateRight(p_sibling);
						else
							RotateLeft(p_sibling);

						p_sibling_child = p_sibling;
						p_sibling = p_other_sibling_child;

						goto case6;
					}

					// case 6
					if (!sibling_red && sibling_child_red)
					{
					case6:
						p_sibling->m_Red = red_parent;
						p_parent->m_Red = false;
						p_sibling_child->m_Red = false;

						if (sibling_index)		// if sibling to the right
							RotateLeft(p_parent);
						else
							RotateRight(p_parent);

						break;
					}
				}
			}
		}

		//--------------------------------------------------------------
		void DeleteLeaf(Node* p_node)
		{
			Node* p_null_node = p_node->mp_Child[0];
			Node* p_parent = p_node->mp_Parent;

			if (p_parent->mp_Child[0] == p_node)
				p_parent->mp_Child[0] = p_null_node;
			else
				p_parent->mp_Child[1] = p_null_node;
			p_null_node->mp_Parent = p_parent;

			FreeNode(p_node->mp_Child[1]);
			FreeNode(p_node);
		}

		//--------------------------------------------------------------
		void RotateLeft(Node* p_node)
		{
			Node* y = p_node;
			Node* z = y->mp_Parent;
			Node* x = y->mp_Child[1];
			Node* b = x->mp_Child[0];

			if(z)
			{
				if(z->mp_Child[0] == y)
					z->mp_Child[0] = x;
				else
					z->mp_Child[1] = x;
			}
			else
			{
				mp_Root = x;
			}

			x->mp_Parent = z;

			x->mp_Child[0] = y;
			y->mp_Parent = x;

			y->mp_Child[1] = b;
			b->mp_Parent = y;
		}

		//--------------------------------------------------------------
		void RotateRight(Node* p_node)
		{
			Node* y = p_node;
			Node* z = y->mp_Parent;
			Node* x = y->mp_Child[0];
			Node* b = x->mp_Child[1];

			if(z)
			{
				if(z->mp_Child[0] == y)
					z->mp_Child[0] = x;
				else
					z->mp_Child[1] = x;
			}
			else
			{
				mp_Root = x;
			}

			x->mp_Parent = z;

			x->mp_Child[1] = y;
			y->mp_Parent = x;

			y->mp_Child[0] = b;
			b->mp_Parent = y;
		}

		//--------------------------------------------------------------
		Node* FindInsertNode(T value)
		{
			Node* p_node = mp_Root;
			
			if(p_node)
			{
				while (!p_node->IsNull())
				{
					if(p_node->m_Value < value)
						p_node = p_node->mp_Child[1];
					else if(p_node->m_Value == value)
						break;
					else
						p_node = p_node->mp_Child[0];
				}
			}
			else
			{
				p_node = AllocNode(NULL);
				if(!p_node)
					return NULL;

				mp_Root = p_node;
			}

			return p_node;
		}

		//--------------------------------------------------------------
		// assert if value not found
		Node* Find(T value)
		{
			Node* p_node = mp_Root;
			VMEM_ASSERT(p_node, "RedBlackTree has been corrupted");

			while (!p_node->IsNull())
			{
				if (p_node->m_Value < value)
					p_node = p_node->mp_Child[1];
				else if (p_node->m_Value == value)
					break;
				else
					p_node = p_node->mp_Child[0];
			}

			VMEM_ASSERT(!p_node->IsNull(), "RedBlackTree has been corrupted");
			return p_node;
		}

		//--------------------------------------------------------------
		static size_t AlignUp(size_t size, size_t alignment)
		{
			return ((size + alignment - 1) / alignment) * alignment;
		}

		//--------------------------------------------------------------
		Node* AllocNode(Node* p_parent)
		{
			void* p_slot = mp_FreeNodeSlots;
			if(!p_slot)
			{
				if(!AllocateNewMemPool())
					return NULL;

				p_slot = mp_FreeNodeSlots;
				VMEM_ASSERT(p_slot, "mp_FreeNodeSlots has been corrupted");
			}

			mp_FreeNodeSlots = *(void**)p_slot;

			Node* p_node = (Node*)p_slot;

			new (p_node)Node(p_parent);

			return p_node;
		}

		//--------------------------------------------------------------
		void FreeNode(Node* p_node)
		{
			p_node->~Node();

			*(void**)p_node = mp_FreeNodeSlots;
			mp_FreeNodeSlots = p_node;
		}

		//--------------------------------------------------------------
		bool AllocateNewMemPool()
		{
			if(!mp_Allocator)
				return false;

			VMEM_ASSERT(m_NextMemPoolSize, "m_NextMemPoolSize has been corrupted");
			size_t pool_size = m_NextMemPoolSize;

			void* p = mp_Allocator->Alloc(pool_size);
			if (!p)
				return false;

			MemPool* p_mem_pool = (MemPool*)p;

			VMEM_STATS(m_Stats.m_Reserved += pool_size);
			VMEM_STATS(m_Stats.m_Overhead += pool_size);

			p_mem_pool->mp_Next = mp_MemPools;
			p_mem_pool->m_Size = pool_size;
			mp_MemPools = p_mem_pool;

			void* p_first_slot = (byte*)p_mem_pool + AlignUp(sizeof(MemPool), sizeof(Node));

			size_t slot_count = (pool_size - ((byte*)p_first_slot - (byte*)p_mem_pool)) / sizeof(Node);
			VMEM_ASSERT(slot_count > 0, "RedBlackTree has been corrupted");

			void* p_end_slot = (byte*)p_first_slot + (slot_count - 1) * sizeof(Node);

			void* p_slot = p_first_slot;
			while(p_slot != p_end_slot)
			{
				void* p_next_slot = (byte*)p_slot + sizeof(Node);
				*(void**)p_slot = p_next_slot;
				p_slot = p_next_slot;
			}
			*(void**)p_slot = NULL;

			VMEM_ASSERT(!mp_FreeNodeSlots, "RedBlackTree has been corrupted");
			mp_FreeNodeSlots = p_first_slot;

			m_NextMemPoolSize *= 2;

			return true;
		}

		//--------------------------------------------------------------
		void FreeMemPool(MemPool* p_mem_pool)
		{
			VMEM_STATS(m_Stats.m_Reserved -= p_mem_pool->m_Size);
			VMEM_STATS(m_Stats.m_Overhead -= p_mem_pool->m_Size);

			mp_Allocator->Free(p_mem_pool, p_mem_pool->m_Size);
		}

		//--------------------------------------------------------------
		void DeleteMemPools()
		{
			MemPool* p_mem_pool = mp_MemPools;
			while(p_mem_pool)
			{
				MemPool* p_next = p_mem_pool->mp_Next;
				FreeMemPool(p_mem_pool);
				p_mem_pool = p_next;
			}

			mp_MemPools = NULL;
		}

		//--------------------------------------------------------------
		RedBlackTree(const RedBlackTree<T>&);
		RedBlackTree& operator=(const RedBlackTree<T>&);

		//--------------------------------------------------------------
		// data
	private:
		Node* mp_Root;
		int m_Count;

		size_t m_NextMemPoolSize;
		MemPool* mp_MemPools;
		void* mp_FreeNodeSlots;

		InternalAllocator* mp_Allocator;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//--------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// RangeMap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//--------------------------------------------------------------
namespace VMem
{
	//--------------------------------------------------------------
	template<class TRangeValue>
	class RangeMap
	{
	public:
		//--------------------------------------------------------------
		struct Range
		{
			Range()
			:	mp_Start(NULL),
				mp_End(NULL),
				m_Value(TRangeValue())
			{
			}

			Range(void* p_start, void* p_end, TRangeValue value)
			:	mp_Start(p_start),
				mp_End(p_end),
				m_Value(value)
			{
			}

			bool operator<(const Range& other) const
			{
				return mp_Start < other.mp_Start;
			}

			bool operator==(const Range& other) const
			{
				VMEM_ASSERT(!(mp_Start == other.mp_Start && mp_End != other.mp_End), "RangeMap range has been corrupted");
				return mp_Start == other.mp_Start;
			}

			void* mp_Start;
			void* mp_End;		// end is exclusive (size = mp_End - mp_Start)
			TRangeValue m_Value;
		};

		//--------------------------------------------------------------
		RangeMap(InternalAllocator* p_allocator = NULL, int page_size = 0)
		:	m_RedBlackTree(p_allocator, page_size)
		{
		}

		//--------------------------------------------------------------
		void Clear()
		{
			m_RedBlackTree.Clear();
		}

		//--------------------------------------------------------------
		bool IsFull() const
		{
			return m_RedBlackTree.IsFull();
		}

		//--------------------------------------------------------------
		bool Add(const Range& range)
		{
			VMEM_ASSERT(range.mp_End > range.mp_Start, "bad range passed in to RangeMap::Add");

			Range existing_range;
			VMEM_ASSERT(!TryGetRange(range.mp_Start, existing_range) && !TryGetRange((byte*)range.mp_End - 1, existing_range), "RangeMap has been corrupted. Adding overlapping range");
			VMEM_UNREFERENCED_PARAM(existing_range);

			return m_RedBlackTree.Insert(range);
		}

		//--------------------------------------------------------------
		void Remove(const Range& range)
		{
			m_RedBlackTree.Remove(range);
		}

		//--------------------------------------------------------------
		bool TryGetRange(void* p, Range& range)
		{
			const typename RedBlackTree<Range>::Node* p_node = m_RedBlackTree.GetRoot();

			if (!p_node)
				return false;

			while (!p_node->IsNull())
			{
				Range node_range = p_node->m_Value;

				if (p < node_range.mp_Start)
				{
					p_node = p_node->mp_Child[0];
				}
				else if (p < (byte*)node_range.mp_End)
				{
					range = node_range;
					return true;
				}
				else
				{
					p_node = p_node->mp_Child[1];
				}
			}

			return false;
		}

		//--------------------------------------------------------------
		bool TryRemoveRange(void* p, size_t size, TRangeValue& value)
		{
			void* p_start = p;
			void* p_end = (byte*)p + size;

			typename RedBlackTree<Range>::Node* p_node = m_RedBlackTree.GetRoot();

			if (!p_node)
				return false;

			typename RedBlackTree<Range>::Node* p_overlapping_node = NULL;

			while (!p_node->IsNull())
			{
				Range node_range = p_node->m_Value;

				if (p_start < node_range.mp_Start)
				{
					if (p_end > node_range.mp_Start)
						p_overlapping_node = p_node;

					p_node = p_node->mp_Child[0];
				}
				else if (p_start < (byte*)node_range.mp_End)
				{
					value = node_range.m_Value;
					m_RedBlackTree.Remove(p_node);
					return true;
				}
				else
				{
					p_node = p_node->mp_Child[1];
				}
			}

			if(p_overlapping_node)
			{
				value = p_overlapping_node->m_Value.m_Value;
				m_RedBlackTree.Remove(p_overlapping_node);
				return true;
			}

			return false;
		}

		//--------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			m_RedBlackTree.GiveMemory(p, size);
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const
		{
			return m_RedBlackTree.GetMemorySize();
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		const Stats& GetStats() const
		{
			return m_RedBlackTree.GetStats();
		}
#endif

	private:
		//--------------------------------------------------------------
		RangeMap(const RangeMap&);
		RangeMap& operator=(const RangeMap&);

		//--------------------------------------------------------------
		// data
	private:
		RedBlackTree<Range> m_RedBlackTree;
	};
}

//--------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VirtualMem.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct Reservation
	{
		size_t m_Size;
		size_t m_AlignedSize;
		void* mp_BaseAddr;
		int m_Align;
		int m_ReserveFlags;
	};

	//------------------------------------------------------------------------
	struct DecommitNode
	{
		DecommitNode* mp_Prev;
		DecommitNode* mp_Next;

		void* mp_Mem;
		size_t m_Size;

		int m_CommitFlags;

		unsigned int m_FlushFrame;
	};

	//------------------------------------------------------------------------
	struct ReleaseNode
	{
		size_t m_Size;
		size_t m_AlignedSize;
		void* mp_Mem;
		ReleaseNode* mp_Prev;
		ReleaseNode* mp_Next;
		int m_Align;
		int m_ReserveFlags;
		unsigned int m_FlushFrame;
	};

	//------------------------------------------------------------------------
	class RangeMapAllocator : public InternalAllocator
	{
	public:
		RangeMapAllocator(int page_size)
		:	m_PageSize(page_size)
		{
		}

		void* Alloc(size_t size)
		{
			void* p = VMem::VirtualReserve(size, m_PageSize, VMEM_DEFAULT_RESERVE_FLAGS);

			if (p)
			{
				if (!VMem::VirtualCommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS))
				{
					VMem::VirtualRelease(p, size);
					p = NULL;
				}
			}

			return p;
		}

		void Free(void* p, size_t size)
		{
			VMem::VirtualDecommit(p, size, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
			VMem::VirtualRelease(p, size);
		}

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;
	};

	//------------------------------------------------------------------------
	class VirtualMem
	{
		typedef RangeMap<struct DecommitNode*> DecommitNodeMap;

	public:
		VirtualMem(int page_size);

		~VirtualMem();

		void Flush();

		void Update();

		int GetPageSize() const { return m_PageSize; }

		void* Reserve(size_t size, int align, int reserve_flags);

		void Release(void* p);

		bool Commit(void* p, size_t size, int commit_flags);

		void Decommit(void* p, size_t size, int commit_flags);

#ifdef VMEM_ENABLE_STATS
		Stats GetStats();
		Stats GetStatsNoLock() const;
#endif
		void Lock() const;

		void Release() const;

		void CheckIntegrity();

	private:
#ifdef VMEM_DECOMMIT_CACHE
		VMEM_FORCE_INLINE int GetReleaseNodeBucketIndex(size_t size);

		void DecommitNodesInRange(void* p, size_t size);

		bool TryCommit(void* p_commit, size_t commit_size, int commit_flags);

		void DecommitDecommitNode(DecommitNode* p_node);

		void FlushMemory(bool force);

		void FlushReleaseCache(bool force);

		void FlushDecommitCache(bool force = false);
#endif
		VirtualMem(const VirtualMem&);
		void operator=(const VirtualMem&);

		//------------------------------------------------------------------------
		// data
	private:
		mutable CriticalSection m_CriticalSection;

		int m_PageSize;

		HashMap<AddrKey, Reservation> m_Reservations;

#ifdef VMEM_ENABLE_STATS
		size_t m_ReservedOverhead;
#endif

#ifdef VMEM_DECOMMIT_CACHE
		static const int m_ReleaseNodeBucketCount = 256;

		List<ReleaseNode> m_ReleaseNodeBuckets[m_ReleaseNodeBucketCount];		// nodes are sorted by age (newest at head)

		size_t m_PendingDecommitBytes;
		size_t m_PendingReleaseBytes;

		BasicFSA<ReleaseNode> m_ReleaseNodePool;
		BasicFSA<DecommitNode> m_DecommitNodePool;

		RangeMapAllocator m_RangeMapAllocator;

		DecommitNodeMap m_DecommitNodeMap;

		List<DecommitNode> m_DecommitNodeList;		// sorted newest to oldest

		unsigned int m_CurrentFrame;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemMemProStats.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	namespace MemProStats
	{
		//------------------------------------------------------------------------
		// make everything use uint64 to avoid packing issues and simplify endian byte swapping
		struct Stats
		{
			Stats& operator=(const VMem::Stats& other)
			{
				m_Used = other.m_Used;
				m_Unused = other.m_Unused;
				m_Overhead = other.m_Overhead;
				m_Reserved = other.m_Reserved;
				return *this;
			}

			uint64 m_Used;
			uint64 m_Unused;
			uint64 m_Overhead;
			uint64 m_Reserved;
		};

		//------------------------------------------------------------------------
		struct BasicCoalesceHeapStats
		{
			uint64 m_RegionCount;
			uint64 m_RegionSize;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct PageHeapStats
		{
			uint64 m_PageSize;
			uint64 m_RegionSize;
			uint64 m_RegionCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct FSAHeapStats
		{
			uint64 m_FSACount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct FSAStats
		{
			uint64 m_Size;
			uint64 m_SlotSize;
			uint64 m_Alignment;
			uint64 m_FreePageCount;
			uint64 m_FullPageCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct CoalesceHeapStats
		{
			uint64 m_MinSize;
			uint64 m_MaxSize;
			uint64 m_RegionSize;
			uint64 m_RegionCount;
			Stats m_Stats;
		};

		//------------------------------------------------------------------------
		struct LageHeapStats
		{
			uint64 m_AllocCount;
			Stats m_Stats;
		};
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// AlignedLargeHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	AlignedLargeHeap::AlignedLargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem)
	:	m_Allocs(virtual_mem.GetPageSize()),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	AlignedLargeHeap::~AlignedLargeHeap()
	{
		if(m_Allocs.Initialised())
		{
			HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
			while(iter.MoveNext())
			{
				void* p = (void*)iter.GetKey().GetAddr();
				Free(p);
			}
		}
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Initialise()
	{
		return m_Allocs.Initialise();
	}

	//------------------------------------------------------------------------
	void* AlignedLargeHeap::Alloc(size_t size, size_t alignment)
	{
		int page_size = m_VirtualMem.GetPageSize();

		size_t aligned_size = AlignSizeUpPow2(size + sizeof(AlignedHeader) + alignment, page_size);

		WriteLockScope lock(m_ReadWriteLock);

		void* p = m_VirtualMem.Reserve(aligned_size, page_size, m_ReserveFlags);
		if(!p)
		{
			return NULL;
		}

		if(!m_VirtualMem.Commit(p, aligned_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p);
			return NULL;
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* aligned_p = (byte*)AlignUp((byte*)p + sizeof(AlignedHeader) + sizeof(VMemCustomAllocInfo), alignment) - sizeof(VMemCustomAllocInfo);
#else
		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);
#endif
		LargeAlignedAlloc alloc;
		alloc.m_Size = size;
		alloc.m_AlignedSize = aligned_size;
		if(!m_Allocs.Add(aligned_p, alloc))
		{
			m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
			m_VirtualMem.Release(p);
			return NULL;		// Out of memory
		}

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		VMEM_MEMSET(aligned_p, VMEM_ALLOCATED_MEM, size);

		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Unused += aligned_size - size);
		VMEM_STATS(m_Stats.m_Reserved += aligned_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		return aligned_p;
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Free(void* p)
	{
		WriteLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		AddrKey key(p);
		if(!m_Allocs.TryGetValue(key, alloc))
			return false;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* aligned_p = p_header->p;

		m_VirtualMem.Decommit(aligned_p, alloc.m_AlignedSize, m_CommitFlags);
		m_VirtualMem.Release(aligned_p);

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc.m_Size);
		VMEM_STATS(m_Stats.m_Unused -= alloc.m_AlignedSize - alloc.m_Size);
		VMEM_STATS(m_Stats.m_Reserved -= alloc.m_AlignedSize);

		m_Allocs.Remove(key);

		return true;
	}

	//------------------------------------------------------------------------
	size_t AlignedLargeHeap::GetSize(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		if(m_Allocs.TryGetValue(p, alloc))
			return alloc.m_Size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool AlignedLargeHeap::Owns(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		LargeAlignedAlloc alloc;
		return m_Allocs.TryGetValue(p, alloc);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t AlignedLargeHeap::WriteAllocs()
	{
		ReadLockScope lock(m_ReadWriteLock);

		DebugWrite(_T("-------------------------\n"));
		DebugWrite(_T("AlignedLargeHeap\n"));

		int written_allocs = 0;
		int alloc_count = 0;
		size_t allocated_bytes = 0;

		HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p = (void*)iter.GetKey().GetAddr();
			LargeAlignedAlloc alloc = iter.GetValue();

			if(written_allocs != VMEM_WRITE_ALLOCS_MAX)
			{
				WriteAlloc(p, alloc.m_Size);
				++written_allocs;
			}
			++alloc_count;
			allocated_bytes += alloc.m_Size;
		}

		if(written_allocs == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocations\n"), VMEM_WRITE_ALLOCS_MAX);

		DebugWrite(_T("%d bytes allocated across %d allocations\n"), allocated_bytes, alloc_count);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void AlignedLargeHeap::AcquireLockShared() const
	{
		m_ReadWriteLock.EnterShared();
	}

	//------------------------------------------------------------------------
	void AlignedLargeHeap::ReleaseLockShared() const
	{
		m_ReadWriteLock.LeaveShared();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void AlignedLargeHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		ReadLockScope lock(m_ReadWriteLock);

		MemProStats::LageHeapStats stats;

		stats.m_AllocCount = 0;
		HashMap<AddrKey, LargeAlignedAlloc>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
			++stats.m_AllocCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_LargeHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats AlignedLargeHeap::GetStats() const
	{
		ReadLockScope lock(m_ReadWriteLock);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats AlignedLargeHeap::GetStatsNoLock() const
	{
		int map_size = m_Allocs.GetAllocedMemory();

		Stats internal_stats;
		internal_stats.m_Reserved += map_size;
		internal_stats.m_Overhead += map_size;

		return m_Stats + internal_stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// BasicCoalesceHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	// BasicCoalesceHeap is different from the standard Coalesce heap in that it
	// doesn't allocate seperate nodes for the free list and doesn't do any biasing.
	// BasicCoalesceHeap doesn't rely on any other allocators and gets its memory
	// directly from the system.
	// It only has a very small overhead and does a simple linear search for a big
	// enough block of memory for each alloc. BasicCoalesceHeap is only intended for
	// internal use by VMem. All allocations are guaranteed to be aligned to 4 bytes.
	// BasicCoalesceHeap is NOT thread safe.
	class BasicCoalesceHeap
	{
		// the header is located immediately before the allocation or at the start of the free block
		struct Header
		{
#ifdef VMEM_ASSERTS
			unsigned int m_Marker;
#endif
			Header* mp_Prev;		// the prev and next free block of memory.
			Header* mp_Next;		// if the header is allocated prev and next pointers will be null.
			int m_Size;				// the size includes the size of the header
			int m_PrevSize;
#ifdef VMEM_ENABLE_STATS
			int m_RequestedSize;
#endif
		};

		struct Region
		{
			Header m_FreeList;
			Region* mp_Next;
		};

		VMEM_STATIC_ASSERT((sizeof(Header) & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "BasicCoalesceHeap Header not aligned");
		VMEM_STATIC_ASSERT((sizeof(Region) & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "BasicCoalesceHeap Region not aligned");

		//------------------------------------------------------------------------
	public:
		BasicCoalesceHeap(int region_size, VirtualMem& virtual_mem);

		~BasicCoalesceHeap();

		void* Alloc(size_t size);

		void Free(void* p);

		void CheckIntegrity();

		void DebugWrite() const;

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

	private:
		Region* CreateRegion();

		void DestroyRegion(Region* p_region);

		void DebugWrite(Header* p_header) const;

		void ClearRegion(Region* p_region);

		VMEM_FORCE_INLINE void Unlink(Header* p_header);

		BasicCoalesceHeap(const BasicCoalesceHeap&);
		void operator=(const BasicCoalesceHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		Region* mp_RegionList;
		int m_RegionSize;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& BasicCoalesceHeap::GetStats() const
	{
		return m_Stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// BasicCoalesceHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	BasicCoalesceHeap::BasicCoalesceHeap(int region_size, VirtualMem& virtual_mem)
	:	mp_RegionList(NULL),
		m_RegionSize(AlignUpPow2(region_size, virtual_mem.GetPageSize())),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	BasicCoalesceHeap::~BasicCoalesceHeap()
	{
		// ideally the region will be empty when the coalesce heap is destructed
		// but we will clean up properly anyway. We can't just release all the
		// virtual memory otherwise the committed pages count will be wrong and
		// the free nodes won't be deleted. So we go through all allocations and 
		// delete them using the normal method.
		while(mp_RegionList)
			ClearRegion(mp_RegionList);
	}

	//------------------------------------------------------------------------
	BasicCoalesceHeap::Region* BasicCoalesceHeap::CreateRegion()
	{
		int page_size = m_VirtualMem.GetPageSize();

		// commit the region memory
		// the region struct is at the start of the committed memory
		Region* p_region = (Region*)m_VirtualMem.Reserve(m_RegionSize, page_size, VMEM_DEFAULT_RESERVE_FLAGS);
		if(!p_region)
			return NULL;		// out of memory

		if(!m_VirtualMem.Commit(p_region, m_RegionSize, VMEM_DEFAULT_COMMIT_FLAGS))
		{
			m_VirtualMem.Release(p_region);
			return NULL;		// out of memory
		}

		// initialise the region
		p_region->mp_Next = NULL;

		// initialise the free list header
		Header& free_list_head = p_region->m_FreeList;
		memset(&free_list_head, 0, sizeof(Header));

		// setup first header 'allocated' header. This header is never used
		// but makes the coalesce logic a simpler.
		Header* p_first_header = (Header*)(p_region + 1);
		VMEM_ASSERT_CODE(p_first_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_first_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_first_header->mp_Next = NULL;
		p_first_header->m_Size = sizeof(Header);
		VMEM_STATS(p_first_header->m_RequestedSize = 0);
		p_first_header->m_PrevSize = 0;

		// setup free header and link it to the first header
		Header* p_header = p_first_header + 1;
		VMEM_ASSERT_CODE(p_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_header->mp_Prev = &free_list_head;		// circular linked list
		p_header->mp_Next = &free_list_head;
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		p_header->m_Size = total_free_size;			// includes size of the header
		VMEM_STATS(p_header->m_RequestedSize = 0);
		p_header->m_PrevSize = sizeof(Header);
		free_list_head.mp_Prev = p_header;
		free_list_head.mp_Next = p_header;

		// setup last header guard
		Header* p_last_header = (Header*)((byte*)p_header + p_header->m_Size);
		VMEM_ASSERT_CODE(p_last_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
		p_last_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_last_header->mp_Next = NULL;
		p_last_header->m_Size = sizeof(Header);
		VMEM_STATS(p_last_header->m_RequestedSize = 0);
		p_last_header->m_PrevSize = total_free_size;

		// update stats
		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused += m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Region) + 3*sizeof(Header));

		return p_region;
	}

	//------------------------------------------------------------------------
	// WARNING: this deletes p_region, don't use p_region after calling this function.
	// This function calls the standard Free function on each alloc in the region until
	// the region disappears. It's slightly messy, but avoids duplicating the code in
	// the Free function.
	void BasicCoalesceHeap::ClearRegion(Region* p_region)
	{
		// get the first alloc that we can free
		Header* p_header = (Header*)(p_region + 1) + 1;

		// first we work out how many bytes have been allocated so that we
		// know when we are done. This is needed because the last Free call
		// will delete the region and all headers, so we need a simple way
		// of telling when this happens.
		int size = 0;
		Header* p_end_header = (Header*)((byte*)p_region + m_RegionSize) - 1;
		for(Header* p_h = p_header; p_h != p_end_header; p_h=(Header*)((byte*)p_h + p_h->m_Size))
		{
			VMEM_MEM_CHECK(&p_h->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

			if(!p_h->mp_Prev)
				size += p_h->m_Size;
		}

		// now keep deleting the first allocated block in the list until all
		// allocations are freed (which frees the region)
		while(size)
		{
			Header* p_alloc_header = p_header;

			// if p_header is free the next header is garanteed to be allocated
			if(p_alloc_header->mp_Prev)
				p_alloc_header = (Header*)((byte*)p_alloc_header + p_alloc_header->m_Size);
			VMEM_MEM_CHECK(&p_alloc_header->mp_Prev, NULL);

			size -= p_alloc_header->m_Size;
			VMEM_ASSERT_MEM(p_alloc_header->m_Size >= 0, &p_alloc_header->m_Size);

			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_alloc_header + 1;
#ifdef VMEM_COALESCE_GUARDS
			p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
#endif
			Free(p);
		}
	}

	//------------------------------------------------------------------------
	// releases the memory associated with the region. Do not use p_region after this.
	void BasicCoalesceHeap::DestroyRegion(Region* p_region)
	{
		// check it's empty
#ifdef VMEM_ASSERTS
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		Header* p_header = (Header*)(p_region + 1) + 1;
		VMEM_ASSERT_MEM(p_header->m_Size == total_free_size, &p_header->m_Size);		// destroying non-empty region
#endif

		// release memory
		m_VirtualMem.Decommit(p_region, m_RegionSize, VMEM_DEFAULT_COMMIT_FLAGS);
		m_VirtualMem.Release(p_region);

		// update stats
		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused -= m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Region) + 3*sizeof(Header));
	}

	//------------------------------------------------------------------------
	void* BasicCoalesceHeap::Alloc(size_t size)
	{
		size = AlignSizeUpPow2(size, VMEM_INTERNAL_ALIGNMENT);

		int i_size = ToInt(size);

		VMEM_ASSERT((i_size&3) == 0, "internal allocs must be aligned to 4 or stats go wrong");

		// work out the size of the block
		int total_size = i_size + sizeof(Header);
#ifdef VMEM_COALESCE_GUARDS
		total_size += 2*VMEM_COALESCE_GUARD_SIZE;
#endif
		// handle the case where the allocation is too big for this heap, fall back to VirtualAlloc
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		if(total_size > total_free_size)
		{
			int page_size = m_VirtualMem.GetPageSize();
			size_t page_aligned_size = AlignSizeUpPow2(size + sizeof(size_t), page_size);
			
			void* p = m_VirtualMem.Reserve(page_aligned_size, page_size, VMEM_DEFAULT_RESERVE_FLAGS);
			if(!p)
				return NULL;

			if(!m_VirtualMem.Commit(p, page_aligned_size, VMEM_DEFAULT_COMMIT_FLAGS))
			{
				m_VirtualMem.Release(p);
				return NULL;
			}

			*(size_t*)p = size;
			p = (byte*)p + sizeof(size_t);

			VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, i_size);

			VMEM_STATS(m_Stats.m_Reserved += page_aligned_size);
			VMEM_STATS(m_Stats.m_Used += size);
			VMEM_STATS(m_Stats.m_Overhead += page_aligned_size - size);

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "pointer returned by BasicCoalescHeap not aligned correctly");

			return p;
		}

		// get the first region (or create one)
		Region* p_region = mp_RegionList;
		if(!p_region)
		{
			p_region = mp_RegionList = CreateRegion();
			if(!p_region)
				return NULL;
		}

		// linear search through the regions for a big enough free block
		while(p_region)
		{
			Header* p_list_head = &p_region->m_FreeList;

			// linear search through the free block list for a big enough block
			for(Header* p_header=p_list_head->mp_Next; p_header != p_list_head; p_header=p_header->mp_Next)
			{
				VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
				VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);

				// work out what the offcut would be
				int offcut_size = p_header->m_Size - total_size;

				// if we have found an allocation big enough
				if(offcut_size >= 0)
				{
					Header* p_prev_free_header = p_header->mp_Prev;
					Header* p_next_free_header = p_header->mp_Next;

					// ignore offcut if too small
					if(offcut_size < (int)sizeof(Header))
					{
						total_size += offcut_size;
						offcut_size = 0;
					}

					// get the next header
					Header* p_next_header = (Header*)((byte*)p_header + p_header->m_Size);
					VMEM_MEM_CHECK(&p_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

					if(offcut_size)
					{
						// create a new header for the offcut
						Header* p_offcut_header = (Header*)((byte*)p_header + total_size);
						VMEM_ASSERT_CODE(p_offcut_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER);
						p_offcut_header->m_Size = offcut_size;
						VMEM_STATS(p_offcut_header->m_RequestedSize = 0);
						p_offcut_header->m_PrevSize = total_size;

						// link new offcut header into free list
						VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);
						p_prev_free_header->mp_Next = p_offcut_header;
						p_offcut_header->mp_Prev = p_prev_free_header;
						p_offcut_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_offcut_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = offcut_size;

						VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
						VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));
					}
					else
					{
						// no offcut, so remove the original header from the free list
						VMEM_ASSERT_MEM(p_header->mp_Prev, &p_header->mp_Prev);
						p_prev_free_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_prev_free_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = total_size;
					}

					// setup the header for this alloc
					p_header->mp_Prev = NULL;		// NULL prev and next pointers mean header is allocated
					p_header->mp_Next = NULL;
					p_header->m_Size = total_size;
					VMEM_STATS(p_header->m_RequestedSize = i_size);

					void* p = p_header + 1;
					VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, i_size);

					// set guards
#ifdef VMEM_COALESCE_GUARDS
					SetGuards(p, VMEM_COALESCE_GUARD_SIZE);
					SetGuards((byte*)p_header + total_size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE);
					p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
#endif
					// update stats
					VMEM_STATS(m_Stats.m_Unused -= total_size - sizeof(Header));
					VMEM_STATS(m_Stats.m_Used += i_size);
					VMEM_STATS(m_Stats.m_Overhead += total_size - sizeof(Header) - i_size);

					VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT-1)) == 0, "pointer returned by BasicCoalescHeap not aligned correctly");

					return p;
				}
			}

			// region full, try the next one
			if(!p_region->mp_Next)
				p_region->mp_Next = CreateRegion();
			p_region = p_region->mp_Next;
		}

		return NULL;	// out of memory
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::Free(void* p)
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;

		// handle the case where this allocation was too big for this heap and was allocated with VirtualAlloc
		if(!p_region)
		{
			void* p_base_alloc = (byte*)p - sizeof(size_t);
			size_t size = *(size_t*)p_base_alloc;
			int page_size = m_VirtualMem.GetPageSize();
			size_t page_aligned_size = AlignSizeUpPow2(size + sizeof(size_t), page_size);
			m_VirtualMem.Decommit(p_base_alloc, page_aligned_size, VMEM_DEFAULT_COMMIT_FLAGS);
			m_VirtualMem.Release(p_base_alloc);
			VMEM_STATS(m_Stats.m_Reserved -= page_aligned_size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_Stats.m_Overhead -= page_aligned_size - size);
			return;
		}

#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
#endif

		// check the header
		Header* p_header = (Header*)p - 1;
		VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
		VMEM_ASSERT_MEM(!p_header->mp_Prev, &p_header->mp_Prev);		// allocation already freed?
		VMEM_ASSERT_MEM(!p_header->mp_Next, &p_header->mp_Next);		// header already on free list?

		VMEM_ASSERT_MEM(p_header->m_Size, &p_header->m_Size);
		int size = p_header->m_Size;

		// check the guards
#ifdef VMEM_COALESCE_GUARDS
		CheckMemory(p, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p_header + size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
#endif
		// stats
		VMEM_STATS(m_Stats.m_Unused += p_header->m_Size - sizeof(Header));
		VMEM_STATS(m_Stats.m_Used -= p_header->m_RequestedSize);
		VMEM_STATS(m_Stats.m_Overhead -= p_header->m_Size - sizeof(Header) - p_header->m_RequestedSize);

		// clear the memory
		VMEM_MEMSET(p, VMEM_FREED_MEM, size - sizeof(Header));

		// get the prev and next headers
		VMEM_ASSERT_MEM(p_header->m_PrevSize, &p_header->m_PrevSize);
		Header* p_prev_header = (Header*)((byte*)p_header - p_header->m_PrevSize);
		Header* p_next_header = (Header*)((byte*)p_header + size);
		int merged_size = size;

		//----------------------------
		// merge with prev header if it's free (mp_Prev!=NULL means free)
		if(p_prev_header && p_prev_header->mp_Prev)
		{
			VMEM_MEM_CHECK(&p_prev_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

			// merge with prev header
			merged_size += p_prev_header->m_Size;

			p_prev_header->m_Size = merged_size;
			p_next_header->m_PrevSize = merged_size;

			VMEM_MEMSET(p_header, VMEM_FREED_MEM, sizeof(Header));

			p_header = p_prev_header;

			// remove from free list
			Unlink(p_header);

			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
		}

		//----------------------------
		// merge with next header if it's free (mp_Prev!=NULL means free)
		if(p_next_header->mp_Prev)
		{
			VMEM_MEM_CHECK(&p_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

			merged_size += p_next_header->m_Size;

			Header* p_next_next_header = (Header*)((byte*)p_next_header + p_next_header->m_Size);
			if(p_next_next_header != (Header*)((byte*)p_region + m_RegionSize))
			{
				VMEM_MEM_CHECK(&p_next_next_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);
				p_next_next_header->m_PrevSize = merged_size;
			}

			p_header->m_Size = merged_size;

			// remove next header from free list
			Unlink(p_next_header);

			VMEM_MEMSET(p_next_header, VMEM_FREED_MEM, sizeof(Header));

			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
		}

		//----------------------------
		// add header to free list
		Header* p_prev = &p_region->m_FreeList;
		Header* p_next = p_prev->mp_Next;
		p_prev->mp_Next = p_header;
		p_header->mp_Prev = p_prev;
		p_header->mp_Next = p_next;
		p_next->mp_Prev = p_header;

		//----------------------------
		// remove the region if it's empty
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		if(merged_size == total_free_size)
		{
			Region* p_prev_region = NULL;
			for(Region* p_srch_region = mp_RegionList; p_srch_region!=p_region; p_srch_region=p_srch_region->mp_Next)
				p_prev_region = p_srch_region;

			Region* p_next_region = p_region->mp_Next;
			if(p_prev_region)
				p_prev_region->mp_Next = p_next_region;
			else
				mp_RegionList = p_next_region;

			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::Unlink(Header* p_header)
	{
		// prev and next pointers always guaranteed to be valid because of the fixed start and end headers
		Header* p_prev = p_header->mp_Prev;
		Header* p_next = p_header->mp_Next;
		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::CheckIntegrity()
	{
#ifdef VMEM_ASSERTS
		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			int total_size = sizeof(Region);

			//----------------------------
			// check all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			bool last_free = false;
			int prev_size = 0;
			while(p_header != p_last_header)
			{
				// check marker
				VMEM_MEM_CHECK(&p_header->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

				int size = p_header->m_Size;
				total_size += size;

				// check prev size
				VMEM_ASSERT_MEM(p_header->m_PrevSize == prev_size, &p_header->m_PrevSize);
				prev_size = size;

				// check no two consecutive free blocks
				bool free = p_header->mp_Prev != NULL;
				VMEM_ASSERT(!(free && last_free), "two consecutive free blocks, they should have been coalesced");
				last_free = free;

				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			VMEM_ASSERT(total_size == m_RegionSize, "blocks don't add up to region size");

			//----------------------------
			// check free list
			Header* p_head = &p_region->m_FreeList;
			Header* p_prev_header = p_head;
			for(Header* p_iter = p_head->mp_Next; p_iter!=p_head; p_iter=p_iter->mp_Next)
			{
				// check marker
				VMEM_MEM_CHECK(&p_iter->m_Marker, VMEM_COALESCE_ALLOC_MARKER);

				// check prev header pointer
				VMEM_MEM_CHECK(&p_iter->mp_Prev, p_prev_header);
				p_prev_header = p_iter;
			}
		}
#endif
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::DebugWrite(Header* p_header) const
	{
		int size = p_header->m_Size;
		void* p_start = p_header;
		void* p_end = (byte*)p_header + size;

		VMem::DebugWrite(_T("0x%08x - 0x%08x\t%7d\t%s\n"), p_start, p_end, size, p_header->mp_Prev ? _T("free") : _T("alloc"));
	}

	//------------------------------------------------------------------------
	void BasicCoalesceHeap::DebugWrite() const
	{
		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			VMem::DebugWrite(_T("--------------------------------\n"), p_region);
			VMem::DebugWrite(_T("Region 0x%08x\n"), p_region);

			// write all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			while(p_header != p_last_header)
			{
				DebugWrite(p_header);
				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			// write free list
			VMem::DebugWrite(_T("\nFree list\n"));

			Header* p_list_head = &p_region->m_FreeList;
			for(Header* p_iter=p_list_head->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				DebugWrite(p_iter);
			}
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void BasicCoalesceHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		MemProStats::BasicCoalesceHeapStats stats;

		stats.m_RegionCount = 0;
		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_RegionSize = m_RegionSize;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_BasicCoalesceHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// Bitfield.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	// This bitfield is designed to be very fast at finding the lowest clear bit
	// in the array of bits. It uses a hint index that points to the lowest bit
	// that might be clear. See GetFirstClearBit for more details.
	class Bitfield
	{
	public:
		Bitfield(int size, InternalHeap& internal_heap);

		~Bitfield();

		bool Initialise();

		bool Initialised() const { return mp_Bits != NULL; }

		VMEM_FORCE_INLINE bool Get(int index) const;

		VMEM_FORCE_INLINE void Set(int index);

		VMEM_FORCE_INLINE void Clear(int index);

		VMEM_FORCE_INLINE int GetFirstClearBit();

		VMEM_FORCE_INLINE int GetCount() const;

		VMEM_FORCE_INLINE int GetSizeInBytes() const;

	private:
		Bitfield& operator=(const Bitfield& other);

		//------------------------------------------------------------------------
		// data
	private:
		int m_Size;					// size in bits
		unsigned int* mp_Bits;		// the bit array
		int m_FirstClearBitHint;	// always garanteed to be <= the first clear bit index. Usually the exact index.
		int m_Count;				// number of set bits

		InternalHeap& m_InternalHeap;	// heap that is used to allocate the bit array
	};

	//------------------------------------------------------------------------
	bool Bitfield::Get(int index) const
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");
		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;
		return (mp_Bits[u32_index] & (1 << bit_index)) != 0;
	}

	//------------------------------------------------------------------------
	void Bitfield::Set(int index)
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");

		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;

		int bit_mask = 1 << bit_index;
		VMEM_ASSERT_MEM((mp_Bits[u32_index] & bit_mask) == 0, &mp_Bits[u32_index]);		// Bitfield bit already set
		mp_Bits[u32_index] |= bit_mask;

		++m_Count;
	}

	//------------------------------------------------------------------------
	void Bitfield::Clear(int index)
	{
		VMEM_ASSERT(index >= 0 && index < m_Size, "Bitfield index out of range");

		unsigned int u32_index = index / 32;
		unsigned int bit_index = index & 0x1f;

		int bit_mask = 1 << bit_index;
		VMEM_ASSERT_MEM((mp_Bits[u32_index] & bit_mask) != 0, &mp_Bits[u32_index]);		// Bitfield bit already clear
		mp_Bits[u32_index] &= ~bit_mask;

		if(index < m_FirstClearBitHint)
			m_FirstClearBitHint = index;

		VMEM_ASSERT_MEM(m_Count, &m_Count);		// can't claer bit, bitfield is empty
		--m_Count;
	}

	//------------------------------------------------------------------------
	int Bitfield::GetCount() const
	{
		return m_Count;
	}

	//------------------------------------------------------------------------
	int Bitfield::GetSizeInBytes() const
	{
		return AlignUpPow2(m_Size, 32) / 8;
	}

	//------------------------------------------------------------------------
	// NOTE: the returned bit must be set after this call to keep m_FirstClearButHint valid.
	// m_FirstClearButHint is incremented by this function to avoid having a conditional in the
	// Set function.
	int Bitfield::GetFirstClearBit()
	{
		VMEM_ASSERT_MEM(m_FirstClearBitHint >= 0, &m_FirstClearBitHint);
		VMEM_ASSERT_MEM(m_FirstClearBitHint < m_Size, &m_FirstClearBitHint);
		VMEM_ASSERT_MEM(m_Count >= 0 && m_Count < m_Size, &m_Count);

		unsigned int u32_index = m_FirstClearBitHint / 32;
		unsigned int bit_index = m_FirstClearBitHint & 0x1f;
		unsigned int* p_u32 = mp_Bits + u32_index;

		int index = m_FirstClearBitHint;

		// if m_FirstClearBitHint is in use look for the next one
		if((*p_u32 & (1 << bit_index)) != 0)
		{
			// look for the next u32 that is not full
#ifdef VMEM_ASSERTS
			unsigned int u32_count = (m_Size + 31) / 32;
#endif
			while(mp_Bits[u32_index] == 0xffffffff)
			{
				++u32_index;
				VMEM_ASSERT_CODE(VMEM_ASSERT_MEM(u32_index < u32_count, &mp_Bits[u32_index]));
			}

			// find the clear bit in the u32
			unsigned int bits = mp_Bits[u32_index];
			unsigned int bit_mask = 1;
			bit_index = 0;
			while(bits & bit_mask)
			{
				bit_mask <<= 1;
				++bit_index;
			}

			index = u32_index * 32 + bit_index;
		}

		// might be free, but at least always guaranteed to be <= the free index
		m_FirstClearBitHint = index + 1;

		return index;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// RelaxedAtomic.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	#define VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
#endif

//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

//------------------------------------------------------------------------
// std::atomic with relaxed memory ordering should do what we want, but
// on some windows platforms it is still adding in memory barriers! so we
// simply use aligned read/writes which on almost all platforms are atomic.
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
	#ifdef VMEM_OS_WIN
		#include <intrin.h>
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#endif
#else
	#include <atomic>
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class RelaxedAtomic64
	{
		VMEM_STATIC_ASSERT(sizeof(T) == sizeof(int64), "Template argument for RelaxedAtomic64 needs to be 8 bytes in size");

	public:
		RelaxedAtomic64()
		{
			VMEM_ASSERT((((uint64)this) & 7) == 0, "RelaxedAtomic misalignment");
		}

		RelaxedAtomic64(T value)
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		:	m_Value(value)
#endif
		{
#ifndef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value.store(value, std::memory_order_relaxed);
#endif
			VMEM_ASSERT((((uint64)this) & 7) == 0, "RelaxedAtomic misalignment");
		}

		void operator=(T p)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value = p;
#else
			return m_Value.store(p, std::memory_order_relaxed);
#endif
		}

		operator T()
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			return m_Value;
#else
			return m_Value.load(std::memory_order_relaxed);
#endif
		}

		VMEM_FORCE_INLINE bool CompareExchange(T expected, T new_value)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			#ifdef VMEM_OS_WIN
				return _InterlockedCompareExchange64((volatile __int64*)&m_Value, (__int64)new_value, (__int64)expected) == (__int64)expected;
			#elif defined(VMEM_PLATFORM_PS4)
				#error Please contact slynch@puredevsoftware.com for this platform
			#else
				#error		// CAS not implemented for OS
			#endif
#else
			return m_Value.compare_exchange_weak(expected, new_value, std::memory_order_release, std::memory_order_relaxed);
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		T m_Value;
#else
		std::atomic<T> m_Value;
#endif
	};

	//------------------------------------------------------------------------
	template<typename T>
	class RelaxedAtomic32
	{
		VMEM_STATIC_ASSERT(sizeof(T) == sizeof(int), "Template argument for RelaxedAtomic32 needs to be 4 bytes in size");

	public:
		RelaxedAtomic32()
		{
			VMEM_ASSERT((((uint64)this) & 3) == 0, "RelaxedAtomic misalignment");
		}

		RelaxedAtomic32(T value)
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		:	m_Value(value)
#endif
		{
#ifndef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value.store(value, std::memory_order_relaxed);
#endif
			VMEM_ASSERT((((uint64)this) & 3) == 0, "RelaxedAtomic misalignment");
		}

		void operator=(T p)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			m_Value = p;
#else
			return m_Value.store(p, std::memory_order_relaxed);
#endif
		}

		operator T()
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			return m_Value;
#else
			return m_Value.load(std::memory_order_relaxed);
#endif
		}

		VMEM_FORCE_INLINE bool CompareExchange(T expected, T new_value)
		{
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
			#ifdef VMEM_OS_WIN
				return _InterlockedCompareExchange((volatile long*)&m_Value, (long)new_value, (long)expected) == (long)expected;
			#elif defined(VMEM_PLATFORM_PS4)
				#error Please contact slynch@puredevsoftware.com for this platform
			#else
				#error		// CAS not implemented for OS
			#endif
#else
			return m_Value.compare_exchange_weak(expected, new_value, std::memory_order_release, std::memory_order_relaxed);
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_SUPPORTS_ALIGNED_ATOMICS
		T m_Value;		// std::atomic relaxed uses locks on some windows compilers, so we use raw aligned reads/writes
#else
		std::atomic<T> m_Value;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// InternalHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	class InternalHeap
	{
	public:
		InternalHeap(VirtualMem& virtual_mem);

		VMEM_FORCE_INLINE void* Alloc(int size);

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void* Alloc_WithFalseSharingBuffer(int size);

		VMEM_FORCE_INLINE void Free_WithFalseSharingBuffer(void* p);

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		inline void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

		template<typename T>
		T* New()
		{
			T* p = (T*)Alloc(sizeof(T));
			if(p)
				new (p)T();
			return p;
		}

		template<typename T, typename TArg1, typename TArg2>
		T* NewRef2(TArg1 arg1, TArg2& arg2)
		{
			T* p = (T*)Alloc(sizeof(T));
			if(p)
				new (p)T(arg1, arg2);
			return p;
		}

		template<typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
		T* New_WithFalseSharingBufferRef45(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4& arg4, TArg5& arg5, TArg6 arg6)
		{
			T* p = (T*)Alloc_WithFalseSharingBuffer(sizeof(T));
			if (p)
				new (p)T(arg1, arg2, arg3, arg4, arg5, arg6);
			return p;
		}

		template<typename T>
		void Delete(T* p)
		{
			p->~T();
			Free(p);
		}

		template<typename T>
		void Delete_WithFalseSharingBuffer(T* p)
		{
			p->~T();
			Free_WithFalseSharingBuffer((byte*)p);
		}

		static int GetFalseSharingBufferOverhead()
		{
			return 2 * m_FalseSharingBufferSize;
		}

	private:
		InternalHeap(const InternalHeap&);	// not implemented
		InternalHeap& operator=(const InternalHeap&);

#if VMEM_SIMULATE_OOM
		bool SimulateOOM();
#endif
		//------------------------------------------------------------------------
		// data
	private:
		static const int m_FalseSharingBufferSize = 128;

		mutable CriticalSection m_CriticalSection;

		BasicCoalesceHeap m_BasicCoalesceHeap;
	};

	//------------------------------------------------------------------------
	void* InternalHeap::Alloc(int size)
	{
		CriticalSectionScope lock(m_CriticalSection);

#if VMEM_SIMULATE_OOM
		if(SimulateOOM())
			return NULL;
#endif
		return m_BasicCoalesceHeap.Alloc(size);
	}

	//------------------------------------------------------------------------
	void InternalHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);
		m_BasicCoalesceHeap.Free(p);
	}

	//------------------------------------------------------------------------
	void* InternalHeap::Alloc_WithFalseSharingBuffer(int size)
	{
		CriticalSectionScope lock(m_CriticalSection);

#if VMEM_SIMULATE_OOM
		if(SimulateOOM())
			return NULL;
#endif
		void* p = m_BasicCoalesceHeap.Alloc(size + 2*m_FalseSharingBufferSize);
		if(!p)
			return NULL;

#ifdef VMEM_ENABLE_MEMSET
		memset(p, VMEM_GUARD_MEM, m_FalseSharingBufferSize);
		memset((byte*)p + m_FalseSharingBufferSize + size, VMEM_GUARD_MEM, m_FalseSharingBufferSize);
		VMEM_STATIC_ASSERT(m_FalseSharingBufferSize >= sizeof(int), "m_FalseSharingBufferSize must be larger than sizeof(int)");
		*(int*)p = size;
#endif
		return (byte*)p + m_FalseSharingBufferSize;
	}

	//------------------------------------------------------------------------
	void InternalHeap::Free_WithFalseSharingBuffer(void* p)
	{
		void* p_alloc = (byte*)p - m_FalseSharingBufferSize;

#ifdef VMEM_ENABLE_MEMSET
		int size = *(int*)p_alloc;
		CheckMemory((byte*)p_alloc + sizeof(int), m_FalseSharingBufferSize - sizeof(int), VMEM_GUARD_MEM);
		CheckMemory((byte*)p_alloc + m_FalseSharingBufferSize + size, m_FalseSharingBufferSize, VMEM_GUARD_MEM);
#endif
		CriticalSectionScope lock(m_CriticalSection);
		m_BasicCoalesceHeap.Free(p_alloc);
	}

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	const Stats& InternalHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_BasicCoalesceHeap.GetStats();
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	const Stats& InternalHeap::GetStatsNoLock() const
	{
		return m_BasicCoalesceHeap.GetStats();
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	void InternalHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		m_BasicCoalesceHeap.SendStatsToMemPro(send_fn, p_context);
	}
	#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// Bitfield.cpp
//
#include <memory.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	Bitfield::Bitfield(int size, InternalHeap& internal_heap)
	:	m_Size(size),
		mp_Bits(NULL),
		m_FirstClearBitHint(0),
		m_Count(0),
		m_InternalHeap(internal_heap)
	{
		VMEM_ASSERT(size, "size must > 0");
	}

	//------------------------------------------------------------------------
	bool Bitfield::Initialise()
	{
		int size_in_bytes = GetSizeInBytes();

		unsigned int* p_bits = (unsigned int*)m_InternalHeap.Alloc(size_in_bytes);
		if(!p_bits)
			return false;

		mp_Bits = p_bits;
		memset(p_bits, 0, size_in_bytes);

		// if the size is not a multiple of 32 set all trailing bits to 1
		int left_over = m_Size & 31;
		if(left_over)
		{
			int size_in_uints = size_in_bytes / sizeof(unsigned int);
			unsigned int& last_uint = p_bits[size_in_uints - 1];
			for(int i=left_over; i<32; ++i)
				last_uint |= (1 << i);
		}

		return true;
	}

	//------------------------------------------------------------------------
	Bitfield::~Bitfield()
	{
		if(mp_Bits)
			m_InternalHeap.Free(mp_Bits);
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// TrailGuard.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class TrailGuard
	{
	public:
		TrailGuard();

		void Initialise(int size, int check_freq, int alloc_size=-1);

		void* Add(void* p) { return Add(p, m_FixedAllocSize); }

		void* Add(void* p, int size);

		int GetSize() const { return m_Size; }

		void* Shutdown();

		bool Contains(void* p) const;

		void CheckIntegrity() const;

		//------------------------------------------------------------------------
		// data
	private:
		int m_Size;
		int m_CurSize;
		int m_FixedAllocSize;

		void* mp_Head;
		void* mp_Tail;

		int m_FullCheckFreq;
		int m_FullCheckCounter;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemArray.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class VMemArray
	{
	public:
		//------------------------------------------------------------------------
		VMemArray(InternalHeap& internal_heap)
		:	mp_Array(NULL),
			m_Count(0),
			m_Capacity(0),
			m_InternalHeap(internal_heap)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return Grow();
		}

		//------------------------------------------------------------------------
		~VMemArray()
		{
			if(mp_Array)
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Array);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		T& operator[](int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");
			return mp_Array[index];
		}

		//------------------------------------------------------------------------
		const T& operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");
			return mp_Array[index];
		}

		//------------------------------------------------------------------------
		bool Add(const T& value)
		{
			if(m_Count == m_Capacity)
			{
				if(!Grow())
					return false;
			}

			mp_Array[m_Count++] = value;

			return true;
		}

		//------------------------------------------------------------------------
		void RemoveAt(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "VMemArray index out of range");

			--m_Count;

			for(int i=index; i!=m_Count; ++i)
				mp_Array[i] = mp_Array[i+1];
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_Capacity * sizeof(T) + InternalHeap::GetFalseSharingBufferOverhead();
		}

	private:
		//------------------------------------------------------------------------
		bool Grow()
		{
			m_Capacity = m_Capacity ? 2*m_Capacity : m_DefaultCapacity;
			
			T* p_new_array = (T*)m_InternalHeap.Alloc_WithFalseSharingBuffer(m_Capacity * sizeof(T));

			if(!p_new_array)
			{
				m_Capacity = 0;
				return false;
			}

			if(mp_Array)
			{
				memcpy(p_new_array, mp_Array, m_Count * sizeof(T));
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Array);
			}

			mp_Array = p_new_array;

			return true;
		}

		//------------------------------------------------------------------------
		VMemArray<T>(const VMemArray<T>&);
		VMemArray<T>& operator=(const VMemArray<T>&);

		//------------------------------------------------------------------------
		// data
	private:
		static const int m_DefaultCapacity = 32;

		T* mp_Array;
		int m_Count;
		int m_Capacity;

		InternalHeap& m_InternalHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// HeapRegions.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	namespace RegionType
	{
		enum Enum
		{
			PageHeap1 = 0,
			PageHeap2,
			CoalesceHeap1,
			CoalesceHeap2,
			AlignedCoalesceHeap,
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			TLSCoalesceHeap,
#endif
			Invalid,
		};
	};

	//------------------------------------------------------------------------
	class HeapRegions
	{
		struct Region
		{
			void* mp_Mem;
			int m_Size;
			RegionType::Enum m_RegionType;

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				void* mp_Heap;
			#endif
		};

	public:
		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		struct RegionAndHeap
		{
			RegionType::Enum m_RegionType;
			void* mp_Heap;
		};
#endif
		//------------------------------------------------------------------------
		HeapRegions(InternalHeap& internal_heap)
		:	m_Regions(internal_heap)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return m_Regions.Initialise();
		}

		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		VMEM_FORCE_INLINE RegionAndHeap GetRegionAndHeap(void* p) const
		{
			ReadLockScope lock(m_ReadWriteLock);

			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
			{
				const Region& region = m_Regions[i];

				void* p_mem = region.mp_Mem;
				if(p >= p_mem && p < (byte*)p_mem + region.m_Size)
				{
					RegionAndHeap region_and_heap;
					region_and_heap.m_RegionType = region.m_RegionType;
					region_and_heap.mp_Heap = region.mp_Heap;
					return region_and_heap;
				}
			}

			RegionAndHeap region_and_heap;
			region_and_heap.m_RegionType = RegionType::Invalid;
			region_and_heap.mp_Heap = NULL;
			return region_and_heap;
		}
#else
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE RegionType::Enum GetRegion(void* p) const
		{
			ReadLockScope lock(m_ReadWriteLock);

			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
			{
				const Region& region = m_Regions[i];

				void* p_mem = region.mp_Mem;
				if(p >= p_mem && p < (byte*)p_mem + region.m_Size)
					return region.m_RegionType;
			}

			return RegionType::Invalid;
		}
#endif
		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		bool AddRegion(void* p_heap, void* p_mem, int size, RegionType::Enum region_type)
		{
			WriteLockScope lock(m_ReadWriteLock);

			Region region;
			region.mp_Heap = p_heap;
			region.mp_Mem = p_mem;
			region.m_Size = size;
			region.m_RegionType = region_type;

			return m_Regions.Add(region);
		}
#else
		bool AddRegion(void* p_mem, int size, RegionType::Enum region_type)
		{
			WriteLockScope lock(m_ReadWriteLock);

			Region region;
			region.mp_Mem = p_mem;
			region.m_Size = size;
			region.m_RegionType = region_type;

			return m_Regions.Add(region);
		}
#endif
		//------------------------------------------------------------------------
		void RemoveRegion(void* p_mem)
		{
			WriteLockScope lock(m_ReadWriteLock);

			int index = FindRegion(p_mem);
			VMEM_ASSERT(index != -1, "unable to find region");

			m_Regions.RemoveAt(index);
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			ReadLockScope lock(m_ReadWriteLock);
			return m_Regions.GetMemoryUsage();
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage_NoLock() const
		{
			return m_Regions.GetMemoryUsage();
		}

	private:
		//------------------------------------------------------------------------
		int FindRegion(void* p_mem) const
		{
			int count = m_Regions.GetCount();
			for(int i=0; i<count; ++i)
				if(m_Regions[i].mp_Mem == p_mem)
					return i;

			return -1;
		}

		//------------------------------------------------------------------------
		// data
	private:
		VMemArray<Region> m_Regions;

		mutable ReadWriteLock m_ReadWriteLock;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// CoalesceHeapCache.h
//
#ifdef VMEM_COALESCE_HEAP_CACHE
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
#ifdef VMEM_COALESCE_HEAP_CACHE
	#define VMEM_COALESCE_CACHE_ALLOC_MARKER 0xccaaccaa

	//------------------------------------------------------------------------
	class CoalesceHeapCache
	{
		//------------------------------------------------------------------------
		struct CachedAlloc
		{
			CachedAlloc* mp_Prev;
			CachedAlloc* mp_Next;
			int m_Size;
			int m_Frame;
			int m_Marker;
		};

		//------------------------------------------------------------------------
		struct AllocList
		{
			RelaxedAtomic32<int> m_Empty;
			List<CachedAlloc> m_Allocs;
			CriticalSection m_CriticalSection;
			AllocList* mp_Prev;
			AllocList* mp_Next;
		};

	public:
		//------------------------------------------------------------------------
		CoalesceHeapCache(InternalHeap& internal_heap)
		:	m_MinSize(0),
			m_MaxSize(0),
			mp_SizeAllocLists(NULL),
			m_Size(0),
			m_InternalHeap(internal_heap),
			m_SizesCount(0),
			m_Frame(0)
		{
		}

		//------------------------------------------------------------------------
		~CoalesceHeapCache()
		{
			VMEM_ASSERT(m_Size == 0, "allocs still in cache");

			if(mp_SizeAllocLists)
			{
				for(int i=0; i<m_SizesCount; ++i)
					mp_SizeAllocLists[i].~AllocList();

				m_InternalHeap.Free(mp_SizeAllocLists);
			}
		}

		//------------------------------------------------------------------------
		bool Initialise(int min_size, int max_size)
		{
			VMEM_ASSERT(min_size >= sizeof(CachedAlloc), "invalid min size");
			VMEM_ASSERT(max_size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE, "invalid max size");

			m_MinSize = AlignDownPow2(min_size, m_Alignment);
			m_MaxSize = AlignUpPow2(max_size, m_Alignment);
			
			m_SizesCount = (m_MaxSize - m_MinSize) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP + 1;
			
			int sizes_size = m_SizesCount * sizeof(AllocList);
			mp_SizeAllocLists = (AllocList*)m_InternalHeap.Alloc(sizes_size);
			if(!mp_SizeAllocLists)
			{
				m_SizesCount = 0;
				return false;
			}

			memset(mp_SizeAllocLists, 0, sizes_size);
			for(int i=0; i<m_SizesCount; ++i)
			{
				new (mp_SizeAllocLists+i)AllocList();
				mp_SizeAllocLists[i].m_Empty = true;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Add(void* p, int size)
		{
			VMEM_ASSERT(size >= m_MinSize && size <= m_MaxSize, "invalid size");

			int index = (size - m_MinSize) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP;
			AllocList& alloc_list = mp_SizeAllocLists[index];

			VMEM_ASSERT(size >= sizeof(CachedAlloc), "invalid size");
			CachedAlloc* p_cached_alloc = (CachedAlloc*)p;
			VMEM_ASSERT_CODE(p_cached_alloc->mp_Prev = p_cached_alloc->mp_Next = NULL);
			p_cached_alloc->m_Size = size;
			p_cached_alloc->m_Frame = m_Frame;
			p_cached_alloc->m_Marker = VMEM_COALESCE_CACHE_ALLOC_MARKER;

			bool was_empty;
			{
				CriticalSectionScope lock(alloc_list.m_CriticalSection);

				was_empty = alloc_list.m_Allocs.Empty();

				alloc_list.m_Empty = false;
				alloc_list.m_Allocs.AddHead(p_cached_alloc);
			}

			m_Size += size;

			if(was_empty)
				AddToUpdateList(&alloc_list);

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Remove(int size)
		{
			VMEM_ASSERT(size >= m_MinSize && size <= m_MaxSize, "invalid size");

			#ifdef VMEM_X64
				VMEM_ASSERT((size % m_Alignment) == 0, "invalid size");
				VMEM_ASSERT(m_Alignment == VMEM_NATURAL_ALIGNMENT, "m_Alignment has been corrupted");
			#else
				size = AlignSizeUpPow2(size, m_Alignment);
			#endif

			int index = (size - m_MinSize + VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP - 1) / VMEM_COALESCE_HEAP_CACHE_BUCKET_STEP;
			AllocList& alloc_list = mp_SizeAllocLists[index];

			if(alloc_list.m_Empty)
				return NULL;

			CachedAlloc* p = NULL;
			{
				CriticalSectionScope lock(alloc_list.m_CriticalSection);

				if(alloc_list.m_Allocs.Empty())
					alloc_list.m_Empty = true;
				else
					p = alloc_list.m_Allocs.RemoveHead();
			}

			if (p)
			{
				VMEM_ASSERT(p->m_Marker == VMEM_COALESCE_CACHE_ALLOC_MARKER, "CoalesceHeap Cache alloc has been corrupted");
				int actual_size = p->m_Size;
				m_Size -= actual_size;
				VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, actual_size);
			}

			return p;
		}

		//------------------------------------------------------------------------
		void Trim(size_t max_size, void*& p_allocs_to_free)
		{
			CriticalSectionScope lock(m_CriticalSection);

			AllocList* p_alloc_list = m_UpdateList.GetTail();
			AllocList* p_alloc_list_end = m_UpdateList.GetIterEnd();
			
			while(p_alloc_list != p_alloc_list_end)
			{
				CriticalSectionScope list_lock(p_alloc_list->m_CriticalSection);

				AllocList* p_prev_alloc_list = p_alloc_list->mp_Prev;

				List<CachedAlloc>& allocs = p_alloc_list->m_Allocs;

				CachedAlloc* p_iter_end = allocs.GetIterEnd();
				CachedAlloc* p_alloc = allocs.GetTail();

				while(p_alloc != p_iter_end)
				{
					CachedAlloc* p_prev_alloc = p_alloc->mp_Prev;

					if(m_Size > max_size || p_alloc->m_Frame + VMEM_COALESCE_HEAP_CACHE_MAX_AGE < m_Frame)
					{
						size_t size = p_alloc->m_Size;
						m_Size -= size;

						CachedAlloc* p_removed_alloc = allocs.RemoveTail();
						VMEM_ASSERT(p_removed_alloc == p_alloc, "fail");
						VMEM_UNREFERENCED_PARAM(p_removed_alloc);

						VMEM_ASSERT(p_alloc->m_Marker == VMEM_COALESCE_CACHE_ALLOC_MARKER, "CoalesceHeap Cache alloc has been corrupted");

						void* p = p_alloc;
						VMEM_MEMSET(p, VMEM_UNUSED_PAGE, size);

						*(void**)p = p_allocs_to_free;
						p_allocs_to_free = p;
					}
					else
					{
						break;
					}

					p_alloc = p_prev_alloc;
				}

				if(allocs.Empty())
				{
					p_alloc_list->m_Empty = true;
					RemoveFromUpdateList_NoLock(p_alloc_list);
				}

				p_alloc_list = p_prev_alloc_list;
			}
		}

		//------------------------------------------------------------------------
		size_t GetSize() const
		{
			return m_Size;
		}

		//------------------------------------------------------------------------
		void Update()
		{
			++m_Frame;
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		size_t GetMemoryUsage() const
		{
			return m_SizesCount * sizeof(AllocList);
		}
#endif

	private:
		//------------------------------------------------------------------------
		void AddToUpdateList(AllocList* p_allocs_list)
		{
			CriticalSectionScope lock(m_CriticalSection);

			// need to check that someone else didn't add it to the list before we aquired the lock
			if(!p_allocs_list->mp_Prev)
				m_UpdateList.AddTail(p_allocs_list);
		}

		//------------------------------------------------------------------------
		void RemoveFromUpdateList_NoLock(AllocList* p_alloc_list)
		{
			m_UpdateList.Remove(p_alloc_list);
		}

		//------------------------------------------------------------------------
		// data
	private:
#if VMEM_NATURAL_ALIGNMENT > 16
		static const int m_Alignment = VMEM_NATURAL_ALIGNMENT;
#else
		static const int m_Alignment = 16;
#endif
		int m_MinSize;
		int m_MaxSize;

		AllocList* mp_SizeAllocLists;

		std::atomic<size_t> m_Size;

		List<AllocList> m_UpdateList;

		InternalHeap& m_InternalHeap;

		CriticalSection m_CriticalSection;

		int m_SizesCount;

		int m_Frame;
	};
#endif		// #ifdef VMEM_COALESCE_HEAP_CACHE
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// CoalesceHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	namespace Coalesce
	{
		//------------------------------------------------------------------------
		struct Region;
		struct Header;
		struct Node;

		//------------------------------------------------------------------------
		// each free block points to a node. Allocated blocks do not have nodes.
		// Instead of embedding this data in the memory block we store it as a
		// seperate object to allow for fast cache friendly iteration when sorting
		// and looking up nodes.
		struct Node
		{
			void* mp_Mem;
			Node* mp_Prev;
			Node* mp_Next;
			int m_Size;
			VMEM_X64_ONLY(unsigned int m_Padding);
		};

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		//------------------------------------------------------------------------
		struct NodeBucket
		{
			NodeBucket()
			:	mp_PrevNeedsSortBucket(NULL),
				mp_NextNeedsSortBucket(NULL)
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
				,m_NeedsSort(false)
				,m_SortByAddressOnly(false)
#endif
			{
				m_Head.mp_Mem = NULL;
				m_Head.mp_Prev = &m_Head;
				m_Head.mp_Next = &m_Head;
				m_Head.m_Size = 0;
			}

			Node m_Head;
			NodeBucket* mp_PrevNeedsSortBucket;
			NodeBucket* mp_NextNeedsSortBucket;
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			bool m_NeedsSort;
			bool m_SortByAddressOnly;
#endif
		};
#endif
	}

	//------------------------------------------------------------------------
	// This coalesce heap used best fit and immediate coalesceing. It also decommits
	// free pages in and free blocks, and performs biasing to reduce fragmentation.
	// Unlike many coalesce heaps the free nodes are not embedded in the memory blocks
	// but are seperate allocations to increase cache coherency when looking up and sorting.
	// This allocator aligns to VMEM_NATURAL_ALIGNMENT bytes.
	class CoalesceHeap
	{
	public:
		CoalesceHeap(
			int region_size,
			int min_size,
			int max_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			int reserve_flags,
			int commit_flags,
			VirtualMem& virtual_mem,
			bool cache_enabled=false,
			bool add_initial_region_to_heap_regions=true);

		~CoalesceHeap();

		bool Initialise();

		void* Alloc(size_t size);

		void Free(void* p);

		static int GetSize(void* p);

		void CheckIntegrity() const;

		void DebugWrite() const;

#ifdef VMEM_ENABLE_STATS
		inline Stats GetStats() const;
		inline Stats GetStatsNoLock() const;
		size_t WriteAllocs() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

#ifdef VMEM_TRAIL_GUARDS
		inline void InitialiseTrailGuard(int size, int check_freq);
#endif

		void Lock() const { m_CriticalSection.Enter(); }

		void Release() const { m_CriticalSection.Leave(); }

		void Flush();

		void Update();

		void* GetFirstRegion() { return mp_RegionList; }

	private:
		bool FreeInternal(void* p);

		Coalesce::Region* CreateRegion(bool add_to_heap_regions=true);

		void ClearRegion(Coalesce::Region* p_region);

		void DestroyRegion(Coalesce::Region* p_region);

		VMEM_FORCE_INLINE Coalesce::Node* AllocNode();

		VMEM_FORCE_INLINE void FreeNode(Coalesce::Node* p_node);

		Coalesce::Node* GetNode(int size, Coalesce::Region*& p_region);

		VMEM_FORCE_INLINE void InsertNode(Coalesce::Node* p_node, Coalesce::Region* p_region);

		VMEM_FORCE_INLINE bool UnlinkNode(Coalesce::Node* p_node);

		VMEM_FORCE_INLINE void RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region);

		inline int GetFreeListIndex(int size) const;

		Coalesce::Region* GetRegion(void* p) const;

		inline bool CommitRange(void* p, int size);

		inline void DecommitRange(void* p, int size);

		inline Coalesce::Header* FindNextAllocatedHeader(Coalesce::Region* p_region, Coalesce::Header* p_header) const;

		void CheckIntegrity(Coalesce::Header* p_header) const;

		void CheckIntegrity_NoLock() const;

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_FORCE_INLINE int SizeToBucketValue(int size) const;
		void RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region, int bucket_index);
#endif

#ifdef VMEM_COALESCE_GUARD_PAGES
		bool SetupGuardPages(Coalesce::Region* p_region);
		void RemoveGuardPages(Coalesce::Region* p_region);
#endif

#ifdef VMEM_COALESCE_HEAP_CACHE
		void FlushCache(size_t max_mem);
#endif

#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		void SortBuckets();
#endif
		CoalesceHeap(const CoalesceHeap&);
		CoalesceHeap& operator=(const CoalesceHeap&);

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_COALESCE_HEAP_CACHE
		bool m_CacheEnabled;
		CoalesceHeapCache m_Cache;
#endif
		int m_RegionSize;

		VirtualMem& m_VirtualMem;

		// the minimum and maximum size that this coalesce heap can allocate. This is
		// used to skew the freelist map indices, deciding what to do with offcuts and
		// asserting the allocation sizes are in range.
		const int m_MinSize;
		const int m_MaxSize;
		const int m_SizeRange;

		Coalesce::Region* mp_RegionList;		// linked list of regions

		BasicFSA<Coalesce::Node> m_NodeFSA;	// the allocator to use to allocate nodes

		// free can't fail, so every alloc we allocate a "spare" node and add it to this list. If free needs a new node it can always be guaranteed to find one here
		Coalesce::Node* mp_SpareNodeFreeList;

		InternalHeap& m_InternalHeap;

		HeapRegions& m_HeapRegions;
		RegionType::Enum m_RegionType;

#if defined(VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD) && !defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
		Coalesce::NodeBucket m_BucketsToSortList;
#endif
		bool m_AddInitialRegionToHeapRegions;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif

#ifdef VMEM_TRAIL_GUARDS
		TrailGuard m_TrailGuard;
#endif
		mutable CriticalSection m_CriticalSection;

		int m_ReserveFlags;
		int m_CommitFlags;
	};

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats CoalesceHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats CoalesceHeap::GetStatsNoLock() const
	{
		Stats stats = m_Stats + m_NodeFSA.GetStats();
		#ifdef VMEM_COALESCE_HEAP_CACHE
			if(m_CacheEnabled)
				stats.m_Overhead += m_Cache.GetMemoryUsage();
		#endif
		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_TRAIL_GUARDS
	void CoalesceHeap::InitialiseTrailGuard(int size, int check_freq)
	{
		CriticalSectionScope lock(m_CriticalSection);

		m_TrailGuard.Initialise(size, check_freq);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// TieredBitfield.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class TieredBitfield
	{
		//------------------------------------------------------------------------
		typedef unsigned int uint;

		//------------------------------------------------------------------------
		enum
		{
			UIntShift = 5,
			UIntBitCount = 1<<UIntShift,
			UIntMask = UIntBitCount-1,
		};

		//------------------------------------------------------------------------
		class Bitfield
		{
		public:
			Bitfield()
			:	mp_Bits(NULL),
				m_UIntCount(0)
			{
			}

			void Initialise(uint* p_bits, int uint_count)
			{
				mp_Bits = p_bits;
				m_UIntCount = uint_count;

				memset(p_bits, 0, uint_count * sizeof(uint));
			}

			// return true if he uint was clear before we set the bit
			VMEM_FORCE_INLINE bool Set(int index)
			{
				int uint_index = index >> UIntShift;
				int shift = index & UIntMask;
				uint bit = 1 << shift;

				uint old_bits = mp_Bits[uint_index];
				if((old_bits & bit) == 0)
					mp_Bits[uint_index] = old_bits | bit;

				return !old_bits;
			}

			// return true if the entire uint has no set bits after the clear
			VMEM_FORCE_INLINE bool Clear(int index)
			{
				int uint_index = index >> UIntShift;
				int shift = index & UIntMask;
				uint bit = 1 << shift;

				VMEM_ASSERT((mp_Bits[uint_index] & bit) != 0, "bit not set");

				uint new_bits = mp_Bits[uint_index] & ~bit;
				mp_Bits[uint_index] = new_bits;
				return !new_bits;
			}

			VMEM_FORCE_INLINE int FindNextSetBit_InCurrentUInt(int index) const
			{
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint bits = mp_Bits[uint_index];

				if(bits)
				{
					for(int i=index&UIntMask; i<UIntBitCount; ++i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				return -1;
			}

			VMEM_FORCE_INLINE int FindPrevSetBit_InCurrentUInt(int index) const
			{
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint bits = mp_Bits[uint_index];

				if(bits)
				{
					for(int i=index&UIntMask; i>=0; --i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				return -1;
			}

			VMEM_FORCE_INLINE int FindNextSetBit(int index) const
			{
				int uint_index = index >> UIntShift;
				if(uint_index == m_UIntCount)
					return -1;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint* p_bits = mp_Bits + uint_index;
				uint bits = *p_bits;

				if(bits)
				{
					for(int i=index&UIntMask; i<UIntBitCount; ++i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				uint* p_bits_end = mp_Bits + m_UIntCount;
				do
				{
					++p_bits;
					if(p_bits == p_bits_end)
						return -1;
				} while(*p_bits == 0);

				uint bits2 = *p_bits;
				VMEM_ASSERT(bits2, "bits2 is 0");
				int start_index = ToInt(p_bits - mp_Bits) * UIntBitCount;
				for(int i=0; ; ++i)
					if((bits2 & (1<<i)) != 0)
						return start_index + i;
			}

			VMEM_FORCE_INLINE int FindPrevSetBit(int index) const
			{
				if(index == -1)
					return -1;
				int uint_index = index >> UIntShift;
				VMEM_ASSERT(uint_index < m_UIntCount, "index out of range");

				uint* p_bits = mp_Bits + uint_index;
				uint bits = *p_bits;

				if(bits)
				{
					for(int i=index&UIntMask; i>=0; --i)
						if((bits & (1<<i)) != 0)
							return uint_index * UIntBitCount + i;
				}

				uint* p_bits_end = mp_Bits - 1;
				do
				{
					--p_bits;
					if(p_bits == p_bits_end)
						return -1;
				} while(*p_bits == 0);

				uint bits2 = *p_bits;
				VMEM_ASSERT(bits2, "bits2 is 0");
				int start_index = ToInt(p_bits - mp_Bits) * UIntBitCount;
				for(int i=UIntBitCount-1; ; --i)
					if((bits2 & (1<<i)) != 0)
						return start_index + i;
			}

			//------------------------------------------
			// data
		private:
			uint* mp_Bits;
			int m_UIntCount;
		};

	public:
		//------------------------------------------------------------------------
		TieredBitfield(InternalHeap& internal_heap)
		:	m_InternalHeap(internal_heap),
			mp_Bits(NULL),
			m_Count(0),
			m_MemoryUsage(0)
		{
		}

		//------------------------------------------------------------------------
		~TieredBitfield()
		{
			if(mp_Bits)
				m_InternalHeap.Free_WithFalseSharingBuffer(mp_Bits);
		}

		//------------------------------------------------------------------------
		bool Initialise(int count)
		{
			m_Count = count;

			int tier1_uint_count = ((count + UIntBitCount-1) >> UIntShift);
			int tier2_uint_count = ((tier1_uint_count + UIntBitCount-1) >> UIntShift);

			int total_count = tier1_uint_count + tier2_uint_count;
			mp_Bits = (uint*)m_InternalHeap.Alloc_WithFalseSharingBuffer(total_count * sizeof(uint));
			if(!mp_Bits)
			{
				m_Count = 0;
				return false;
			}

			m_MemoryUsage = total_count * sizeof(uint) + InternalHeap::GetFalseSharingBufferOverhead();

			m_Tier1Bits.Initialise(mp_Bits, tier1_uint_count);
			m_Tier2Bits.Initialise(mp_Bits + tier1_uint_count, tier2_uint_count);

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Set(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			if(m_Tier1Bits.Set(index))
				m_Tier2Bits.Set(index >> UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Clear(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			if(m_Tier1Bits.Clear(index))
				m_Tier2Bits.Clear(index >> UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE int FindNextSetBit(int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			int set_index = m_Tier1Bits.FindNextSetBit_InCurrentUInt(index);
			if(set_index != -1)
				return set_index;

			int tier2_index = m_Tier2Bits.FindNextSetBit((index >> UIntShift) + 1);
			if(tier2_index == -1)
				return -1;

			return m_Tier1Bits.FindNextSetBit_InCurrentUInt(tier2_index << UIntShift);
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE int FindPrevSetBit(int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			int set_index = m_Tier1Bits.FindPrevSetBit_InCurrentUInt(index);
			if(set_index != -1)
				return set_index;

			int tier2_index = m_Tier2Bits.FindPrevSetBit((index >> UIntShift) - 1);
			if(tier2_index == -1)
				return -1;

			return m_Tier1Bits.FindPrevSetBit_InCurrentUInt((tier2_index << UIntShift) + UIntBitCount-1);
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_MemoryUsage;
		}

		//------------------------------------------------------------------------
		TieredBitfield(const TieredBitfield&);
		TieredBitfield& operator=(const TieredBitfield&);

		//------------------------------------------------------------------------
		// data
	private:
		Bitfield m_Tier1Bits;
		Bitfield m_Tier2Bits;

		InternalHeap& m_InternalHeap;
		uint* mp_Bits;

		int m_Count;
		int m_MemoryUsage;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// BucketTree.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#define VMEM_BUCKET_TREE_MARKER 0xbbbbbbbb

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename BucketT>
	class BucketTree
	{
	public:
		//------------------------------------------------------------------------
		BucketTree(int value_count, int bucket_size, InternalHeap& internal_heap)
		:	m_Bitfield(internal_heap),
			m_BucketShift(0),
			m_BucketCount(0),
			m_ValueCount(value_count),
			mp_Buckets(NULL),
			m_InternalHeap(internal_heap),
			m_MemoryUsage(0),
			m_LastUsedBucketIndex(-1),
			m_Padding(VMEM_BUCKET_TREE_MARKER)
		{
			VMEM_ASSERT(IsPow2(bucket_size), "bucket size must be a power of 2");
			while((1 << m_BucketShift) != bucket_size)
				++m_BucketShift;

			m_BucketCount = (value_count+bucket_size-1) >> m_BucketShift;
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			if(!m_Bitfield.Initialise(m_BucketCount))
				return false;

			int buckets_size = ToInt(m_BucketCount * sizeof(BucketT));
			mp_Buckets = (BucketT*)m_InternalHeap.Alloc(buckets_size);
			if(!mp_Buckets)
				return false;

			for(int i=0; i<m_BucketCount; ++i)
				new (mp_Buckets+i)BucketT();

			m_MemoryUsage = buckets_size + m_Bitfield.GetMemoryUsage();

			return true;
		}

		//------------------------------------------------------------------------
		~BucketTree()
		{
			if(mp_Buckets)
			{
				for(int i=0; i<m_BucketCount; ++i)
					mp_Buckets[i].~BucketT();

				m_InternalHeap.Free(mp_Buckets);
			}
		}

		//------------------------------------------------------------------------
		int GetBucket(int value)
		{
			VMEM_ASSERT(value >= 0 && value < m_ValueCount, "value of of range");

			int bucket_index = value >> m_BucketShift;
			m_Bitfield.Set(bucket_index);

			if(bucket_index > m_LastUsedBucketIndex)
				m_LastUsedBucketIndex = bucket_index;

			return bucket_index;
		}

		//------------------------------------------------------------------------
		// find the first non-empty bucket big enough to hold value
		int FindExistingBucket(int value)
		{
			VMEM_ASSERT(value >= 0 && value < m_ValueCount, "value of of range");

			int bucket_index = value >> m_BucketShift;
			return m_Bitfield.FindNextSetBit(bucket_index);
		}

		//------------------------------------------------------------------------
		void ReleaseBucket(int bucket_index)
		{
			VMEM_ASSERT(bucket_index >= 0 && bucket_index < m_BucketCount, "bucket_index out of range");
			m_Bitfield.Clear(bucket_index);

			if(bucket_index == m_LastUsedBucketIndex)
				m_LastUsedBucketIndex = m_Bitfield.FindPrevSetBit(bucket_index);
		}

		//------------------------------------------------------------------------
		int GetLastBucketIndex() const
		{
			return m_LastUsedBucketIndex;
		}

		//------------------------------------------------------------------------
		int GetBucketCount() const
		{
			return m_BucketCount;
		}

		//------------------------------------------------------------------------
		BucketT& operator[](int index)
		{
			return mp_Buckets[index];
		}

		//------------------------------------------------------------------------
		const BucketT& operator[](int index) const
		{
			return mp_Buckets[index];
		}

		//------------------------------------------------------------------------
		int GetMemoryUsage() const
		{
			return m_MemoryUsage;
		}

	private:
		//------------------------------------------------------------------------
		static bool IsPow2(unsigned int value)
		{
			return (value & (value-1)) == 0;
		}

		//------------------------------------------------------------------------
		// data
	private:
		TieredBitfield m_Bitfield;

		int m_BucketShift;
		int m_BucketCount;
		int m_ValueCount;
		int m_MemoryUsage;
		int m_LastUsedBucketIndex;
		int m_Padding;

		BucketT* mp_Buckets;

		InternalHeap& m_InternalHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_COALESCE_HEAP_BUCKET_TREE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// CoalesceHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#define VMEM_SKEW_SIZES

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4100)
#endif

//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_MARKER
	#define VMEM_ASSERT_COALESCE_MARKER(p_value) VMEM_MEM_CHECK(p_value, VMEM_COALESCE_ALLOC_MARKER)
#else
	#define VMEM_ASSERT_COALESCE_MARKER(p_value)
#endif

//------------------------------------------------------------------------
//
// The free list map.
// -------------------
// Each free block of memory has a node. These nodes are stored in the free list map.
//
// This is a simple example of a free node map. It has a min and max size of
// 0 and 50, and there are 6 buckets (in reality there are 256 buckets and they
// are skewed).
// There is an array of 6 lists L, each contain nodes in a size range.
// Each list L contains a linked list of nodes n.
// The nodes in each list are sorted by size and then by address, in an ascending order.
//
// to find a node for a specific size, it first works out the first list L that
// could contain such a node. It then iterates down the list to find a node that is
// big enough. If no such node is found it moves onto the next non-empty list
// and uses the first node.
//
//   L0(0-10)  L1(10-20)  L2(20-30)  L3(30-40)  L4(40-50)  L5(>50)
//       |          |          |          |                   |
//       n2         n14        n20        n33                 n55
//       |                     |                              |
//       n6                    n27                            n200
//       |
//       n10
//

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	const int g_FreeNodeMapSize = 255;
	const int g_CoalesceHeapAlignment = VMEM_NATURAL_ALIGNMENT;

	//------------------------------------------------------------------------
	// The node pointer is aliased with this enum, so the enum values must not be
	// valid pointers. The order is important, fragment must come after allocated
	// so that we can do > allocated to test if the memory is free.
	enum ENodeType
	{
		nt_Fixed = 1,			// fixed memory block at start or end of memory that is never freed
		nt_Allocated = 2,		// allocated memory
		nt_Fragment = 3			// offcut that is too small to be re-used and so doesn't need a free node
								// > than nt_Fragment means that it is a pointer to a free node. We are
								// relying on the fact that 1,2,3 are invalid pointers.
	};

	//------------------------------------------------------------------------
	namespace Coalesce
	{
		//------------------------------------------------------------------------
		const size_t g_MaxCheckMemorySize = 128;

		//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		const int g_FreeNodeBucketSize = 32;
		typedef BucketTree<NodeBucket> NodeBucketTree;
#endif

		//------------------------------------------------------------------------
		// A reserved range of memory from which we commit memory for the allocations
		struct Region
		{
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			Region(int value_count, int bucket_size, InternalHeap& internal_heap)
			:	m_BucketTree(value_count, bucket_size, internal_heap),
#else
			Region() :
#endif
				mp_Next(NULL)
			{
				for(int i=0; i<(int)(sizeof(m_Marker)/sizeof(unsigned int)); ++i)
					m_Marker[i] = VMEM_COALESCE_ALLOC_MARKER;
			}

			Region* mp_Next;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			NodeBucketTree m_BucketTree;
			unsigned int m_Marker[1];		// pack to maintain alignment
#else
			int m_LargestFreeNodeSize;
			unsigned int m_Marker[1];
			VMEM_X64_ONLY(unsigned int padding[4]);
			Node mp_FreeNodeLists[g_FreeNodeMapSize];	// essentially a map from alloc size to free list
#endif
		};

		//------------------------------------------------------------------------
		// embedded before the start of the allocation. The size and prev size are
		// needed so that we can coalesce previous and next allocation when deallocating.
		struct Header
		{
#ifdef VMEM_COALESCE_HEAP_MARKER
			unsigned int m_Marker;
			VMEM_X64_ONLY(unsigned int m_Padding[3]);		// unused
#endif

			int m_Size;					// includes the size of the header
			int m_PrevSize;				// size of the previous sequential block in memory

			// pointer to a node or one of the node flag defines
			union
			{
				size_t m_NodeType;		// ENodeType - use size_t to ensure sizeof(m_NodeType) == sizeof(Node*)
				Node* mp_Node;
			};

#if VMEM_NATURAL_ALIGNMENT == 32 && !defined(VMEM_COALESCE_HEAP_MARKER)
			int padding[4];
#endif
		};

		//------------------------------------------------------------------------
		// check alignment
		VMEM_STATIC_ASSERT((sizeof(Header) & (g_CoalesceHeapAlignment-1)) == 0, "Header size not aligned");
		VMEM_STATIC_ASSERT((sizeof(Region) & (g_CoalesceHeapAlignment-1)) == 0, "Region size not aligned");
		VMEM_STATIC_ASSERT((VMEM_COALESCE_GUARD_SIZE & (g_CoalesceHeapAlignment-1)) == 0, "guard size not aligned");

		//------------------------------------------------------------------------
		const int g_RegionOverhead = sizeof(Region) + 2*sizeof(Header);

		//------------------------------------------------------------------------
		inline void SetupHeader(Header* p_header)
		{
#ifdef VMEM_COALESCE_HEAP_MARKER
			p_header->m_Marker = VMEM_COALESCE_ALLOC_MARKER;
			VMEM_X64_ONLY(p_header->m_Padding[0] = p_header->m_Padding[1] = p_header->m_Padding[2] = 0);
#endif
		}

		//------------------------------------------------------------------------
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		void UpdateLargestFreeNodeSize(Region* p_region)
		{
			Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;

			Coalesce::Node* p_list_head = p_free_node_lists + g_FreeNodeMapSize - 1;
			int max_size = 0;
			for (Coalesce::Node* p_node = p_list_head->mp_Next; p_node != p_list_head; p_node = p_node->mp_Next)
			{
				if(p_node->m_Size > max_size)
					max_size = p_node->m_Size;
			}

			if (max_size)
			{
				p_region->m_LargestFreeNodeSize = max_size;
			}
			else
			{
				--p_list_head;
				while(p_list_head >= p_region->mp_FreeNodeLists)
				{
					Coalesce::Node* p_node = p_list_head->mp_Next;
					if (p_node != p_list_head)
					{
						while (p_node->mp_Next != p_list_head)
							p_node = p_node->mp_Next;
						p_region->m_LargestFreeNodeSize = p_node->m_Size;
						return;
					}
					--p_list_head;
				}
				p_region->m_LargestFreeNodeSize = 0;
			}
		}
#endif
		//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		VMEM_FORCE_INLINE void SwapWithNextNode(Node* p_node1)
		{
			Node* p_node2 = p_node1->mp_Next;

			Node* p_prev = p_node1->mp_Prev;
			Node* p_next = p_node2->mp_Next;

			p_prev->mp_Next = p_node2;
			p_node2->mp_Prev = p_prev;
			p_node2->mp_Next = p_node1;
			p_node1->mp_Prev = p_node2;
			p_node1->mp_Next = p_next;
			p_next->mp_Prev = p_node1;
		}
#endif

		//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		void AddNodeBucketToSortList(Coalesce::NodeBucket* p_prev, Coalesce::NodeBucket* p_bucket)
		{
			Coalesce::NodeBucket* p_prev_bucket = p_prev;
			Coalesce::NodeBucket* p_next_bucket = p_prev_bucket->mp_NextNeedsSortBucket;
			p_prev_bucket->mp_NextNeedsSortBucket = p_bucket;
			p_bucket->mp_PrevNeedsSortBucket = p_prev_bucket;
			p_bucket->mp_NextNeedsSortBucket = p_next_bucket;
			p_next_bucket->mp_PrevNeedsSortBucket = p_bucket;
		}

		//------------------------------------------------------------------------
		void RemoveNodeBucketFromSortList(Coalesce::NodeBucket* p_bucket)
		{
			Coalesce::NodeBucket* p_prev = p_bucket->mp_PrevNeedsSortBucket;
			Coalesce::NodeBucket* p_next = p_bucket->mp_NextNeedsSortBucket;

			p_prev->mp_NextNeedsSortBucket = p_next;
			p_next->mp_PrevNeedsSortBucket = p_prev;
			p_bucket->mp_PrevNeedsSortBucket = p_bucket->mp_NextNeedsSortBucket = NULL;
		}
#endif
	}

	//------------------------------------------------------------------------
	// The region size will be reserved, but not necessarily committed.
	// All allocation sizes requested must be in this min/max range.
	CoalesceHeap::CoalesceHeap(
		int region_size,
		int min_size,
		int max_size,
		InternalHeap& internal_heap,
		HeapRegions& heap_regions,
		RegionType::Enum region_type,
		int reserve_flags,
		int commit_flags,
		VirtualMem& virtual_mem,
		bool cache_enabled,
		bool add_initial_region_to_heap_regions)
	:
#ifdef VMEM_COALESCE_HEAP_CACHE
		m_CacheEnabled(cache_enabled),
		m_Cache(internal_heap),
#endif
		m_RegionSize(AlignUpPow2(region_size, virtual_mem.GetPageSize())),
		m_VirtualMem(virtual_mem),
		m_MinSize(AlignDownPow2(min_size, g_CoalesceHeapAlignment)),
		m_MaxSize(AlignUpPow2(max_size, g_CoalesceHeapAlignment)),
		m_SizeRange(m_MaxSize - m_MinSize - 1),
		mp_RegionList(NULL),
		m_NodeFSA(virtual_mem.GetPageSize()),
		mp_SpareNodeFreeList(NULL),
		m_InternalHeap(internal_heap),
		m_HeapRegions(heap_regions),
		m_RegionType(region_type),
		m_AddInitialRegionToHeapRegions(add_initial_region_to_heap_regions),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags)
	{
#ifndef VMEM_COALESCE_HEAP_CACHE
		VMEM_UNREFERENCED_PARAM(cache_enabled);
#endif
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::Initialise()
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled)
		{
			if(!m_Cache.Initialise(m_MinSize, VMin(m_MaxSize, VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE)))
				return false;
		}
#endif

#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
		m_BucketsToSortList.mp_PrevNeedsSortBucket = m_BucketsToSortList.mp_NextNeedsSortBucket = &m_BucketsToSortList;
#endif

		VMEM_ASSERT(m_MaxSize + (int)sizeof(Coalesce::Header) <= m_RegionSize - Coalesce::g_RegionOverhead, "region size not big enough to hold dmax alloc (+overhead)");

		mp_RegionList = CreateRegion(m_AddInitialRegionToHeapRegions);

		return mp_RegionList != NULL;
	}

	//------------------------------------------------------------------------
	CoalesceHeap::~CoalesceHeap()
	{
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_COALESCE_HEAP_CACHE
		FlushCache(0);
#endif

	#ifdef VMEM_TRAIL_GUARDS
		// free the trail allocs
		void* p_trail_alloc = m_TrailGuard.Shutdown();
		while(p_trail_alloc)
		{
			void* p_next = *(void**)p_trail_alloc;
			FreeInternal(p_trail_alloc);
			p_trail_alloc = p_next;
		}
	#endif

		// ideally the region will be empty when the coalesce heap is destructed
		// but we will clean up properly anyway. We can't just release all the
		// virtual memory otherwise the committed pages count will be wrong and
		// the free nodes won't be deleted. So we go through all allocations and
		// delete them using the normal method.
		if(mp_RegionList)
		{
			Coalesce::Region* p_region = mp_RegionList;
			while(p_region)
			{
				Coalesce::Region* p_next_region = p_region->mp_Next;
				ClearRegion(p_region);
				p_region = p_next_region;
			}

			// the initial region is not destroyed automatically, so have to destroy it explicitly
			DestroyRegion(mp_RegionList);
		}

		// free the spare node free list
		Coalesce::Node* p_node = mp_SpareNodeFreeList;
		while(p_node)
		{
			Coalesce::Node* p_next = p_node->mp_Next;
			VMEM_ASSERT_CODE(p_node->mp_Prev = p_node->mp_Next = NULL);
			FreeNode(p_node);
			p_node = p_next;
		}
	}

	//------------------------------------------------------------------------
	void* CoalesceHeap::Alloc(size_t size)
	{
		int i_size = ToInt(size);

		if(i_size < m_MinSize)
			i_size = m_MinSize;

		VMEM_ASSERT(i_size <= m_MaxSize, "size out of range for this coalesce heap");

		// work out the actual size of the memory block
		int aligned_size = AlignUpPow2(i_size, g_CoalesceHeapAlignment);

#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled && size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE)
		{
			void* p_cached_alloc = m_Cache.Remove(aligned_size);
			if(p_cached_alloc)
			{
				VMEM_STATS(CriticalSectionScope lock(m_CriticalSection));
				VMEM_STATS(size_t alloc_size = GetSize(p_cached_alloc));
				VMEM_STATS(m_Stats.m_Unused -= alloc_size);
				VMEM_STATS(m_Stats.m_Used += alloc_size);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
				#endif
				return p_cached_alloc;
			}
		}
#endif

		int total_size = sizeof(Coalesce::Header) + aligned_size;
		#ifdef VMEM_COALESCE_GUARDS
			total_size += 2*VMEM_COALESCE_GUARD_SIZE;
		#endif

		// find a node that will fit this allocation size
		Coalesce::Region* p_region = NULL;
		CriticalSectionScope lock(m_CriticalSection);		// entering critical section here -------------------

		// allocate a spare node so that free can never fail
		Coalesce::Node* p_node_for_free = AllocNode();
		if(!p_node_for_free)
			return NULL;
		p_node_for_free->mp_Next = mp_SpareNodeFreeList;
		mp_SpareNodeFreeList = p_node_for_free;

		Coalesce::Node* VMEM_RESTRICT p_node = GetNode(total_size, p_region);	// this also removes the node
		if(!p_node)
			return NULL;	// this means out of memory

		void* VMEM_RESTRICT p_mem = p_node->mp_Mem;

		// work out the offcut size
		int offcut_size = p_node->m_Size - total_size;
		VMEM_ASSERT(offcut_size >= 0, "node returned from GetNode is not big enough");

		// if the offcut size is too small ignore it and add it on to the alloc
		if(offcut_size < (int)sizeof(Coalesce::Header))
		{
			total_size += offcut_size;
			offcut_size = 0;
		}

		// work out the range to commit
		void* VMEM_RESTRICT p_commit_start = (byte*)p_node->mp_Mem + sizeof(Coalesce::Header);
		int commit_size = total_size - sizeof(Coalesce::Header);

		// setup the header for this allocation
		Coalesce::Header* VMEM_RESTRICT p_header = (Coalesce::Header*)p_mem;
		Coalesce::Header* VMEM_RESTRICT p_next_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
		VMEM_ASSERT_MEM(p_header->m_NodeType > nt_Fragment, &p_header->m_NodeType);		// GetNode returned an allocated node?
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int page_size = m_VirtualMem.GetPageSize();

		// deal with the offcut
		if(offcut_size)
		{
			// get the pointer to the end of the range to commit. The end pointer is exclusive.
			// +sizeof(Coalesce::Header) because we always need to commit the new header for the new offcut
			void* p_commit_end = (byte*)p_commit_start + commit_size + sizeof(Coalesce::Header);

			// Commit the page for the offcut header.
			// If the next block header is in the same page as the offcut then it will
			// already be committed, otherwise we need to commit it here
			void* p_last_page = AlignUpPow2(p_commit_end, page_size);
			VMEM_ASSERT_COALESCE_MARKER(&p_next_header->m_Marker);
			if(p_last_page <= p_next_header)
				p_commit_end = p_last_page;

			// need to commit the memory before writing to the new offcut header
			int offcut_commit_size = ToInt((byte*)p_commit_end - (byte*)p_commit_start);
			if(!CommitRange(p_commit_start, offcut_commit_size))
			{
				InsertNode(p_node, p_region);

				// destroy the region if we just created it
				if (p_region != mp_RegionList)
				{
					int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2 * sizeof(Coalesce::Header);
					if (p_node->m_Size == total_free_size)
						DestroyRegion(p_region);
				}

				return NULL;		// out of memory
			}

			// set the offcut header
			void* VMEM_RESTRICT p_offcut_mem = (byte*)p_mem + total_size;
			Coalesce::Header* VMEM_RESTRICT p_offcut_header = (Coalesce::Header*)p_offcut_mem;
			SetupHeader(p_offcut_header);
			p_offcut_header->m_Size = offcut_size;
			p_offcut_header->m_PrevSize = total_size;

			p_next_header->m_PrevSize = offcut_size;

			if(offcut_size < m_MinSize)
			{
				p_offcut_header->m_NodeType = nt_Fragment;
				FreeNode(p_node);
			}
			else
			{
				// re-use the free node for the offcut
				p_offcut_header->mp_Node = p_node;

				// update the offcut node
				p_node->mp_Mem = p_offcut_mem;
				p_node->m_Size = offcut_size;

				// reinsert the offcut node
				InsertNode(p_node, p_region);
			}

			#ifdef VMEM_ENABLE_MEMSET
				if(offcut_size > ToInt(sizeof(Coalesce::Header)))
				{
					void* p_offcut_memset_mem = (byte*)p_offcut_mem + sizeof(Coalesce::Header);
					size_t memset_size = offcut_size - sizeof(Coalesce::Header);

					// only commit up to the end of the page if the next page is not committed
					void* p_start_page = AlignDownPow2((byte*)p_offcut_memset_mem-1, page_size);
					void* p_end_page = AlignDownPow2((byte*)p_offcut_mem+offcut_size, page_size);
					if((byte*)p_end_page - (byte*)p_start_page > page_size)
						memset_size = (byte*)p_start_page + page_size - (byte*)p_offcut_memset_mem;

					if(memset_size)
						VMEM_MEMSET(p_offcut_memset_mem, VMEM_FREED_MEM, memset_size);
				}
			#endif

			VMEM_STATS(m_Stats.m_Overhead += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused -= sizeof(Coalesce::Header));
		}
		else
		{
			// must try and commit memory before setting anything
			if(!CommitRange(p_commit_start, commit_size))
			{
				InsertNode(p_node, p_region);

				// destroy the region if we just created it
				if (p_region != mp_RegionList)
				{
					int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2 * sizeof(Coalesce::Header);
					if (p_node->m_Size == total_free_size)
						DestroyRegion(p_region);
				}

				return NULL;		// out of memory!
			}

			FreeNode(p_node);

			p_next_header->m_PrevSize = total_size;
		}

		p_header->m_NodeType = nt_Allocated;
		p_header->m_Size = total_size;

		void* p = p_header + 1;
	#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p, total_size-sizeof(Coalesce::Header), VMEM_FREED_MEM);
	#endif
		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, total_size-sizeof(Coalesce::Header));

	#ifdef VMEM_COALESCE_GUARDS
		SetGuards(p, VMEM_COALESCE_GUARD_SIZE);
		SetGuards((byte*)p_header + total_size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE);
		p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
	#endif

	#ifdef VMEM_ENABLE_STATS
		int allocated_size = p_header->m_Size - sizeof(Coalesce::Header);
	#ifdef VMEM_COALESCE_GUARDS
		allocated_size -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif
		VMEM_STATS(m_Stats.m_Used += allocated_size);
		VMEM_STATS(m_Stats.m_Unused -= allocated_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif
	#endif

		return p;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Free(void* p)
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		if(m_CacheEnabled)
		{
			int size = GetSize(p);
			if(size <= VMEM_COALESCE_HEAP_CACHE_MAX_BUCKET_SIZE && m_Cache.Add(p, size))
			{
#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
				#endif
				VMEM_STATS(m_Stats.m_Unused += size);
				VMEM_STATS(m_Stats.m_Used -= size);
				if(m_Cache.GetSize() > VMEM_COALESCE_HEAP_CACHE_SIZE)
					FlushCache(90*VMEM_COALESCE_HEAP_CACHE_SIZE/100);
#else
				if(m_Cache.GetSize() > VMEM_COALESCE_HEAP_CACHE_SIZE)
				{
					CriticalSectionScope lock(m_CriticalSection);
					FlushCache(90*VMEM_COALESCE_HEAP_CACHE_SIZE/100);
				}
#endif
				return;
			}
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		FreeInternal(p);
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::FreeInternal(void* p)
	{
		// find the region that owns this alloc
		Coalesce::Region* p_region = GetRegion(p);
		VMEM_ASSERT(p_region, "CoalesceHeap trying to free allocation that it doesn't own");

#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
#endif
		// get the header for this alloc
		Coalesce::Header* VMEM_RESTRICT p_header = (Coalesce::Header*)p - 1;

		// if this hits it usually means that you are trying to free an
		// allocation that wasn't allocated from VMem (ie. something
		// within a coalesce alloc)
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int size = p_header->m_Size;

#ifdef VMEM_TRAIL_GUARDS
		if(m_TrailGuard.GetSize())
		{
			// push the alloc onto the tail list and pop another alloc off the end
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
				size -= 2*VMEM_COALESCE_GUARD_SIZE;
			#endif
			size -= sizeof(Coalesce::Header);

			// move stats from used to overhead
			#ifdef VMEM_CUSTOM_ALLOC_INFO
				VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
				VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
			#endif
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_Stats.m_Overhead += size);

			// put the alloc onto the trail, and pop another alloc to free
			p = m_TrailGuard.Add(p, size);
			if(!p) return true;					// return here if TrailGuard didn't return an alloc to free

			// get the details of the new alloc to free
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
			#endif
			p_header = (Coalesce::Header*)p - 1;
			size = p_header->m_Size;

			// move stats back from overhead to used
			int stats_size = size - sizeof(Coalesce::Header);
			#ifdef VMEM_COALESCE_GUARDS
				stats_size -= 2*VMEM_COALESCE_GUARD_SIZE;
			#endif
			VMEM_STATS(m_Stats.m_Used += stats_size);
			VMEM_STATS(m_Stats.m_Overhead -= stats_size);
			#ifdef VMEM_CUSTOM_ALLOC_INFO
				VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
				VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
			#endif

			// the region could have changed
			p_region = GetRegion(p);
			VMEM_ASSERT(p_region, "unable to find region");
		}
	#endif

	#ifdef VMEM_COALESCE_GUARDS
		CheckMemory(p, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p_header + size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
	#endif

		VMEM_MEMSET(p, VMEM_FREED_MEM, size-sizeof(Coalesce::Header));

		// get the range of memory we can definitely try and decommit. The end pointer is exclusive.
		void* VMEM_RESTRICT p_decommit_start = p;
		void* p_decommit_end = (byte*)p + size - sizeof(Coalesce::Header);

		// get the prev and next headers
		Coalesce::Header* p_prev_header = (Coalesce::Header*)((byte*)p_header - p_header->m_PrevSize);
		Coalesce::Header* p_next_header = (Coalesce::Header*)((byte*)p_header + size);

		VMEM_ASSERT_COALESCE_MARKER(&p_prev_header->m_Marker);
		VMEM_ASSERT_COALESCE_MARKER(&p_next_header->m_Marker);

		int merged_size = size;

		int page_size = m_VirtualMem.GetPageSize();

		// see if we can merge with the prev block (a valid node pointer means it's free)
		Coalesce::Node* VMEM_RESTRICT p_new_node = NULL;
		size_t prev_node_type = p_prev_header->m_NodeType;
		if(prev_node_type > nt_Allocated)	// if block is free
		{
			CheckIntegrity(p_prev_header);

			merged_size += p_prev_header->m_Size;

			Coalesce::Node* VMEM_RESTRICT p_prev_node = p_prev_header->mp_Node;			// take the node pointer before we memset it

			VMEM_MEMSET(p_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			p_header = p_prev_header;

			p_next_header->m_PrevSize = merged_size;

			// if it's not a fragment update the free node
			if(prev_node_type != nt_Fragment)
			{
				RemoveNode(p_prev_node, p_region);
				p_new_node = p_prev_node;
			}

			// we can decommit the original header if we are merging with the prev block
			p_decommit_start = (byte*)p_decommit_start - sizeof(Coalesce::Header);

			// we can also decommit the current page if it doesn't contain the prev block header
			void* VMEM_RESTRICT p_cur_page = AlignDownPow2(p_decommit_start, page_size);
			if(p_cur_page >= p_header+1)
				p_decommit_start = p_cur_page;

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
		}

		// see if we can merge with the next alloc
		size_t next_node_type = p_next_header->m_NodeType;
		if(next_node_type > nt_Allocated)	// if block is free
		{
			CheckIntegrity(p_next_header);

			merged_size += p_next_header->m_Size;

			Coalesce::Node* p_next_node = p_next_header->mp_Node;			// take the node pointer before we memset it

			VMEM_MEMSET(p_next_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// see if we can re-use the next node
			if(next_node_type != nt_Fragment)
			{
				RemoveNode(p_next_node, p_region);

				if(p_new_node)
				{
					// we already have a node, so free the next node
					FreeNode(p_next_node);
				}
				else
				{
					// don't have a node so re-use the next node
					p_new_node = p_next_node;
					p_new_node->mp_Mem = p_header;
				}
			}

			// update the next header prev size
			Coalesce::Header* VMEM_RESTRICT p_next_next_header = (Coalesce::Header*)((byte*)p_header + merged_size);
			p_next_next_header->m_PrevSize = merged_size;

			// we can decommit the next header
			p_decommit_end = (byte*)p_decommit_end + sizeof(Coalesce::Header);

			// we can also decommit the page that the next header is on if it doesn't contain the next next header
			void* VMEM_RESTRICT p_next_page = AlignUpPow2(p_decommit_end, page_size);
			if(p_next_page <= p_next_next_header)
				p_decommit_end = p_next_page;

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
		}

		p_header->m_Size = merged_size;

		// get the spare node that was allocated by the alloc function
		VMEM_ASSERT_MEM(mp_SpareNodeFreeList, &mp_SpareNodeFreeList);
		VMEM_ASSUME(mp_SpareNodeFreeList);
		Coalesce::Node* p_spare_node = mp_SpareNodeFreeList;
		mp_SpareNodeFreeList = p_spare_node->mp_Next;

		if(!p_new_node)
		{
			// didn't merge, so use spare node
			p_new_node = p_spare_node;
			p_new_node->mp_Mem = p_header;
		}
		else
		{
			// don't need spare node, so free it
			VMEM_ASSERT_CODE(p_spare_node->mp_Prev = p_spare_node->mp_Next = NULL);
			FreeNode(p_spare_node);
		}

		// setup the new node
		p_new_node->m_Size = merged_size;
		p_new_node->mp_Next = NULL;
		p_new_node->mp_Prev = NULL;
		VMEM_X64_ONLY(p_new_node->m_Padding = 0);
		p_header->mp_Node = p_new_node;

		// (re)insert the new free node
		InsertNode(p_new_node, p_region);

	#ifdef VMEM_ENABLE_STATS
		int alloc_size = size - sizeof(Coalesce::Header);
		#ifdef VMEM_COALESCE_GUARDS
			alloc_size -= 2*VMEM_COALESCE_GUARD_SIZE;
		#endif
	#endif
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc_size);
		VMEM_STATS(m_Stats.m_Unused += alloc_size);

		// decommit the range
		int range_size = ToInt((byte*)p_decommit_end - (byte*)p_decommit_start);
		DecommitRange(p_decommit_start, range_size);

		// destroy region if empty (unless it's the initial region
		if(p_region != mp_RegionList)
		{
			int total_free_size = m_RegionSize - sizeof(Coalesce::Region) - 2*sizeof(Coalesce::Header);
			if(merged_size == total_free_size)
				DestroyRegion(p_region);
		}

		return true;
	}

	//------------------------------------------------------------------------
	int CoalesceHeap::GetSize(void* p)
	{
	#ifdef VMEM_COALESCE_GUARDS
		p = (byte*)p - VMEM_COALESCE_GUARD_SIZE;
	#endif

		// get the header for this alloc
		Coalesce::Header* p_header = (Coalesce::Header*)p - 1;
		VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

		int size = p_header->m_Size - sizeof(Coalesce::Header);

	#ifdef VMEM_COALESCE_GUARDS
		size -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif

		return size;
	}

	//------------------------------------------------------------------------
	Coalesce::Region* CoalesceHeap::CreateRegion(bool add_to_heap_regions)
	{
		int page_size = m_VirtualMem.GetPageSize();

		// reserve region memory
		void* p_region_mem = m_VirtualMem.Reserve(m_RegionSize, page_size, m_ReserveFlags);
		if(!p_region_mem)
			return NULL;		// out of virtual memory!

		// we have a fixed header at the start and end of the region that are never
		// allocated or freed. This makes the logic for coalescing simpler because
		// we can always assume there is a prev and next header.

		// commit the first page to store the start fixed alloc header and the free node header
		int offset = sizeof(Coalesce::Region) + 2*sizeof(Coalesce::Header);
		int start_commit_size = AlignUpPow2(offset, page_size);
		if(!m_VirtualMem.Commit(p_region_mem, start_commit_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}
		VMEM_MEMSET((byte*)p_region_mem + offset, VMEM_FREED_MEM, start_commit_size - offset);
		VMEM_STATS(m_Stats.m_Unused += start_commit_size);

		// add a node for the entire free memory
		Coalesce::Node* p_node = AllocNode();
		if(!p_node)
		{
			VMEM_STATS(m_Stats.m_Unused -= start_commit_size);
			m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}

		// commit the last page to store the end fixed alloc
		int end_commit_size = AlignUpPow2(sizeof(Coalesce::Header), page_size);
		void* p_last_page = (byte*)p_region_mem + m_RegionSize - end_commit_size;
		if(p_last_page >= (byte*)p_region_mem + start_commit_size)
		{
			if(!m_VirtualMem.Commit(p_last_page, end_commit_size, m_CommitFlags))
			{
				FreeNode(p_node);
				VMEM_STATS(m_Stats.m_Unused -= start_commit_size);
				m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
				m_VirtualMem.Release(p_region_mem);
				return NULL;		// out of memory
			}
			VMEM_STATS(m_Stats.m_Unused += end_commit_size);
			VMEM_MEMSET(p_last_page, VMEM_FREED_MEM, end_commit_size);
		}

		// allocate region object at the start of the region memory
		Coalesce::Region* p_region = (Coalesce::Region*)p_region_mem;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		new (p_region)Coalesce::Region(m_MaxSize - m_MinSize, Coalesce::g_FreeNodeBucketSize, m_InternalHeap);
#else
		new (p_region)Coalesce::Region();
#endif

		void* p_mem = (byte*)p_region_mem + sizeof(Coalesce::Region);

		// clear free node list head nodes
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;
		memset(p_free_node_lists, 0, sizeof(p_region->mp_FreeNodeLists));

		// the free lists are circular so setup the prev and next pointers for the list heads.
		for(int i=0; i<g_FreeNodeMapSize; ++i)
		{
			Coalesce::Node& node_list = p_free_node_lists[i];
			node_list.mp_Prev = &node_list;
			node_list.mp_Next = &node_list;
		}
#endif
		// we need to put a fixed allocation at the start and end of the memory so that
		// we dont have to check if we are the first or last allocation when coalescing.

		// setup the fixed start alloc
		Coalesce::Header* p_start_header = (Coalesce::Header*)p_mem;
		SetupHeader(p_start_header);
		p_start_header->m_Size = sizeof(Coalesce::Header);
		p_start_header->m_PrevSize = 0;
		p_start_header->m_NodeType = nt_Allocated;

		// setup the main free node
		Coalesce::Header* p_header = p_start_header + 1;
		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		SetupHeader(p_header);
		p_header->m_Size = total_free_size;
		p_header->m_PrevSize = sizeof(Coalesce::Header);

		// setup the fixed end alloc
		Coalesce::Header* p_end_header = (Coalesce::Header*)((byte*)p_header + total_free_size);
		SetupHeader(p_end_header);
		p_end_header->m_Size = sizeof(Coalesce::Header);
		p_end_header->m_PrevSize = total_free_size;
		p_end_header->m_NodeType = nt_Allocated;

		p_node->mp_Mem = p_header;
		p_node->m_Size = total_free_size;
		p_header->mp_Node = p_node;

		// insert the node into the last free node list
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		p_node->mp_Prev = p_node->mp_Next = NULL;
		InsertNode(p_node, p_region);
#else
		Coalesce::Node& node_list_head = p_free_node_lists[g_FreeNodeMapSize-1];
		node_list_head.mp_Next = p_node;
		node_list_head.mp_Prev = p_node;
		p_node->mp_Prev = &node_list_head;
		p_node->mp_Next = &node_list_head;

		p_region->m_LargestFreeNodeSize = total_free_size;
#endif
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_BucketTree.GetMemoryUsage());
#endif

	#ifdef VMEM_COALESCE_GUARD_PAGES
		if(!SetupGuardPages(p_region))
		{
			RemoveGuardPages(p_region);
			p_region->~Region();
			if(p_last_page >= (byte*)p_region_mem + start_commit_size)
				m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
			m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
			m_VirtualMem.Release(p_region_mem);
			return NULL;		// out of memory
		}
	#endif

		// add to heap regions array
		if(add_to_heap_regions)
		{
		#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			if(!m_HeapRegions.AddRegion(this, p_region, m_RegionSize, m_RegionType))
		#else
			if(!m_HeapRegions.AddRegion(p_region, m_RegionSize, m_RegionType))
		#endif
			{
				p_region->~Region();
				if(p_last_page >= (byte*)p_region_mem + start_commit_size)
					m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
				m_VirtualMem.Decommit(p_region_mem, start_commit_size, m_CommitFlags);
				m_VirtualMem.Release(p_region_mem);
				return NULL;		// out of memory
			}
		}

		return p_region;
	}

	//------------------------------------------------------------------------
	Coalesce::Header* CoalesceHeap::FindNextAllocatedHeader(Coalesce::Region* p_region, Coalesce::Header* p_header) const
	{
		Coalesce::Header* p_end_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize) - 1;

		// find the next allocated header by skipping over free nodes and fragment nodes
		while(p_header->m_NodeType != nt_Allocated)
		{
			p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);
			VMEM_ASSERT((byte*)p_header >= (byte*)p_region && (byte*)p_header < (byte*)p_region + m_RegionSize, "p_header out of range");
		}
		return p_header != p_end_header ? p_header : NULL;
	}

	//------------------------------------------------------------------------
	// warning: this deletes p_region, don't use p_region after calling this function
	void CoalesceHeap::ClearRegion(Coalesce::Region* p_region)
	{
		// get the first header
		Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;		// +1 for the fixed start alloc

		// get the first allocated header
		p_header = FindNextAllocatedHeader(p_region, p_header);

		while(p_header)
		{
			// check the header
			VMEM_ASSERT_MEM(p_header->m_NodeType == nt_Allocated, &p_header->m_NodeType);
			VMEM_ASSERT((byte*)p_header >= (byte*)p_region && (byte*)p_header < (byte*)p_region + m_RegionSize, "p_header out of range");

			// find the next allocated header by skipping over free nodes and fragment nodes
			Coalesce::Header* p_next_header =  (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			p_next_header = FindNextAllocatedHeader(p_region, p_next_header);

			// free the alloc
			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_header + 1;
			#ifdef VMEM_COALESCE_GUARDS
				p = (byte*)p + VMEM_COALESCE_GUARD_SIZE;
			#endif
			FreeInternal(p);

			p_header = p_next_header;
		}

	#ifdef VMEM_COALESCE_GUARD_PAGES
		// region won't have been destroyed because it still contains the alloc guards
		RemoveGuardPages(p_region);

		if(p_region != mp_RegionList)
			DestroyRegion(p_region);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::DestroyRegion(Coalesce::Region* p_region)
	{
		CheckIntegrity_NoLock();

#if defined(VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD) && defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
		// remove buckets from update list
		int bucket_count = p_region->m_BucketTree.GetBucketCount();
		for(int i=0; i<bucket_count; ++i)
		{
			Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
			if(bucket.m_NeedsSort)
			{
				bucket.m_NeedsSort = false;
				RemoveNodeBucketFromSortList(&bucket);
			}
		}
#endif

		if(p_region != mp_RegionList || m_AddInitialRegionToHeapRegions)
			m_HeapRegions.RemoveRegion(p_region);

#ifdef VMEM_ASSERTS
		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		Coalesce::Header* p_header = (Coalesce::Header*)((byte*)p_region + sizeof(Coalesce::Region) + sizeof(Coalesce::Header));
		VMEM_ASSERT(p_header->m_Size == total_free_size, "trying to destrroy non-empty region");
#endif

		// take region off list
		if(p_region == mp_RegionList)
		{
			mp_RegionList = p_region->mp_Next;
		}
		else
		{
			Coalesce::Region* p_srch_region = mp_RegionList;
			while(p_srch_region)
			{
				Coalesce::Region* p_next = p_srch_region->mp_Next;
				VMEM_ASSERT(p_next, "unable to find region to remove");
				VMEM_ASSUME(p_next);

				if(p_next == p_region)
				{
					p_srch_region->mp_Next = p_region->mp_Next;
					break;
				}

				p_srch_region = p_next;
			}
		}

		// free the last free node that was alloced when creating the region
		Coalesce::Header* p_free_header = (Coalesce::Header*)(p_region + 1) + 1;
		Coalesce::Node* p_node = p_free_header->mp_Node;
		VMEM_ASSERT_COALESCE_MARKER(&p_free_header->m_Marker);
		VMEM_ASSERT_MEM(p_free_header->m_NodeType != nt_Allocated && p_free_header->m_NodeType != nt_Fragment, &p_free_header->m_NodeType);
		VMEM_ASSERT_CODE(VMEM_ASSERT_MEM(p_free_header->m_Size == total_free_size, &p_free_header->m_Size));
		RemoveNode(p_node, p_region);
		FreeNode(p_node);

		VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Region) + 3*sizeof(Coalesce::Header));

		int page_size = m_VirtualMem.GetPageSize();

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		VMEM_STATS(m_Stats.m_Overhead -= p_region->m_BucketTree.GetMemoryUsage());
#endif
		// decommit the start fixed header and region struct
		int start_offset = sizeof(Coalesce::Region) + 2*sizeof(Coalesce::Header);
		int start_commit_size = AlignUpPow2(start_offset, page_size);

#ifdef VMEM_ENABLE_MEMSET
		Coalesce::Header* p_first_header = (Coalesce::Header*)((byte*)p_region + sizeof(Coalesce::Region) + sizeof(Coalesce::Header));
		int free_size = p_first_header->m_Size - sizeof(Coalesce::Header);
		int commit_size = start_commit_size - start_offset;
		int check_size = VMem::VMin(free_size, commit_size);
		CheckMemory(p_first_header+1, check_size, VMEM_FREED_MEM);
#endif

		p_region->~Region();

		m_VirtualMem.Decommit(p_region, start_commit_size, m_CommitFlags);
		VMEM_STATS(m_Stats.m_Unused -= start_commit_size);

		// decommit the end fixed header
		int end_commit_size = AlignUpPow2(sizeof(Coalesce::Header), page_size);
		void* p_last_header = (byte*)p_region + m_RegionSize - sizeof(Coalesce::Header);
		void* p_last_page = AlignDownPow2(p_last_header, page_size);
		if(p_last_page >= (byte*)p_region + start_commit_size)
		{
			#if defined(VMEM_ENABLE_MEMSET) && !defined(VMEM_MEMSET_ONLY_SMALL)
				if(p_last_header > p_last_page)
					CheckMemory(p_last_page, (byte*)p_last_header - (byte*)p_last_page, VMEM_FREED_MEM);
			#endif

			m_VirtualMem.Decommit(p_last_page, end_commit_size, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= end_commit_size);
		}

		m_VirtualMem.Release(p_region);

		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
	}

	//------------------------------------------------------------------------
	// ensure all the complete pages in the range are committed
	// |-------|---C---|-------|	<- only commit middle page, other pages will already be committed
	//      xxxxxxxxxxxxxx			<- range
	bool CoalesceHeap::CommitRange(void* p, int size)
	{
		VMEM_ASSERT(p, "invalid pointer passed to CommitRange");
		VMEM_ASSERT(size > 0, "invalid size passed to CommitRange");

		int page_size = m_VirtualMem.GetPageSize();

		void* p_page_start = AlignUpPow2(p, page_size);
		void* p_page_end = AlignDownPow2((byte*)p + size, page_size);

		if(p_page_start < p_page_end)
		{
			size_t commit_size = (byte*)p_page_end - (byte*)p_page_start;

			if(!m_VirtualMem.Commit(p_page_start, commit_size, m_CommitFlags))
				return false;

			VMEM_STATS(m_Stats.m_Unused += commit_size);
			VMEM_MEMSET(p_page_start, VMEM_FREED_MEM, commit_size);
		}

		return true;
	}

	//------------------------------------------------------------------------
	// ensure all the complete pages in the range are decommitted
	// |-------|---D---|-------|	<- decommit just the middle page
	//      xxxxxxxxxxxxxx			<- range
	void CoalesceHeap::DecommitRange(void* p, int size)
	{
		VMEM_ASSERT(p, "invalid pointer passed to DecommitRange");
		VMEM_ASSERT(size > 0, "invalid size passed to DecommitRange");

		int page_size = m_VirtualMem.GetPageSize();

		void* p_page_start = AlignUpPow2(p, page_size);
		void* p_page_end = AlignDownPow2((byte*)p + size, page_size);

		if(p_page_start < p_page_end)
		{
			size_t decommit_size = (byte*)p_page_end - (byte*)p_page_start;

			m_VirtualMem.Decommit(p_page_start, decommit_size, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= decommit_size);
		}
	}

	//------------------------------------------------------------------------
	Coalesce::Node* CoalesceHeap::AllocNode()
	{
		Coalesce::Node* p_node = m_NodeFSA.Alloc();
		VMEM_ASSERT_CODE(if(p_node) { p_node->mp_Prev = p_node->mp_Next = NULL; });
		return p_node;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::FreeNode(Coalesce::Node* p_node)
	{
		VMEM_ASSERT_MEM(!p_node->mp_Prev, &p_node->mp_Prev);		// trying to free node that is still on a list?
		VMEM_ASSERT_MEM(!p_node->mp_Next, &p_node->mp_Next);		// trying to free node that is still on a list?
		m_NodeFSA.Free(p_node);
	}

	//------------------------------------------------------------------------
	int CoalesceHeap::GetFreeListIndex(int size) const
	{
		// fragments don't go into the free list
		VMEM_ASSERT(size >= m_MinSize, "size passed to GetFreeListIndex is < min size");
		int r = m_MaxSize - m_MinSize;	// the range of possible sizes
		long long s = size - m_MinSize;	// the size (in range space)
#ifdef VMEM_SKEW_SIZES
		if(s > r) s = r;				// coalesced blocks can be bigger than the max size.

		// the skewing loses resolution if the range is not big enough, we are better of with a linear mapping
		// 36 seems about right for 32 bit ints.
		int max_index = g_FreeNodeMapSize - 1;
		if(r > 36 * g_FreeNodeMapSize)
		{
			// the skew formula is
			// skew = r - (s*s*s) / (r*r)
			// where s = r - s
			// this skews it so that there are more indices for smaller sizes which is usually beneficial
			s = r - s;
			s = r - (s * ((s*s)/r) ) / r;
		}

		int index = (int)((s * max_index) / r);
#else
		int index = VMin((int)(s * g_FreeNodeMapSize / r), g_FreeNodeMapSize-1);
#endif
		VMEM_ASSERT(index >= 0 && index < g_FreeNodeMapSize, "GetFreeListIndex returning index out of range");
		return index;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
	int CoalesceHeap::SizeToBucketValue(int size) const
	{
		VMEM_ASSERT(m_MaxSize >= m_MinSize, "CoalesceHeap: Corrupt m_MinSize or m_MaxSize");
		return VMin(size - m_MinSize, m_SizeRange);
	}
#endif

	//------------------------------------------------------------------------
	Coalesce::Node* CoalesceHeap::GetNode(int size, Coalesce::Region*& p_region)
	{
		// get the first region
		p_region = mp_RegionList;
		VMEM_ASSERT(p_region, "something has corrupted mp_RegionList");

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		int node_size = size + Coalesce::g_FreeNodeBucketSize - 1;	// increase to the next bucket size so that we can be guranteed that the first node we find will be big enough

		for(;;)
		{
			int bucket_index = p_region->m_BucketTree.FindExistingBucket(SizeToBucketValue(node_size));
			if(bucket_index != -1)
			{
				Coalesce::NodeBucket& node_bucket = p_region->m_BucketTree[bucket_index];

				Coalesce::Node* p_list = &node_bucket.m_Head;
				Coalesce::Node* p_node = p_list->mp_Next;
				while(p_node != p_list)
				{
					if(p_node->m_Size >= size)
					{
						RemoveNode(p_node, p_region, bucket_index);
						return p_node;
					}

					p_node = p_node->mp_Next;
				}
			}

			Coalesce::Region* p_next_region = p_region->mp_Next;

			if(!p_next_region)
			{
				p_next_region = CreateRegion();
				if(!p_next_region)
					return NULL;

				p_region->mp_Next = p_next_region;
			}

			p_region = p_next_region;
		}
#else
		// get the best list to start searching in
		int start_index = GetFreeListIndex(size);
		VMEM_ASSERT(start_index >= 0 && start_index < g_FreeNodeMapSize, "invalid index returned by GetFreeListIndex");

		int region_count = 0;

		// search for a free node list big enough to hold the alloc
		while(p_region)
		{
			if(p_region->m_LargestFreeNodeSize == INT_MAX)
				UpdateLargestFreeNodeSize(p_region);

			if (p_region->m_LargestFreeNodeSize >= size)
			{
				Coalesce::Node* p_free_node_lists = p_region->mp_FreeNodeLists;

				int index = start_index;
				Coalesce::Node* p_list_head = p_free_node_lists + index;
				Coalesce::Node* p_node = p_list_head->mp_Next;

				// Best Fit - find the smallest node that fits the request (the list is sorted by size, small to large)
				while(p_node != p_list_head && p_node->m_Size < size)
					p_node = p_node->mp_Next;

				// if we haven't found a node that is big enough in the list move on to the next
				// non-empty list. Note that if we move onto another list we don't need to iterate
				// down the list, we can just use the first node in the list because it is always
				// guaranteed to be big enough.
				while(p_node == p_list_head && index < g_FreeNodeMapSize)
				{
					++index;
					++p_list_head;
					p_node = p_list_head->mp_Next;
				}

				// if we've found a node return it
				if(index < g_FreeNodeMapSize && p_node != p_list_head)
				{
					RemoveNode(p_node, p_region);
					return p_node;
				}
			}

			// no next region, so create one
			if(!p_region->mp_Next)
			{
				p_region->mp_Next = CreateRegion();

				if(p_region->mp_Next && region_count > 10)
					VMem::DebugWrite(_T("*** VMem warning: CoalesceHeap with more than 10 regions. Consider making the region size larger.\n"));
			}

			// not found a node, so try the next region
			p_region = p_region->mp_Next;

			++region_count;
		}

		return NULL;		// out of memory
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::InsertNode(Coalesce::Node* p_node, Coalesce::Region* p_region)
	{
		VMEM_ASSERT(p_node->mp_Mem, "p_node->mp_Mem has been corrupted");
		VMEM_ASSERT_MEM(!p_node->mp_Prev, &p_node->mp_Prev);		// node already inserted?
		VMEM_ASSERT_MEM(!p_node->mp_Next, &p_node->mp_Next);
		VMEM_ASSERT(p_region == GetRegion(p_node->mp_Mem) || !GetRegion(p_node->mp_Mem), "Inserting node from another region");

		int node_size = p_node->m_Size;

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		int bucket_index = p_region->m_BucketTree.GetBucket(SizeToBucketValue(node_size));
		Coalesce::NodeBucket& node_bucket = p_region->m_BucketTree[bucket_index];

		Coalesce::Node* p_prev = &node_bucket.m_Head;

		#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			if(!node_bucket.m_NeedsSort)
			{
				node_bucket.m_NeedsSort = true;
				node_bucket.m_SortByAddressOnly = bucket_index == p_region->m_BucketTree.GetBucketCount()-1;
				AddNodeBucketToSortList(m_BucketsToSortList.mp_NextNeedsSortBucket, &node_bucket);
			}
		#else
			Coalesce::Node* p_end = &node_bucket.m_Head;
			void* p_node_mem = p_node->mp_Mem;

			bool sort_by_address_only = bucket_index == p_region->m_BucketTree.GetBucketCount()-1;

			if(sort_by_address_only)
			{
				Coalesce::Node* p_next = p_prev->mp_Next;
				while(p_next != p_end && p_next < p_node_mem)
				{
					p_prev = p_next;
					p_next = p_next->mp_Next;
				}
			}
			else
			{
				Coalesce::Node* p_next = p_prev->mp_Next;
				while(p_next != p_end && p_next->m_Size < node_size && p_next < p_node_mem)
				{
					p_prev = p_next;
					p_next = p_next->mp_Next;
				}
			}
		#endif
#else
		// get the list for this node size
		int index = GetFreeListIndex(node_size);
		Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + index;

		// find the best place to insert
		Coalesce::Node* p_prev = p_list_head;
		if(index == g_FreeNodeMapSize-1)
		{
	#ifndef VMEM_DISABLE_BIASING
			// always sort the top bucket by address only. No point doing best fit on blocks
			// that are much larger than our maximum allocation size, just take the lowest
			// in memory. this biasing reduces fragmentation.
			for(Coalesce::Node* p_iter=p_prev->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				if(p_node->mp_Mem < p_iter->mp_Mem)
					break;
				p_prev = p_iter;
			}
	#endif
		}
		else
		{
			for(Coalesce::Node* p_iter=p_prev->mp_Next; p_iter!=p_list_head; p_iter=p_iter->mp_Next)
			{
				// sort by size first and then by address, smallest to largest (in both)
	#ifdef VMEM_DISABLE_BIASING
				if(p_iter->m_Size > node_size)
	#else
				if(p_iter->m_Size > node_size ||
					(p_iter->m_Size == node_size && p_node->mp_Mem < p_iter->mp_Mem))
	#endif
				{
					break;
				}
				p_prev = p_iter;
			}
		}

		// update the largest node size
		if(p_node->m_Size > p_region->m_LargestFreeNodeSize)
			p_region->m_LargestFreeNodeSize = p_node->m_Size;
#endif
		// link the node in
		Coalesce::Node* p_next = p_prev->mp_Next;
		p_prev->mp_Next = p_node;
		p_node->mp_Prev = p_prev;
		p_node->mp_Next = p_next;
		p_next->mp_Prev = p_node;
	}

	//------------------------------------------------------------------------
	bool CoalesceHeap::UnlinkNode(Coalesce::Node* p_node)
	{
		Coalesce::Node* p_prev = p_node->mp_Prev;
		Coalesce::Node* p_next = p_node->mp_Next;

		VMEM_ASSERT(GetRegion(p_node->mp_Mem) == GetRegion(p_prev->mp_Mem) || !p_prev->mp_Mem, "Removing node from another region");
		VMEM_ASSERT(GetRegion(p_node->mp_Mem) == GetRegion(p_next->mp_Mem) || !p_next->mp_Mem, "Removing node from another region");

		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;

		p_node->mp_Prev = NULL;
		p_node->mp_Next = NULL;

		return p_prev == p_next;
	}

	//------------------------------------------------------------------------
	// unlink the node form the list
	void CoalesceHeap::RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region)
	{
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
		// if the node list is empty tell the BucketTree
		if(UnlinkNode(p_node))
		{
			int bucket_index = p_region->m_BucketTree.GetBucket(SizeToBucketValue(p_node->m_Size));
			p_region->m_BucketTree.ReleaseBucket(bucket_index);
		}
#else
		UnlinkNode(p_node);

		if(p_node->m_Size == p_region->m_LargestFreeNodeSize)
			p_region->m_LargestFreeNodeSize = INT_MAX;
#endif
	}

	//------------------------------------------------------------------------
	// unlink the node form the list
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
	void CoalesceHeap::RemoveNode(Coalesce::Node* p_node, Coalesce::Region* p_region, int bucket_index)
	{
		// if the node list is empty tell the BucketTree
		if(UnlinkNode(p_node))
			p_region->m_BucketTree.ReleaseBucket(bucket_index);
	}
#endif

	//------------------------------------------------------------------------
	// return the region that contains the specified address.
	Coalesce::Region* CoalesceHeap::GetRegion(void* p) const
	{
		int region_size = m_RegionSize;
		Coalesce::Region* p_region = mp_RegionList;
		while(p_region)
		{
			byte* p_mem = (byte*)p_region;
			if(p >= p_mem && p < p_mem + region_size)
				return p_region;

			p_region = p_region->mp_Next;
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::DebugWrite() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			VMem::DebugWrite(_T("--------------------------------\n"), p_region);
			VMem::DebugWrite(_T("Region 0x%08x\n"), p_region);

			// write the sequential memory blocks
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			while(p_header != p_last_header)
			{
				VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);

				int size = p_header->m_Size;
				void* p_start = p_header;
				void* p_end = (byte*)p_header + size;

				const _TCHAR* p_status;
				if(p_header->m_NodeType == nt_Fragment) p_status = _T("frag");
				else if(p_header->m_NodeType == nt_Fixed) p_status = _T("fixed");
				else if(p_header->m_NodeType == nt_Allocated) p_status = _T("alloc");
				else p_status = _T("free");

				VMem::DebugWrite(_T("\t0x%08x - 0x%08x %7d\t%s\n"), p_start, p_end, size, p_status);

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}

			// write the free node map
			VMem::DebugWrite(_T("\n"));
			VMem::DebugWrite(_T("Free nodes:\n"));

#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			int bucket_count = p_region->m_BucketTree.GetBucketCount();
			for(int i=0; i<bucket_count; ++i)
			{
				const Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
				const Coalesce::Node* p_list_head = &bucket.m_Head;

				if(p_list_head->mp_Next != p_list_head)
				{
					VMem::DebugWrite(_T("free list %d\n"), i);

					for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
						VMem::DebugWrite(_T("\t%08x %d\n"), p_node->mp_Mem, p_node->m_Size);
				}
			}
#else
			for(int i=0; i<g_FreeNodeMapSize; ++i)
			{
				Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + i;
				if(p_list_head->mp_Next != p_list_head)
				{
					VMem::DebugWrite(_T("free list %d\n"), i);

					for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
					{
						VMEM_ASSUME(p_node);
						VMem::DebugWrite(_T("\t%08x %d\n"), p_node->mp_Mem, p_node->m_Size);
					}
				}
			}
#endif
		}
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity() const
	{
#if defined(VMEM_ASSERTS) || defined(VMEM_TRAIL_GUARDS)
		CriticalSectionScope lock(m_CriticalSection);
#endif
		CheckIntegrity_NoLock();
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity_NoLock() const
	{
#ifdef VMEM_ASSERTS
		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			// check headers
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			size_t last_node_type = nt_Allocated;
			int prev_size = -1;
			while(p_header != p_last_header)
			{
				VMEM_ASSERT_COALESCE_MARKER(&p_header->m_Marker);
				VMEM_ASSERT(prev_size == -1 || p_header->m_PrevSize == prev_size, "header prev size is incorrect");
				prev_size = p_header->m_Size;

				VMEM_ASSERT_MEM(p_header->m_NodeType == nt_Allocated || p_header->m_NodeType != last_node_type, &p_header->m_NodeType);		// two consecutive free nodes of the same status
				last_node_type = p_header->m_NodeType;

				#ifdef VMEM_COALESCE_GUARDS
					if(p_header->m_NodeType == nt_Allocated)
					{
						CheckMemory(p_header + 1, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
						CheckMemory((byte*)p_header + p_header->m_Size - VMEM_COALESCE_GUARD_SIZE, VMEM_COALESCE_GUARD_SIZE, VMEM_GUARD_MEM);
					}
				#endif

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}

			// check free nodes
			prev_size = 0;
#ifdef VMEM_COALESCE_HEAP_BUCKET_TREE
			int bucket_count = p_region->m_BucketTree.GetBucketCount();
			for(int i=0; i<bucket_count; ++i)
			{
				const Coalesce::NodeBucket& bucket = p_region->m_BucketTree[i];
				const Coalesce::Node* p_list_head = &bucket.m_Head;
#else
			int largest_free_node_size = 0;
			for(int i=0; i<g_FreeNodeMapSize; ++i)
			{
				Coalesce::Node* p_list_head = p_region->mp_FreeNodeLists + i;
#endif
				VMEM_ASSERT_MEM(p_list_head->mp_Next, &p_list_head->mp_Next);		// found a list in the free node map with a null next pointer

				void* p_prev_addr = NULL;

				for(Coalesce::Node* p_node = p_list_head->mp_Next; p_node!=p_list_head; p_node=p_node->mp_Next)
				{
					VMEM_ASSERT(p_node, "null node");
					VMEM_ASSUME(p_node);
					Coalesce::Header* p_node_header = (Coalesce::Header*)p_node->mp_Mem;
#if !defined(VMEM_DISABLE_BIASING) && !defined(VMEM_COALESCE_HEAP_BUCKET_TREE)
					if(p_node->m_Size == prev_size || i == g_FreeNodeMapSize-1)
						VMEM_ASSERT_MEM(p_node->mp_Mem > p_prev_addr, &p_node->mp_Mem);		// Coalesce heap free node map list out of order
					else
						VMEM_ASSERT_MEM(p_node->m_Size > prev_size, &p_node->m_Size);		// Coalesce heap free node map list out of order
#endif
					// check memory
					CheckIntegrity(p_node_header);

					// remember sizes
					prev_size = p_node->m_Size;
					p_prev_addr = p_node->mp_Mem;

					VMEM_ASSERT(p_node_header, "Corrupted coalesce node");
					VMEM_ASSERT(p_node->m_Size == p_node_header->m_Size, "coalesce heap block size doens't match node size");
					VMEM_ASSERT_COALESCE_MARKER(&p_node_header->m_Marker);
					VMEM_ASSERT_MEM(p_node_header->mp_Node == p_node, &p_node_header->mp_Node);		// Header doesn't point back to free node

#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
					VMEM_ASSUME(p_node);
					if(p_node->m_Size > largest_free_node_size)
						largest_free_node_size = p_node->m_Size;
#endif
				}
			}

			VMEM_ASSERT_MEM(p_region->m_Marker[0] == VMEM_COALESCE_ALLOC_MARKER, &p_region->m_Marker[0]);
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
			VMEM_ASSERT_MEM(p_region->m_LargestFreeNodeSize == largest_free_node_size || p_region->m_LargestFreeNodeSize == INT_MAX, &p_region->m_LargestFreeNodeSize);
#endif
		}
#endif

#ifdef VMEM_TRAIL_GUARDS
		m_TrailGuard.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::CheckIntegrity(Coalesce::Header* p_header) const
	{
		void* p_check_mem = p_header + 1;
		size_t mem_check_size = p_header->m_Size - sizeof(Coalesce::Header);
		int page_size = m_VirtualMem.GetPageSize();
		void* p_start_page = AlignDownPow2((byte*)p_check_mem-1, page_size);
		void* p_end_page = AlignDownPow2((byte*)p_check_mem + mem_check_size, page_size);
		if((byte*)p_end_page - (byte*)p_start_page > page_size)
			mem_check_size = (byte*)p_start_page + page_size - (byte*)p_check_mem;
		mem_check_size = VMem::VMin(mem_check_size, Coalesce::g_MaxCheckMemorySize);

#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p_check_mem, mem_check_size, VMEM_FREED_MEM);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Flush()
	{
#ifdef VMEM_COALESCE_HEAP_CACHE
		CriticalSectionScope lock(m_CriticalSection);
		FlushCache(0);
#endif
	}

	//------------------------------------------------------------------------
	void CoalesceHeap::Update()
	{
		#ifdef VMEM_COALESCE_HEAP_CACHE
			if(m_CacheEnabled)
				m_Cache.Update();
		#endif

		#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
			CriticalSectionScope lock(m_CriticalSection);
			SortBuckets();
		#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_SORT_COALESCE_HEAP_ON_SERVICE_THREAD
	void CoalesceHeap::SortBuckets()
	{
		Coalesce::NodeBucket* VMEM_RESTRICT p_bucket = m_BucketsToSortList.mp_NextNeedsSortBucket;
		Coalesce::NodeBucket* VMEM_RESTRICT p_end_bucket = &m_BucketsToSortList;
		m_BucketsToSortList.mp_PrevNeedsSortBucket = m_BucketsToSortList.mp_NextNeedsSortBucket = &m_BucketsToSortList;

		while(p_bucket != p_end_bucket)
		{
			Coalesce::NodeBucket* p_next_bucket = p_bucket->mp_NextNeedsSortBucket;

			bool swapped = false;
			Coalesce::Node* p_head = &p_bucket->m_Head;
			bool sort_by_address_only = p_bucket->m_SortByAddressOnly;
			for(Coalesce::Node* p_node=p_head->mp_Next; p_node!=p_head && p_node->mp_Next!=p_head; p_node=p_node->mp_Next)
			{
				Coalesce::Node* p_next = p_node->mp_Next;
				if((!sort_by_address_only && p_node->m_Size < p_next->m_Size) || p_node->mp_Mem > p_next->mp_Mem)
				{
					SwapWithNextNode(p_node);
					swapped = true;
				}
			}

			if(swapped)
			{
				AddNodeBucketToSortList(m_BucketsToSortList.mp_NextNeedsSortBucket, p_bucket);
			}
			else
			{
				p_bucket->m_NeedsSort = false;
				p_bucket->mp_PrevNeedsSortBucket = p_bucket->mp_NextNeedsSortBucket = NULL;
			}

			p_bucket = p_next_bucket;
		}
	}
#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	size_t CoalesceHeap::WriteAllocs() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMem::DebugWrite(_T("CoalesceHeap %d-%d\n"), m_MinSize, m_MaxSize);

		int allocs_written = 0;
		int alloc_count = 0;
		size_t bytes_allocated = 0;

		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			// check headers
			Coalesce::Header* p_header = (Coalesce::Header*)(p_region + 1) + 1;
			Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));
			while(p_header != p_last_header)
			{
				if(p_header->m_NodeType == nt_Allocated)
				{
					if(allocs_written != VMEM_WRITE_ALLOCS_MAX)
					{
						const void* p_alloc = p_header + 1;
	#ifdef VMEM_COALESCE_GUARDS
						p_alloc = (byte*)p_alloc + VMEM_COALESCE_GUARD_SIZE;
	#endif
						WriteAlloc(p_alloc, p_header->m_Size);
						++allocs_written;
					}
					++alloc_count;
					bytes_allocated += p_header->m_Size - sizeof(Coalesce::Header);
	#ifdef VMEM_COALESCE_GUARDS
					bytes_allocated -= 2*VMEM_COALESCE_GUARD_SIZE;
	#endif
				}

				p_header = (Coalesce::Header*)((byte*)p_header + p_header->m_Size);
			}
		}

		if(allocs_written == VMEM_WRITE_ALLOCS_MAX)
			VMem::DebugWrite(_T("only showing first %d allocs\n"), VMEM_WRITE_ALLOCS_MAX);

		VMem::DebugWrite(_T("%d bytes allocated across %d allocations\n"), bytes_allocated, alloc_count);

		return bytes_allocated;
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_ENABLE_STATS
	void CoalesceHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::CoalesceHeapStats stats;
		stats.m_MinSize = m_MinSize;
		stats.m_MaxSize = m_MaxSize;
		stats.m_RegionSize = m_RegionSize;

		stats.m_RegionCount = 0;
		for(Coalesce::Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_CoalesceHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
	#endif

	//------------------------------------------------------------------------
	#ifdef VMEM_COALESCE_GUARD_PAGES
	bool CoalesceHeap::SetupGuardPages(Coalesce::Region* p_region)
	{
		Coalesce::Header* p_free_header = (Coalesce::Header*)(p_region+1) + 1;
		Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_free_header + p_free_header->m_Size);

		int page_size = m_VirtualMem.GetPageSize();

		void* p_prev_committed_page = AlignDownPow2(p_free_header, page_size);
		void* p_last_committed_page = AlignDownPow2(p_last_header, page_size);

		const size_t required_free_size = VMEM_COALESCE_GUARD_PAGES * m_MaxSize + 2* page_size;

		// we put a guard page every # times the max allocation size
		while(p_free_header->m_Size > (int)required_free_size)
		{
			//--------------------------------
			// create the new decommitted page block

			// find the page that we will leave decommitted
			void* p_page = AlignUpPow2((byte*)p_free_header + VMEM_COALESCE_GUARD_PAGES * m_MaxSize, page_size);
			VMEM_ASSERT((byte*)p_page + page_size <= (byte*)p_region + m_RegionSize, "Possible corrupted RegionSize. Guard page out of range");

			// get the guard header
			int guard_size = sizeof(Coalesce::Header) + page_size;
			Coalesce::Header* p_guard_header = (Coalesce::Header*)((byte*)p_page - sizeof(Coalesce::Header));

			// create a new node
			Coalesce::Node* p_node = AllocNode();
			if(!p_node)
				return false;

			// commit the page for the guard header.
			void* p_commit_page = AlignDownPow2(p_guard_header, page_size);
			bool commit_page_for_guard_header = p_commit_page != p_prev_committed_page;
			if(commit_page_for_guard_header)
			{
				if(!m_VirtualMem.Commit(p_commit_page, page_size, m_CommitFlags))
				{
					FreeNode(p_node);
					return false;		// out of memory
				}
				VMEM_STATS(m_Stats.m_Unused += page_size);
				VMEM_MEMSET(p_commit_page, VMEM_FREED_MEM, page_size);
				p_prev_committed_page = p_commit_page;
			}

			// commit the page that contains the next free header
			Coalesce::Header* p_next_free_header = (Coalesce::Header*)((byte*)p_guard_header + guard_size);
			void* p_next_free_commit_page = AlignDownPow2(p_next_free_header, page_size);
			if(p_next_free_commit_page != p_last_committed_page)
			{
				if(!m_VirtualMem.Commit(p_next_free_commit_page, page_size, m_CommitFlags))
				{
					FreeNode(p_node);
					if(commit_page_for_guard_header)
					{
						m_VirtualMem.Decommit(p_commit_page, page_size, m_CommitFlags);
						VMEM_STATS(m_Stats.m_Unused -= page_size);
					}

					return false;		// out of memory
				}
				VMEM_MEMSET(p_next_free_commit_page, VMEM_FREED_MEM, page_size);
				VMEM_STATS(m_Stats.m_Unused += page_size);
				p_prev_committed_page = p_next_free_commit_page;
			}

			// reduce the size of the prev free block
			int new_free_size = (int)((byte*)p_guard_header - (byte*)p_free_header);
			p_free_header->m_Size = new_free_size;
			RemoveNode(p_free_header->mp_Node, p_region);
			p_free_header->mp_Node->m_Size = new_free_size;
			InsertNode(p_free_header->mp_Node, p_region);

			// setup the guard header
			SetupHeader(p_guard_header);
			p_guard_header->m_Size = guard_size;
			p_guard_header->m_PrevSize = p_free_header->m_Size;
			p_guard_header->m_NodeType = nt_Fixed;

			//--------------------------------
			// setup the next free block

			int next_free_size = (int)((byte*)p_last_header - (byte*)p_next_free_header);

			// initialise the new node
			if(next_free_size < m_MinSize)
			{
				FreeNode(p_node);
				p_next_free_header->m_NodeType = nt_Fragment;
			}
			else
			{
				p_node->mp_Mem = p_next_free_header;
				p_node->m_Size = next_free_size;
				p_node->mp_Prev = p_node->mp_Next = NULL;
				InsertNode(p_node, p_region);
				p_next_free_header->mp_Node = p_node;
			}

			// setup the header
			SetupHeader(p_next_free_header);
			p_next_free_header->m_Size = next_free_size;
			p_next_free_header->m_PrevSize = guard_size;

			p_last_header->m_PrevSize = next_free_size;

			VMEM_STATS(m_Stats.m_Unused -= 2 * sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead += 2 * sizeof(Coalesce::Header));

			// move to the next free header
			p_free_header = p_next_free_header;
		}

		return true;
	}
	#endif

	//------------------------------------------------------------------------
	// this function assumes that all other allocs have been removed and only
	// the guard allocs remain. All guard allocs will be removed and the region
	// destroyed.
	#ifdef VMEM_COALESCE_GUARD_PAGES
	void CoalesceHeap::RemoveGuardPages(Coalesce::Region* p_region)
	{
		int page_size = m_VirtualMem.GetPageSize();

		// if the region is too small no guard pages will have been setup and the region will alreddy have been decallocated
		if(m_RegionSize - Coalesce::g_RegionOverhead < VMEM_COALESCE_GUARD_PAGES * m_MaxSize + 2* page_size)
			return;

		Coalesce::Header* p_first_free_header = (Coalesce::Header*)(p_region+1) + 1;
		Coalesce::Header* p_last_header = (Coalesce::Header*)((byte*)p_region + m_RegionSize - sizeof(Coalesce::Header));

		void* p_first_committed_page = AlignDownPow2(p_first_free_header, page_size);

		Coalesce::Header* p_guard_header = (Coalesce::Header*)((byte*)p_first_free_header + p_first_free_header->m_Size);

		while(p_guard_header != p_last_header)
		{
			VMEM_MEM_CHECK(&p_guard_header->m_NodeType, nt_Fixed);		// expected guard node

			// get the next free header
			Coalesce::Header* p_free_header = (Coalesce::Header*)((byte*)p_guard_header + p_guard_header->m_Size);
			VMEM_ASSERT_MEM(p_free_header->m_NodeType > nt_Allocated, &p_free_header->m_NodeType);		// expected free node
			Coalesce::Header* p_next_guard_header = (Coalesce::Header*)((byte*)p_free_header + p_free_header->m_Size);
			VMEM_MEMSET(p_guard_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// delete the free node
			if(p_free_header->m_NodeType != nt_Fragment)
			{
				RemoveNode(p_free_header->mp_Node, p_region);
				FreeNode(p_free_header->mp_Node);
			}
			VMEM_MEMSET(p_free_header, VMEM_FREED_MEM, sizeof(Coalesce::Header));

			// decommit the guard header page
			void* p_guard_header_page = AlignDownPow2(p_guard_header, page_size);
			void* p_free_header_page = AlignDownPow2(p_free_header, page_size);
			if(p_guard_header_page != p_free_header_page && p_guard_header_page != p_first_committed_page)
			{
				m_VirtualMem.Decommit(p_guard_header_page, page_size, m_CommitFlags);
				VMEM_STATS(m_Stats.m_Unused -= page_size);
			}
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));

			// decommit the free header page
			void* p_next_guard_header_page = AlignDownPow2(p_next_guard_header, page_size);
			if(p_free_header_page != p_next_guard_header_page && p_free_header_page != p_first_committed_page)
			{
				m_VirtualMem.Decommit(p_free_header_page, page_size, m_CommitFlags);
				VMEM_STATS(m_Stats.m_Unused -= page_size);
			}
			VMEM_STATS(m_Stats.m_Unused += sizeof(Coalesce::Header));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Coalesce::Header));

			p_guard_header = p_next_guard_header;
		}

		int total_free_size = m_RegionSize - Coalesce::g_RegionOverhead;
		p_first_free_header->m_Size = total_free_size;
		p_first_free_header->mp_Node->m_Size = total_free_size;
#ifndef VMEM_COALESCE_HEAP_BUCKET_TREE
		p_region->m_LargestFreeNodeSize = total_free_size;
#endif
		p_last_header->m_PrevSize = total_free_size;

		CheckIntegrity_NoLock();
	}
	#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_CACHE
	void CoalesceHeap::FlushCache(size_t max_mem)
	{
		if(m_CacheEnabled)
		{
			void* p_allocs_to_free = NULL;
			m_Cache.Trim(max_mem, p_allocs_to_free);

			void* p_alloc = p_allocs_to_free;
			while(p_alloc)
			{
				void* p_next = *(void**)p_alloc;

				VMEM_STATS(int size = GetSize(p_alloc));
				VMEM_STATS(m_Stats.m_Unused -= size);
				VMEM_STATS(m_Stats.m_Used += size);
				#ifdef VMEM_CUSTOM_ALLOC_INFO
					VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
					VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
				#endif

				FreeInternal(p_alloc);

				p_alloc = p_next;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// FSACache.h
//
#include <memory.h>
#include <new>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE

//------------------------------------------------------------------------
#define VMEM_USE_RELAXED_ATOMIC 1

//------------------------------------------------------------------------
#if !VMEM_USE_RELAXED_ATOMIC
	#include <atomic>
#endif

//------------------------------------------------------------------------
namespace VMem
{
#if VMEM_USE_RELAXED_ATOMIC
	//------------------------------------------------------------------------
	#ifdef VMEM_X64
		typedef RelaxedAtomic64<void*> FSACacheRelaxedAtomic;
	#else
		typedef RelaxedAtomic32<void*> FSACacheRelaxedAtomic;
	#endif

	//------------------------------------------------------------------------
	class FSACache
	{
	public:
		//------------------------------------------------------------------------
		FSACache(int count, InternalHeap& internal_heap)
		:	m_CountMask(count - 1),
			mp_Cache(NULL),
			m_AllocIndex(0),
			m_FreeIndex(0),
			m_InternalHeap(internal_heap),
			m_Count(count)
		{
			VMEM_ASSERT(IsPow2(count), "FSACache count not a power of 2");
		}

		//------------------------------------------------------------------------
		~FSACache()
		{
			if(mp_Cache)
				m_InternalHeap.Free(mp_Cache);
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			mp_Cache = (FSACacheRelaxedAtomic*)m_InternalHeap.Alloc(m_Count*sizeof(FSACacheRelaxedAtomic));
			if(!mp_Cache)
				return false;

			for(int i=0; i<m_Count; ++i)
			{
				new (mp_Cache+i)FSACacheRelaxedAtomic();
				mp_Cache[i] = NULL;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Alloc()
		{
			int index = m_AllocIndex;
			void* p = mp_Cache[index];

			if(p && mp_Cache[index].CompareExchange(p, NULL))
			{
				m_AllocIndex = (index + 1) & m_CountMask;
				return p;
			}

			return NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Free(void* p)
		{
			int index = m_FreeIndex;
			
			if(mp_Cache[index].CompareExchange(NULL, p))
			{
				m_FreeIndex = (index + 1) & m_CountMask;
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Set(void* p, int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			return mp_Cache[index].CompareExchange(NULL, p);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		// only used in the FSA destructor so doesn't need to be thread safe
		void* operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "FSACache index out of range");
			return mp_Cache[index];
		}

		//------------------------------------------------------------------------
		size_t GetMemoryOverhead() const
		{
			return m_Count * sizeof(FSACacheRelaxedAtomic);
		}

		//------------------------------------------------------------------------
		FSACache(const FSACache&);
		FSACache& operator=(const FSACache&);

		//------------------------------------------------------------------------
		// data
	private:
		const int m_CountMask;
		FSACacheRelaxedAtomic* mp_Cache;
		RelaxedAtomic32<int> m_AllocIndex;
		RelaxedAtomic32<int> m_FreeIndex;

		InternalHeap& m_InternalHeap;
		const int m_Count;
	};
#else
	//------------------------------------------------------------------------
	class FSACache
	{
	public:
		//------------------------------------------------------------------------
		FSACache(int count, InternalHeap& internal_heap)
		:	m_CountMask(count - 1),
			mp_Cache(NULL),
			m_AllocIndex(0),
			m_FreeIndex(0),
			m_Count(count),
			m_InternalHeap(internal_heap)
		{
			VMEM_ASSERT(IsPow2(count), "FSACache count not a power of 2");
		}

		//------------------------------------------------------------------------
		~FSACache()
		{
			if(mp_Cache)
				m_InternalHeap.Free(mp_Cache);
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			mp_Cache = (std::atomic<void*>*)m_InternalHeap.Alloc(m_Count*sizeof(std::atomic<void*>));
			if(!mp_Cache)
				return false;
			
			for(int i=0; i<m_Count; ++i)
			{
				new (mp_Cache+i)std::atomic<void*>();
				mp_Cache[i] = NULL;
			}

			return true;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void* Alloc()
		{
			int index = m_AllocIndex.load(std::memory_order_relaxed);
			void* p = mp_Cache[index];

			if(p && mp_Cache[index].compare_exchange_weak(p, NULL, std::memory_order_relaxed, std::memory_order_relaxed))
			{
				m_AllocIndex.store((index + 1) & m_CountMask, std::memory_order_relaxed);
				return p;
			}

			return NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Free(void* p)
		{
			int index = m_FreeIndex.load(std::memory_order_relaxed);
			
			void* p_null = NULL;
			if(mp_Cache[index].compare_exchange_weak(p_null, p, std::memory_order_relaxed, std::memory_order_relaxed))
			{
				m_FreeIndex.store((index + 1) & m_CountMask, std::memory_order_relaxed);
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE bool Set(void* p, int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "index out of range");

			void* p_null = NULL;
			return mp_Cache[index].compare_exchange_weak(p_null, p, std::memory_order_relaxed, std::memory_order_relaxed);
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		// only used in the FSA destructor so doesn't need to be thread safe
		void* operator[](int index) const
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "FSACache index out of range");
			return mp_Cache[index];
		}

		//------------------------------------------------------------------------
		size_t GetMemoryOverhead() const
		{
			return m_Count * sizeof(std::atomic<void*>);
		}

		//------------------------------------------------------------------------
		// data
	private:
		const int m_CountMask;
		std::atomic<void*>* mp_Cache;
		std::atomic<int> m_AllocIndex;
		std::atomic<int> m_FreeIndex;

		InternalHeap& m_InternalHeap;
		const int m_Count;
	};
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_FSA_CACHE

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PageHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct PageHeapRegion;
	class InternalHeap;

	//------------------------------------------------------------------------
	// PageHeap is used to allocate pages of a specific size. The page size must
	// be an exact multiple of the system page size, or vice versa.
	//
	// Note that the regions array was added to allow the Owns function to be lock free
	// because this wsas a point of contention for Realloc. The reason the region list
	// is still used is because it is ordered with the oldest region first. This biases
	// the allocs to the oldest regions allowing the new regions to be cleaned up. The
	// regions array is not ordered, it simply uses the first empty element. The regions
	// array could not be sorted and still be lockfree, so both the array and list are needed.
	//
	class PageHeap
	{
	public:
		PageHeap(
			int page_size,
			int region_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			int reserve_flags,
			int commit_flags,
			VirtualMem& virtual_mem);

		~PageHeap();

		bool Initialise();

		void* Alloc();

		void Free(void* p_page);

		VMEM_FORCE_INLINE bool Empty() const;

		VMEM_FORCE_INLINE int GetPageSize() const;

		void* GetFirstRegion();

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

	private:
		PageHeapRegion* CreateRegion();

		void UnlinkRegion(PageHeapRegion* p_region);

		void DestroyRegion(PageHeapRegion* p_region);

		PageHeapRegion* GetRegion(void* p) const;

		inline bool SysPageClear(int index, PageHeapRegion* p_region) const;

		PageHeap& operator=(const PageHeap& other);

		//------------------------------------------------------------------------
		// data
	private:
		mutable CriticalSection m_CriticalSection;

		int m_PageSize;
		int m_SysPageSize;
		int m_RegionSize;
		int m_PagesPerRegion;
		int m_PagesPerSysPage;

		int m_ReserveFlags;
		int m_CommitFlags;

		PageHeapRegion* mp_RegionList;

		HeapRegions& m_HeapRegions;
		RegionType::Enum m_RegionType;

		InternalHeap& m_InternalHeap;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};

	//------------------------------------------------------------------------
	int PageHeap::GetPageSize() const
	{
		return m_PageSize;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& PageHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& PageHeap::GetStatsNoLock() const
	{
		return m_Stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// FSA.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#if defined(VMEM_CHECK_ALLOCATED_MARKERS) || defined(VMEM_ENABLE_STATS)
	#define VMEM_FSA_TRACK_FULL_PAGES
#endif

#if defined(VMEM_ASSERTS) || defined(VMEM_X64)
	#define VMEM_FSA_HEADER_MARKER
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class FSA;

	//------------------------------------------------------------------------
	struct FSAPageHeader
	{
#ifdef VMEM_FSA_HEADER_MARKER
		unsigned int m_Marker;
#endif
		int m_UsedSlotCount;
		FSA* mp_FSA;
		void* mp_FreeSlots;
		FSAPageHeader* mp_Prev;
		FSAPageHeader* mp_Next;
	};

	//------------------------------------------------------------------------
	class FSAUpdateList
	{
	public:
		FSAUpdateList()
		:	mp_List(NULL)
		{
		}

		VMEM_FORCE_INLINE void Add(FSA* p_fsa);

		VMEM_FORCE_INLINE FSA* GetAndClearList();

	private:
		FSA* mp_List;
		CriticalSection m_CriticalSection;
	};

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	// p points to the start of the slot
	inline void SetGuards(void* p, int size, int slot_size)
	{
		SetGuards(p, VMEM_FSA_PRE_GUARD_SIZE);
		SetGuards((byte*)p + size, slot_size - size - VMEM_FSA_PRE_GUARD_SIZE);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	inline void CheckGuards(void* p, int size, int slot_size)
	{
		CheckMemory((byte*)p, VMEM_FSA_PRE_GUARD_SIZE, VMEM_GUARD_MEM);
		CheckMemory((byte*)p + VMEM_FSA_PRE_GUARD_SIZE + size, slot_size - size - VMEM_FSA_PRE_GUARD_SIZE, VMEM_GUARD_MEM);
	}
#endif

	//------------------------------------------------------------------------
	// FSA is a Fixed Size Allocator. It is setup with an allocation size, alignment
	// and a page heap. It allocates pages from the page heap and divides them up
	// into slots of the allocation size. The pages are put onto a free page list.
	// The slots are linked onto a free list. When allocating a slot of popped off
	// the free list, and added back onto the free list when freeing. When a page is
	// full it is removed from the free page list. When a page is empty it is given
	// back to the page heap. When a slot is freed from a full page the page is put
	// back onto the free page list. The freee page list is sorted by address for
	// biasing to reduce fragmentation.
	// This class is not thread safe.
	class FSA
	{
		friend FSAUpdateList;

	public:
		FSA(
			int size,
			int alignment,					// allocation size, alignment of each allocation and the page heap.
			PageHeap* p_page_heap,
			InternalHeap& internal_heap,
			FSAUpdateList& fsa_update_list,
			int page_size);

		~FSA();

		bool Initialise();

		VMEM_FORCE_INLINE void* Alloc();

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void Free(void* p, int page_size);

		VMEM_FORCE_INLINE static FSA* GetFSA(void* p, int page_size);		// get the FSA object for the address. p must be from an FSA.

		VMEM_FORCE_INLINE int GetSize() const;						// the allocation size

		void Flush();

#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
		void Update();
#endif
		void CheckIntegrity() const;

#ifdef VMEM_ENABLE_STATS
		inline const Stats& GetStats() const;
		inline const Stats& GetStatsNoLock() const;
		size_t WriteAllocs(int* p_allocs_written_count=NULL, int max_allocs_to_write=VMEM_WRITE_ALLOCS_MAX, bool write_header=true) const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

#ifdef VMEM_TRAIL_GUARDS
		inline void InitialiseTrailGuard(int size, int check_freq);
#endif

		void Lock() { m_CriticalSection.Enter(); }

		void Release() { m_CriticalSection.Leave(); }

		FSA* GetNextUpdateFSA() const { return mp_NextUpdateFSA; }

	protected:
#ifdef VMEM_FSA_CACHE
		void AddAllocToStats();
		void RemoveAllocFromStats();
		#ifdef VMEM_POPULATE_FSA_CACHE
			void PopulateCache();
		#endif
#endif

	private:
		VMEM_FORCE_INLINE void* Alloc_NoLock();

		VMEM_FORCE_INLINE void Free_NoLock(void* p, int page_size);

		void CheckIntegrity(FSAPageHeader* p_page) const;

		FSAPageHeader* AllocPage();

		VMEM_NO_INLINE FSAPageHeader* AllocPageAndInsert();

		void FreePage(FSAPageHeader* p_page);

		void InsertPageInFreeList(FSAPageHeader* p_page);

#ifdef VMEM_FSA_PAGE_CACHE
		VMEM_NO_INLINE void AddPageToEmptyList(FSAPageHeader* p_page);
		void ReleaseEmptyPages();
#endif

#ifdef VMEM_FSA_CACHE
		void ClearCache();
#endif
		// not implemented
		FSA(const FSA&);
		FSA& operator=(const FSA&);

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_FSA_CACHE
		FSACache m_FSACache;
#endif
		int m_Size;				// the allocation size
		int m_SlotSize;			// the slot size can be bigger than the allocation size because of alignment and guards
		int m_SlotsPerPage;

#ifdef VMEM_X64
		static const int m_Alignment = VMEM_NATURAL_ALIGNMENT;
#else
		const int m_Alignment;
#endif
		int m_AlignedHeaderSize;

		PageHeap* mp_PageHeap;

		List<FSAPageHeader> m_FreePageList;		// only contains partially full pages.

		FSAPageHeader* mp_LastInsertedFreePage;

		bool m_InUpdateList;
		FSA* mp_NextUpdateFSA;
		FSAUpdateList& m_UpdateList;

#ifdef VMEM_FSA_PAGE_CACHE
		List<FSAPageHeader> m_EmptyPageList;
		int m_EmptyPageListCount;
#endif

#ifdef VMEM_ASSERTS
		unsigned int m_Marker;
#endif

#ifdef VMEM_FSA_TRACK_FULL_PAGES
		List<FSAPageHeader> m_FullPageList;
#endif

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif

#ifdef VMEM_TRAIL_GUARDS
		TrailGuard m_TrailGuard;
#endif
		mutable CriticalSection m_CriticalSection;
	};

	//------------------------------------------------------------------------
	FSA* FSA::GetFSA(void* p, int page_size)
	{
		size_t page_mask = ~(page_size - 1);
		FSAPageHeader* p_page = (FSAPageHeader*)((size_t)p & page_mask);
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif
		FSA* p_fsa = p_page->mp_FSA;
		VMEM_ASSERT_CODE(VMEM_MEM_CHECK(&p_fsa->m_Marker, VMEM_FSA_MARKER));
		return p_fsa;
	}

	//------------------------------------------------------------------------
	void* FSA::Alloc()
	{
#ifdef VMEM_FSA_CACHE
		void* p_cached = m_FSACache.Alloc();
		if(p_cached)
		{
			#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				AddAllocToStats();
			#endif
			return p_cached;
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		return Alloc_NoLock();
	}

	//------------------------------------------------------------------------
	void* FSA::Alloc_NoLock()
	{
		// get the first page with a free slot
		FSAPageHeader* VMEM_RESTRICT p_page = m_FreePageList.GetHead();		// if m_FreePageList is empty GetHead will return the end iterator which will have a NULL mp_FreeSlots

		void* VMEM_RESTRICT p = p_page->mp_FreeSlots;

		// allocate a page if we need to
		if(!p)
		{
			p_page = AllocPageAndInsert();
			if(!p_page)
				return NULL;		// out of memory
			p = p_page->mp_FreeSlots;
		}

		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		// take a slot off the page
		VMEM_ASSERT_MEM(p >= p_page+1 && p < (byte*)p_page + mp_PageHeap->GetPageSize(), &p_page->mp_FreeSlots);

		void* VMEM_RESTRICT p_next = *(void**)p;
		p_page->mp_FreeSlots = p_next;
		VMEM_ASSERT_MEM(!p_page->mp_FreeSlots || (p_page->mp_FreeSlots >= p_page+1 && p_page->mp_FreeSlots < (byte*)p_page + mp_PageHeap->GetPageSize()), p);

		++p_page->m_UsedSlotCount;

		// if the page is full take it off the free list
		if(!p_next)
		{
			if(p_page == mp_LastInsertedFreePage)
				mp_LastInsertedFreePage = p_page->mp_Prev;

			m_FreePageList.Remove(p_page);

			#ifdef VMEM_FSA_TRACK_FULL_PAGES
				// put the page onto the full list
				m_FullPageList.AddTail(p_page);
			#endif
		}

		#ifdef VMEM_ENABLE_MEMSET
			if(m_Size > (int)sizeof(void*))
				CheckMemory((byte*)p + sizeof(void*), m_SlotSize - sizeof(void*), VMEM_FREED_MEM);
		#endif
		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, m_Size);

		#ifdef VMEM_FSA_GUARDS
			SetGuards(p, m_Size, m_SlotSize);
			p = (byte*)p + VMEM_FSA_PRE_GUARD_SIZE;
		#endif

		VMEM_STATS(m_Stats.m_Unused -= m_SlotSize);
		VMEM_STATS(m_Stats.m_Used += m_Size);
		VMEM_STATS(m_Stats.m_Overhead += m_SlotSize - m_Size);

		return p;
	}

	//------------------------------------------------------------------------
	void FSA::Free(void* VMEM_RESTRICT p)
	{
		Free(p, mp_PageHeap->GetPageSize());
	}

	//------------------------------------------------------------------------
	void FSA::Free(void* VMEM_RESTRICT p, int page_size)
	{
#ifdef VMEM_FSA_CACHE
		if(m_FSACache.Free(p))
		{
			#ifdef VMEM_ENABLE_STATS
				CriticalSectionScope lock(m_CriticalSection);
				RemoveAllocFromStats();
			#endif
			return;
		}
#endif
		CriticalSectionScope lock(m_CriticalSection);
		Free_NoLock(p, page_size);
	}

	//------------------------------------------------------------------------
	void FSA::Free_NoLock(void* VMEM_RESTRICT p, int page_size)
	{
		#ifdef VMEM_TRAIL_GUARDS
			if(m_Size >= sizeof(void*) + sizeof(unsigned int) && *((unsigned int*)((void**)p+1)) == VMEM_TRAIL_GUARD_MEM)
			{
				bool already_free = m_TrailGuard.Contains(p);
				VMEM_ASSERT2(!already_free, "Double free of pointer %s%p", VMEM_POINTER_PREFIX, p);
				if(already_free)
					return;
			}
		#endif

		#ifdef VMEM_FSA_GUARDS
			// check for double free
			VMEM_ASSERT(m_SlotSize - m_Size >= (int)sizeof(unsigned int), "VMEM_FSA_POST_GUARD_SIZE must be >= sizeof(unsigned int)");
			if(*(unsigned int*)((byte*)p + m_Size) == VMEM_FREED_MEM)
			{
				VMem::DebugWrite(_T("Possible double free of pointer 0x%p\n"), p);
				if(!VMem::Break(_T("VMem Assert\n")))
					VMEM_DEBUG_BREAK;
				return;
			}

			p = (byte*)p - VMEM_FSA_PRE_GUARD_SIZE;
			CheckGuards(p, m_Size, m_SlotSize);
		#endif

		#ifdef VMEM_TRAIL_GUARDS
			if(m_TrailGuard.GetSize())
			{
				p = m_TrailGuard.Add(p);
				VMEM_STATS(m_Stats.m_Used -= m_Size);		// move stats to overhead
				VMEM_STATS(m_Stats.m_Overhead += m_Size);
				if(!p) return;							// return here if TrailGuard ddin't return an alloc to free

				VMEM_STATS(m_Stats.m_Used += m_Size);		// move stats back to used
				VMEM_STATS(m_Stats.m_Overhead -= m_Size);
				#ifdef VMEM_FSA_GUARDS
					SetGuards(p, m_Size, m_SlotSize);	// re-apply our guards
				#endif
			}
		#endif

		// memset to VMEM_FREED_MEM
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			void* p_memset = p;
			if(m_SlotSize > (int)sizeof(void*))
				VMEM_MEMSET((byte*)p_memset + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
		#endif

		// get the page that owns this alloc
		FSAPageHeader* VMEM_RESTRICT p_page = (FSAPageHeader*)AlignDownPow2(p, page_size);
		#ifdef VMEM_ASSERTS
			VMEM_ASSERT((size_t)p >= (size_t)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment) && (size_t)p < (size_t)p_page + m_AlignedHeaderSize + m_SlotSize * m_SlotsPerPage, "Bad pointer passed to Free. Pointer not in free list range");
			VMEM_ASSERT((((size_t)p - (size_t)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment)) % m_SlotSize) == 0, "Bad pointer passed to Free. Pointer not aligned to slot");
		#endif

		// if the page was full put it back onto the free list
		void* p_free_slot = p_page->mp_FreeSlots;
		if(!p_free_slot)
		{
			#ifdef VMEM_FSA_TRACK_FULL_PAGES
				m_FullPageList.Remove(p_page);
			#endif
			InsertPageInFreeList(p_page);
		}

		// put the slot back onto the free list
		*(void**)p = p_free_slot;
		p_page->mp_FreeSlots = p;

		// if the page is empty free it
		if(!--p_page->m_UsedSlotCount)
		{
			if(p_page == mp_LastInsertedFreePage)
				mp_LastInsertedFreePage = p_page->mp_Prev;

			m_FreePageList.Remove(p_page);

			#ifdef VMEM_FSA_PAGE_CACHE
				AddPageToEmptyList(p_page);
			#else
				FreePage(p_page);
			#endif
		}

		VMEM_STATS(m_Stats.m_Unused += m_SlotSize);
		VMEM_STATS(m_Stats.m_Used -= m_Size);
		VMEM_STATS(m_Stats.m_Overhead -= m_SlotSize - m_Size);
	}

	//------------------------------------------------------------------------
	int FSA::GetSize() const
	{
		return m_Size;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& FSA::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	const Stats& FSA::GetStatsNoLock() const
	{
		return m_Stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_TRAIL_GUARDS
	void FSA::InitialiseTrailGuard(int size, int check_freq)
	{
		CriticalSectionScope lock(m_CriticalSection);
		m_TrailGuard.Initialise(size, check_freq, m_SlotSize);
	}
#endif

	//------------------------------------------------------------------------
	void FSAUpdateList::Add(FSA* p_fsa)
	{
		CriticalSectionScope lock(m_CriticalSection);
		p_fsa->mp_NextUpdateFSA = mp_List;
		mp_List = p_fsa;
	}

	//------------------------------------------------------------------------
	FSA* FSAUpdateList::GetAndClearList()
	{
		CriticalSectionScope lock(m_CriticalSection);
		FSA* p_list = mp_List;
		mp_List = NULL;
		return p_list;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// FSA.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#ifdef VMEM_FSA_GUARDS
	VMEM_STATIC_ASSERT((VMEM_FSA_PRE_GUARD_SIZE & 3) == 0, "pre-guard size not aligned");
	VMEM_STATIC_ASSERT((VMEM_FSA_POST_GUARD_SIZE & 3) == 0, "post guard size not aligned");
#endif

	//------------------------------------------------------------------------
	FSA::FSA(
		int size,
		int alignment,
		PageHeap* p_page_heap,
		InternalHeap& internal_heap,
		FSAUpdateList& fsa_update_list,
		int page_size)
	:
#ifdef VMEM_FSA_CACHE
		m_FSACache(AlignUpToNextPow2(VMEM_FSA_CACHE_SIZE / size), internal_heap),
#endif
		m_Size(size),
		m_SlotSize(size),
#ifndef VMEM_X64
		m_Alignment(alignment),
#endif
		mp_PageHeap(p_page_heap),
		mp_LastInsertedFreePage(NULL),
		m_InUpdateList(false),
		mp_NextUpdateFSA(NULL),
		m_UpdateList(fsa_update_list)
#ifdef VMEM_FSA_PAGE_CACHE
		,m_EmptyPageListCount(0)
#endif
	{
#ifndef VMEM_FSA_CACHE
		VMEM_UNREFERENCED_PARAM(internal_heap);
#endif

#ifdef VMEM_ASSERTS
		m_Marker = VMEM_FSA_MARKER;
#endif

		if((page_size % alignment) == 0)
			m_AlignedHeaderSize = AlignUp(ToInt(sizeof(FSAPageHeader)), alignment);
		else
			m_AlignedHeaderSize = sizeof(FSAPageHeader) + alignment;

#ifdef VMEM_X64
		VMEM_ASSERT(alignment == m_Alignment, "Non-natural alignment not supported in 64bit");
#endif

#ifdef VMEM_FSA_GUARDS
		m_SlotSize += VMEM_FSA_PRE_GUARD_SIZE + VMEM_FSA_POST_GUARD_SIZE;
#endif
		m_SlotSize = AlignUp(m_SlotSize, alignment);

		m_SlotsPerPage = (p_page_heap->GetPageSize() - m_AlignedHeaderSize) / m_SlotSize;

#ifdef VMEM_FSA_CACHE
		VMEM_STATS(m_Stats.m_Overhead += m_FSACache.GetMemoryOverhead());
#endif

		// it's important to setup these values because they are returned by GetHead if the list is empty
		FSAPageHeader* p_iter_end = m_FreePageList.GetIterEnd();
		#ifdef VMEM_FSA_HEADER_MARKER
			p_iter_end->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
		#endif
		p_iter_end->mp_FSA = NULL;
		p_iter_end->mp_FreeSlots = NULL;
		p_iter_end->m_UsedSlotCount = 0;

		mp_LastInsertedFreePage = m_FreePageList.GetIterEnd();

#ifdef VMEM_FSA_TRACK_FULL_PAGES
		FSAPageHeader* p_full_iter_end = m_FullPageList.GetIterEnd();
		#ifdef VMEM_FSA_HEADER_MARKER
			p_full_iter_end->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
		#endif
		p_full_iter_end->mp_FSA = NULL;
		p_full_iter_end->mp_FreeSlots = NULL;
		p_full_iter_end->m_UsedSlotCount = 0;
#endif
	}

	//------------------------------------------------------------------------
	FSA::~FSA()
	{
#ifdef VMEM_FSA_CACHE
		int cache_count = m_FSACache.GetCount();
		for(int i=0; i<cache_count; ++i)
		{
			void* p = m_FSACache[i];
			if(p)
				Free(p);
		}
#endif

#ifdef VMEM_TRAIL_GUARDS
		void* p_trail_alloc = m_TrailGuard.Shutdown();
		while(p_trail_alloc)
		{
			void* p_next = *(void**)p_trail_alloc;
#ifdef VMEM_FSA_GUARDS
			SetGuards(p_trail_alloc, m_Size, m_SlotSize);	// re-apply our guards
#endif
			Free(p_trail_alloc);
			p_trail_alloc = p_next;
		}
#endif
	}

	//------------------------------------------------------------------------
	bool FSA::Initialise()
	{
#ifdef VMEM_FSA_CACHE
		return m_FSACache.Initialise();
#else
		return true;
#endif
	}

	//------------------------------------------------------------------------
	FSAPageHeader* FSA::AllocPageAndInsert()
	{
		FSAPageHeader* p_page = AllocPage();

		VMEM_ASSERT(m_FreePageList.Empty(), "m_FreePageList shouold be empty at this point");
		if(p_page)
			m_FreePageList.AddHead(p_page);

		return p_page;
	}

	//------------------------------------------------------------------------
	FSAPageHeader* FSA::AllocPage()
	{
#ifdef VMEM_FSA_PAGE_CACHE
		VMEM_ASSERT(m_EmptyPageListCount == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
		if(m_EmptyPageListCount)
		{
			--m_EmptyPageListCount;
			VMEM_ASSERT(m_EmptyPageListCount >= 0, "m_EmptyPageListCount has been corrupted");

			return m_EmptyPageList.RemoveHead();
		}
#endif

		// allocate the page
		FSAPageHeader* p_page = (FSAPageHeader*)mp_PageHeap->Alloc();
		if(!p_page)
			return NULL;	// out of memory

		// put all slots onto the free list
		// note that slots are always aligned to slot size
		byte* p_free_slots = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
		byte* p = p_free_slots;
		byte* p_end = p_free_slots + (m_SlotsPerPage-1) * m_SlotSize;
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			bool do_memset = m_Size > (int)sizeof(void*);
		#endif
		while(p != p_end)
		{
			byte* p_next = p + m_SlotSize;
			*(byte**)p = p_next;
			#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
				if(do_memset) VMEM_MEMSET((byte*)p + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
			#endif
			p = p_next;
		}
		#if defined(VMEM_ENABLE_MEMSET) || defined(VMEM_MEMSET_ONLY_SMALL)
			if(do_memset) VMEM_MEMSET((byte*)p + sizeof(void*), VMEM_FREED_MEM, m_SlotSize - sizeof(void*));
		#endif
		*(byte**)p = NULL;

		// setup the page
		p_page->mp_FSA = this;
		p_page->mp_FreeSlots = p_free_slots;
		p_page->m_UsedSlotCount = 0;
		p_page->mp_Prev = NULL;
		p_page->mp_Next = NULL;

#ifdef VMEM_ASSERTS
		int page_size = mp_PageHeap->GetPageSize();
		int used_size = m_AlignedHeaderSize + m_SlotsPerPage * m_SlotSize;
		int unused_size = page_size - used_size;
		if (unused_size)
			memset((byte*)p_page + used_size, VMEM_GUARD_MEM, unused_size);
#endif

#ifdef VMEM_FSA_HEADER_MARKER
		p_page->m_Marker = VMEM_FSA_PAGE_HEADER_MARKER;
#endif
		VMEM_STATS(m_Stats.m_Unused += page_size - m_AlignedHeaderSize);
		VMEM_STATS(m_Stats.m_Overhead += m_AlignedHeaderSize);

		return p_page;
	}

	//------------------------------------------------------------------------
	void FSA::FreePage(FSAPageHeader* p_page)
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		VMEM_ASSERT_CODE(CheckIntegrity(p_page));

		mp_PageHeap->Free(p_page);

		VMEM_STATS(m_Stats.m_Unused -= mp_PageHeap->GetPageSize() - m_AlignedHeaderSize);
		VMEM_STATS(m_Stats.m_Overhead -= m_AlignedHeaderSize);
	}

	//------------------------------------------------------------------------
	// keeps the free page list sorted by address for the biasing
	void FSA::InsertPageInFreeList(FSAPageHeader* p_page)
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

#ifdef VMEM_DISABLE_BIASING
		FSAPageHeader* p_srch_page = m_FreePageList.GetHead();
#else
		// find the previous page < p_page
		FSAPageHeader* p_end_page = m_FreePageList.GetIterEnd();
		FSAPageHeader* p_srch_page = mp_LastInsertedFreePage != p_end_page ? mp_LastInsertedFreePage : m_FreePageList.GetHead();

		if(p_srch_page != p_end_page && p_srch_page > p_page)
		{
			do
			{
				p_srch_page = p_srch_page->mp_Prev;
			} while(p_srch_page != p_end_page && p_srch_page > p_page);
		}
		else
		{
			while(p_srch_page != p_end_page && p_srch_page < p_page)
				p_srch_page = p_srch_page->mp_Next;

			p_srch_page = p_srch_page->mp_Prev;
		}

		mp_LastInsertedFreePage = p_page;
#endif
		m_FreePageList.Insert(p_srch_page, p_page);
	}

	//------------------------------------------------------------------------
	void FSA::Flush()
	{
#if defined(VMEM_FSA_CACHE) || defined(VMEM_FSA_PAGE_CACHE)
		CriticalSectionScope lock(m_CriticalSection);
#endif

#ifdef VMEM_FSA_CACHE
		ClearCache();
#endif

#ifdef VMEM_FSA_PAGE_CACHE
		ReleaseEmptyPages();
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::ClearCache()
	{
		int page_size = mp_PageHeap->GetPageSize();

		for(int i=0; i<m_FSACache.GetCount(); ++i)
		{
			void* p = m_FSACache[i];
			if(p)
			{
				#ifdef VMEM_ENABLE_STATS
					CriticalSectionScope lock(m_CriticalSection);
					AddAllocToStats();
				#endif

				Free_NoLock(p, page_size);

				if(m_FSACache.Set(p, i))
					RemoveAllocFromStats();
			}
		}
	}
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
	void FSA::Update()
	{
		CriticalSectionScope lock(m_CriticalSection);

		#ifdef VMEM_FSA_PAGE_CACHE
			VMEM_ASSERT(m_EmptyPageListCount == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
			int remove_count = m_EmptyPageListCount - VMEM_FSA_PAGE_CACHE_COUNT_MIN;
			if(remove_count > 0)
			{
				for(int i=0; i<remove_count; ++i)
				{
					FSAPageHeader* p_page = m_EmptyPageList.RemoveTail();
					FreePage(p_page);
				}
				m_EmptyPageListCount = VMEM_FSA_PAGE_CACHE_COUNT_MIN;
			}

			mp_NextUpdateFSA = NULL;
		#endif

		m_InUpdateList = false;

#if defined(VMEM_FSA_CACHE) && defined(VMEM_POPULATE_FSA_CACHE)
		PopulateCache();
#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_PAGE_CACHE
	void FSA::ReleaseEmptyPages()
	{
		while(!m_EmptyPageList.Empty())
		{
			FSAPageHeader* p_page = m_EmptyPageList.RemoveTail();
			FreePage(p_page);
		}

		m_EmptyPageListCount = 0;
	}
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_CACHE) && defined(VMEM_POPULATE_FSA_CACHE)
	void FSA::PopulateCache()
	{
		void* p = NULL;

		for(int i=0; i<m_FSACache.GetCount(); ++i)
		{
			if(!m_FSACache[i])
			{
				if(!p)
					p = Alloc_NoLock();

				if(m_FSACache.Set(p, i))
					RemoveAllocFromStats();
			}
		}

		if(p)
			Free_NoLock(p, mp_PageHeap->GetPageSize());
	}
#endif

	//------------------------------------------------------------------------
	void FSA::CheckIntegrity() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		m_FreePageList.CheckIntegrity();

		VMEM_ASSERT_MEM(m_FreePageList.GetIterEnd()->mp_FreeSlots == NULL, m_FreePageList.GetIterEnd()->mp_FreeSlots);

		const FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		FSAPageHeader* p_prev = NULL;
		for(FSAPageHeader* p_page=m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
		{
			CheckIntegrity(p_page);
			#ifndef VMEM_DISABLE_BIASING
				VMEM_ASSERT(p_page > p_prev, "FSA free list is not sorted correctly");
			#endif
			VMEM_UNREFERENCED_PARAM(p_prev);
			p_prev = p_page;
		}

#ifdef VMEM_CHECK_ALLOCATED_MARKERS
		const FSAPageHeader* p_full_end = m_FullPageList.GetIterEnd();
		for (FSAPageHeader* p_page = m_FullPageList.GetHead(); p_page != p_full_end; p_page = p_page->mp_Next)
			CheckIntegrity(p_page);
#endif
			
#ifdef VMEM_TRAIL_GUARDS
		m_TrailGuard.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void FSA::CheckIntegrity(FSAPageHeader* p_page) const
	{
		#ifdef VMEM_FSA_HEADER_MARKER
			VMEM_MEM_CHECK(&p_page->m_Marker, VMEM_FSA_PAGE_HEADER_MARKER);
		#endif

		VMEM_ASSERT_CODE(VMEM_MEM_CHECK(&p_page->mp_FSA->m_Marker, VMEM_FSA_MARKER));

		#ifdef VMEM_ASSERTS
			int page_size = mp_PageHeap->GetPageSize();
			int used_size = m_AlignedHeaderSize + m_SlotsPerPage * m_SlotSize;
			int unused_size = page_size - used_size;
			if (unused_size)
				CheckMemory((byte*)p_page + used_size, unused_size, VMEM_GUARD_MEM);
		#endif

		// check p_slot pointers are valid and the free slot count matches
		int free_slot_count = 0;
		void* p_slot = p_page->mp_FreeSlots;
		
		VMEM_ASSERT_CODE(void* p_slot_addr = &p_page->mp_FreeSlots);

		// if the assert fails re-get the slot because the assert callback in the test framework will have repaired it
		VMEM_ASSERT_MEM(!p_slot || (p_slot >= p_page+1 && p_slot < (byte*)p_page + mp_PageHeap->GetPageSize()), p_slot_addr);
		p_slot = p_page->mp_FreeSlots;

		while(p_slot && free_slot_count < m_SlotsPerPage)
		{
			++free_slot_count;
			VMEM_ASSERT_CODE(p_slot_addr = p_slot);
			void* p_old_slot = p_slot;
			p_slot = *(void**)p_old_slot;

			// if the assert fails re-get the slot because the assert callback in the test framework will have repaired it
			VMEM_ASSERT_MEM(!p_slot || (p_slot >= p_page+1 && p_slot < (byte*)p_page + mp_PageHeap->GetPageSize()), p_slot_addr);
			p_slot = *(void**)p_old_slot;
		}
		VMEM_ASSERT(free_slot_count == m_SlotsPerPage-p_page->m_UsedSlotCount, "FSA free slot list or m_UsedSlotCount corrupted");

		// check the memory contents
#ifdef VMEM_ENABLE_MEMSET
		if(m_Size > (int)sizeof(void*))
		{
			int check_size = m_SlotSize - sizeof(void*);
			void* p_slot_iter = p_page->mp_FreeSlots;
			while(p_slot_iter)
			{
				CheckMemory((void**)p_slot_iter+1, check_size, VMEM_FREED_MEM);
				p_slot_iter = *(void**)p_slot_iter;
			}
		}
#endif

		// check markers for allocated slots
#ifdef VMEM_CHECK_ALLOCATED_MARKERS
		if(m_SlotSize > 2 * sizeof(void*))
		{
			void* p_start_slot = AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			void* p_end_slot = (byte*)p_start_slot + m_SlotSize * m_SlotsPerPage;
			for (void* p_slot_iter = p_start_slot; p_slot_iter != p_end_slot; p_slot_iter = (byte*)p_slot_iter + m_SlotSize)
			{
				unsigned int* p_mem = (unsigned int*)((byte*)p_slot_iter + sizeof(void*));
				unsigned int value = *p_mem;
				bool is_free_slot = value == VMEM_FREED_MEM || value == VMEM_TRAIL_GUARD_MEM;		// will miss allocated memory that is set to free mem but that's acceptable
				if (!is_free_slot)
					CheckGuards(p_slot_iter, m_Size, m_SlotSize);
			}
		}
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t FSA::WriteAllocs(int* p_allocs_written_count, int max_allocs_to_write, bool write_header) const
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(write_header)
		{
			DebugWrite(_T("-------------------------\n"));
			DebugWrite(_T("FSA %d\n"), m_Size);
		}

		bool empty = true;

		int allocs_written = p_allocs_written_count ? *p_allocs_written_count : 0;

		int alloc_count = 0;
		size_t bytes_allocated = 0;

		// write all allocs in the full pages
		for(FSAPageHeader* p_page=m_FullPageList.GetHead(); p_page!=m_FullPageList.GetIterEnd(); p_page=p_page->mp_Next)
		{
			const byte* p_slot = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			const byte* p_end_slot = p_slot + m_SlotsPerPage*m_SlotSize;
			for(; p_slot!=p_end_slot; p_slot+=m_SlotSize)
			{
				if(allocs_written < max_allocs_to_write)
				{
					WriteAlloc(p_slot, m_Size);
					++allocs_written;
					empty = false;
				}
				++alloc_count;
				bytes_allocated += m_Size;
			}
		}

		// write any allocs in the partially free pages
		const FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		for(const FSAPageHeader* p_page = m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
		{
			const byte* p_slot = (byte*)AlignUp((byte*)p_page + sizeof(FSAPageHeader), m_Alignment);
			const byte* p_end_slot = p_slot + m_SlotsPerPage*m_SlotSize;
			for(; p_slot!=p_end_slot; p_slot+=m_SlotSize)
			{
				// find out if this slot if on the free list
				bool allocated = true;
				for(void* p_iter=p_page->mp_FreeSlots; p_iter!=NULL; p_iter=*(void**)p_iter)
				{
					if(p_iter == p_slot)
					{
						allocated = false;
						break;
					}
				}

				if(allocated)
				{
					if(allocs_written < max_allocs_to_write)
					{
						WriteAlloc(p_slot, m_Size);
						++allocs_written;
					}
					++alloc_count;
					bytes_allocated += m_Size;
				}
			}
		}

		if(!empty)
		{
			if(!p_allocs_written_count)
			{
				if(allocs_written == max_allocs_to_write)
					DebugWrite(_T("Only showing first %d allocs\n"), max_allocs_to_write);

				DebugWrite(_T("%d bytes allocated across %d allocations\n"), bytes_allocated, alloc_count);
			}
		}

		if(p_allocs_written_count)
			*p_allocs_written_count = allocs_written;

		return bytes_allocated;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void FSA::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::FSAStats stats;
		stats.m_Size = m_Size;
		stats.m_SlotSize = m_SlotSize;
		stats.m_Alignment = m_Alignment;
		stats.m_Stats = m_Stats;

		stats.m_FreePageCount = 0;
		FSAPageHeader* p_end = m_FreePageList.GetIterEnd();
		for(FSAPageHeader* p_page=m_FreePageList.GetHead(); p_page!=p_end; p_page=p_page->mp_Next)
			++stats.m_FreePageCount;

		stats.m_FullPageCount = 0;
		for(FSAPageHeader* p_page=m_FullPageList.GetHead(); p_page!=m_FullPageList.GetIterEnd(); p_page=p_page->mp_Next)
			++stats.m_FullPageCount;

		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::AddAllocToStats()
	{
		VMEM_STATS(m_Stats.m_Unused -= m_SlotSize);
		VMEM_STATS(m_Stats.m_Used += m_Size);
		VMEM_STATS(m_Stats.m_Overhead += m_SlotSize - m_Size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_CACHE
	void FSA::RemoveAllocFromStats()
	{
		VMEM_STATS(m_Stats.m_Unused += m_SlotSize);
		VMEM_STATS(m_Stats.m_Used -= m_Size);
		VMEM_STATS(m_Stats.m_Overhead -= m_SlotSize - m_Size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_PAGE_CACHE
	void FSA::AddPageToEmptyList(FSAPageHeader* p_page)
	{
		// find where to insert into the list (sorted by page address)
		FSAPageHeader* p_iter = m_EmptyPageList.GetHead();
		FSAPageHeader* p_iter_end = m_EmptyPageList.GetIterEnd();
		while(p_iter != p_iter_end && p_iter < p_page)
			p_iter = p_iter->mp_Next;

		m_EmptyPageList.Insert(p_iter->mp_Prev, p_page);
		int empty_page_list_count = ++m_EmptyPageListCount;

		// remove a page if we have reached the limit
		VMEM_ASSERT(empty_page_list_count == m_EmptyPageList.GetCount(), "Empty page list has been corrupted");
		if(empty_page_list_count == VMEM_FSA_PAGE_CACHE_COUNT_MAX)
		{
			FSAPageHeader* p_last = m_EmptyPageList.RemoveTail();
			FreePage(p_last);
			--m_EmptyPageListCount;
		}
		else if(!m_InUpdateList && empty_page_list_count > VMEM_FSA_PAGE_CACHE_COUNT_MIN)
		{
			m_UpdateList.Add(this);
			m_InUpdateList = true;
		}
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// FSAHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	// note: this class doesn't need to be thread safe. Once it's setup it's read only.
	class FSAHeap
	{
	public:
		FSAHeap(PageHeap* p_page_heap, InternalHeap& internal_heap, int page_size);

		~FSAHeap();

		bool Initialise();

		bool InitialiseFSA(int size, int alignment);

		VMEM_FORCE_INLINE void* Alloc(size_t size);

		VMEM_FORCE_INLINE void Free(void* p);

		VMEM_FORCE_INLINE void Free(void* p, int page_size);

		void Flush();

#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
		void Update();
#endif
		void CheckIntegrity() const;

#ifdef VMEM_INC_INTEG_CHECK
		void IncIntegrityCheck();
#endif

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
		size_t WriteAllocs() const;
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif
		VMEM_FORCE_INLINE FSA* GetFSA(int size);

		void Lock() const;

		void Release() const;

	private:
		FSAHeap& operator=(const FSAHeap& other);

		//------------------------------------------------------------------------
		// data
	private:
		PageHeap* mp_PageHeap;

		static const int m_MaxFSASize = 16*1024;
		static const int m_MaxFSACount = m_MaxFSASize / VMEM_NATURAL_ALIGNMENT;

		FSA* m_FSAMap[m_MaxFSACount];

#ifdef VMEM_INC_INTEG_CHECK
		int m_FSAIntegCheckIndex;		// for incremental integrity checking
#endif
		InternalHeap& m_InternalHeap;

		Bitfield m_AllocatedFSAs;

		FSAUpdateList m_FSAUpdateList;

		int m_PageSize;
	};

	//------------------------------------------------------------------------
	void* FSAHeap::Alloc(size_t size)
	{
		VMEM_ASSERT(size >= 0, "invalid size");
		size_t index = (size + VMEM_NATURAL_ALIGNMENT - 1) / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "FSA index out of range");
		VMEM_ASSUME(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT(m_FSAMap[index], "FSA not setup for the specified size");

		return m_FSAMap[index]->Alloc();
	}

	//------------------------------------------------------------------------
	void FSAHeap::Free(void* p)
	{
		Free(p, mp_PageHeap->GetPageSize());
	}

	//------------------------------------------------------------------------
	// passing in page_size as an optimisaton so that it doesn't have to be looked up from the page heap
	void FSAHeap::Free(void* p, int page_size)
	{
		FSA* p_fsa = FSA::GetFSA(p, page_size);
		p_fsa->Free(p, page_size);
	}

	//------------------------------------------------------------------------
	FSA* FSAHeap::GetFSA(int size)
	{
		int index = (size + VMEM_NATURAL_ALIGNMENT - 1) / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "FSA index out of range");
		VMEM_ASSUME(index >= 0 && index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT(m_FSAMap[index], "FSA not setup for the specified size");

		return m_FSAMap[index];
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// FSAHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	FSAHeap::FSAHeap(PageHeap* p_page_heap, InternalHeap& internal_heap, int page_size)
	:	mp_PageHeap(p_page_heap),
#ifdef VMEM_INC_INTEG_CHECK
		m_FSAIntegCheckIndex(0),
#endif
		m_InternalHeap(internal_heap),
		m_AllocatedFSAs(m_MaxFSASize, internal_heap),
		m_PageSize(page_size)
	{
		memset(m_FSAMap, 0, sizeof(m_FSAMap));
	}

	//------------------------------------------------------------------------
	bool FSAHeap::Initialise()
	{
		return m_AllocatedFSAs.Initialise();
	}

	//------------------------------------------------------------------------
	FSAHeap::~FSAHeap()
	{
		if(m_AllocatedFSAs.Initialised())
		{
			for(int i=0; i<m_MaxFSACount; ++i)
			{
				if(m_AllocatedFSAs.Get(i))
					m_InternalHeap.Delete_WithFalseSharingBuffer(m_FSAMap[i]);
			}
		}
	}

	//------------------------------------------------------------------------
	bool FSAHeap::InitialiseFSA(int size, int alignment)
	{
		int index = size / VMEM_NATURAL_ALIGNMENT;
		VMEM_ASSERT(index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT, "increase m_MaxFSASize");
		VMEM_ASSUME(index < m_MaxFSASize/VMEM_NATURAL_ALIGNMENT);
		VMEM_ASSERT_MEM(!m_FSAMap[index], &m_FSAMap[index]);		// FSA size already setup?

		FSA* p_fsa = m_InternalHeap.New_WithFalseSharingBufferRef45<FSA>(
			size,
			alignment,
			mp_PageHeap,
			m_InternalHeap,
			m_FSAUpdateList,
			m_PageSize);

		if(!p_fsa)
			return false;

		if(!p_fsa->Initialise())
		{
			m_InternalHeap.Delete_WithFalseSharingBuffer(p_fsa);
			return false;
		}

		m_FSAMap[index] = p_fsa;
		m_AllocatedFSAs.Set(index);

		// set all the lower indices that haven't been setup up in map to use this fsa
		int i = index - 1;
		while(i >= 0 && !m_AllocatedFSAs.Get(i))
		{
			m_FSAMap[i] = p_fsa;
			--i;
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats FSAHeap::GetStats() const
	{
		Stats stats;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
			{
				stats += m_FSAMap[i]->GetStats();
				stats.m_Overhead += sizeof(FSA) + InternalHeap::GetFalseSharingBufferOverhead();
			}
		}

		stats.m_Overhead += m_AllocatedFSAs.GetSizeInBytes();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats FSAHeap::GetStatsNoLock() const
	{
		Stats stats;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
			{
				stats += m_FSAMap[i]->GetStatsNoLock();
				stats.m_Overhead += sizeof(FSA) + InternalHeap::GetFalseSharingBufferOverhead();
			}
		}

		stats.m_Overhead += m_AllocatedFSAs.GetSizeInBytes();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::Flush()
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Flush();
		}
	}

	//------------------------------------------------------------------------
#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
	void FSAHeap::Update()
	{
		// Note: Thread safely is not obvious here.
		// This is safe because only FSA's can add and remove themselves to/from the list,
		// and this will always be done inside of the FSA critical sction. An FSA will only
		// add itself if it isn't already added. The FSA is still part of the list after 
		// GetAndClearList is called so it won't be added again. It is removed from the
		// list in the update, and any new FSA's that are added while this is Update happening
		// will go onto the main m_FSAUpdateList.
		FSA* p_fsa = m_FSAUpdateList.GetAndClearList();
		while(p_fsa)
		{
			FSA* p_next = p_fsa->GetNextUpdateFSA();
			p_fsa->Update();
			p_fsa = p_next;
		}
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::CheckIntegrity() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->CheckIntegrity();
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	void FSAHeap::IncIntegrityCheck()
	{
		int i = 0;
		while(!m_FSAMap[m_FSAIntegCheckIndex] && i++ < m_MaxFSACount)
			m_FSAIntegCheckIndex = (m_FSAIntegCheckIndex + 1) % m_MaxFSACount;

		FSA* p_fsa = m_FSAMap[m_FSAIntegCheckIndex];
		if(p_fsa)
			p_fsa->CheckIntegrity();

		m_FSAIntegCheckIndex = (m_FSAIntegCheckIndex + 1) % m_MaxFSACount;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t FSAHeap::WriteAllocs() const
	{
		size_t allocated_bytes = 0;
		int allocs_written = 0;

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				allocated_bytes += m_FSAMap[i]->WriteAllocs(&allocs_written, VMEM_WRITE_ALLOCS_MAX, false);
		}

		if(allocs_written == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocs\n"), VMEM_WRITE_ALLOCS_MAX);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void FSAHeap::Lock() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Lock();
		}
	}

	//------------------------------------------------------------------------
	void FSAHeap::Release() const
	{
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->Release();
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void FSAHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		SendEnumToMemPro(vmem_FSAHeap, send_fn, p_context);

		MemProStats::FSAHeapStats stats;

		stats.m_FSACount = 0;
		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				++stats.m_FSACount;
		}
		stats.m_Stats = GetStats();
		SendToMemPro(stats, send_fn, p_context);

		for(int i=0; i<m_MaxFSACount; ++i)
		{
			if(m_AllocatedFSAs.Get(i))
				m_FSAMap[i]->SendStatsToMemPro(send_fn, p_context);
		}
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// InternalHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	const int g_InternalHeapRegionSize = 16*1024;

	//------------------------------------------------------------------------
	InternalHeap::InternalHeap(VirtualMem& virtual_mem)
	:	m_BasicCoalesceHeap(g_InternalHeapRegionSize, virtual_mem)
	{
	}

	//------------------------------------------------------------------------
#if VMEM_SIMULATE_OOM
	bool InternalHeap::SimulateOOM()
	{
		int simulate_oom_count = GetInternalHeapSimulateOOMCount();

		if(simulate_oom_count > 0)
		{
			if(simulate_oom_count == 1)
				return true;
			else
				SetInternalHeapSimulateOOMCount(simulate_oom_count - 1);
		}

		return false;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// LargeHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class VirtualMem;

	//------------------------------------------------------------------------
	class LargeHeap
	{
	public:
		LargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem);

		~LargeHeap();

		bool Initialise();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p) const;

		bool Owns(void* p) const;

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
		size_t WriteAllocs();
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif

		void AcquireLockShared() const;

		void ReleaseLockShared() const;

	private:
		LargeHeap(const LargeHeap&);
		void operator=(const LargeHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		mutable VMem::ReadWriteLock m_ReadWriteLock;

		HashMap<AddrKey, size_t> m_Allocs;

		int m_ReserveFlags;
		int m_CommitFlags;

		VirtualMem& m_VirtualMem;

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// LargeHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	bool LargeHeap::Initialise()
	{
		return m_Allocs.Initialise();
	}

	//------------------------------------------------------------------------
	LargeHeap::LargeHeap(int reserve_flags, int commit_flags, VirtualMem& virtual_mem)
	:	m_Allocs(virtual_mem.GetPageSize()),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_VirtualMem(virtual_mem)
	{
	}

	//------------------------------------------------------------------------
	LargeHeap::~LargeHeap()
	{
		if(m_Allocs.Initialised())
		{
			HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
			while(iter.MoveNext())
			{
				void* p = (void*)iter.GetKey().GetAddr();
				Free(p);
			}
		}
	}

	//------------------------------------------------------------------------
	void* LargeHeap::Alloc(size_t size)
	{
		int page_size = m_VirtualMem.GetPageSize();
		size_t aligned_size = AlignSizeUpPow2(size, page_size);

		WriteLockScope lock(m_ReadWriteLock);

		void* p = m_VirtualMem.Reserve(aligned_size, page_size, m_ReserveFlags);
		if(!p)
			return NULL;

		if(!m_VirtualMem.Commit(p, aligned_size, m_CommitFlags))
		{
			m_VirtualMem.Release(p);
			return NULL;
		}

		if(!m_Allocs.Add(p, size))
		{
			m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
			m_VirtualMem.Release(p);
			return NULL;		// Out of memory
		}

		VMEM_MEMSET(p, VMEM_ALLOCATED_MEM, aligned_size);

		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Unused += aligned_size - size);
		VMEM_STATS(m_Stats.m_Reserved += aligned_size);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		VMEM_ASSERT((((size_t)p) & (page_size-1)) == 0, "this allocator should align to page size");

		return p;
	}

	//------------------------------------------------------------------------
	bool LargeHeap::Free(void* p)
	{
		WriteLockScope lock(m_ReadWriteLock);

		size_t alloc_size = 0;
		AddrKey key(p);
		if(!m_Allocs.TryGetValue(key, alloc_size))
			return false;

		int page_size = m_VirtualMem.GetPageSize();
		size_t aligned_size = AlignSizeUpPow2(alloc_size, page_size);

		m_VirtualMem.Decommit(p, aligned_size, m_CommitFlags);
		m_VirtualMem.Release(p);

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= alloc_size);
		VMEM_STATS(m_Stats.m_Unused -= aligned_size - alloc_size);
		VMEM_STATS(m_Stats.m_Reserved -= aligned_size);

		m_Allocs.Remove(key);

		return true;
	}

	//------------------------------------------------------------------------
	size_t LargeHeap::GetSize(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		size_t size = 0;
		if(m_Allocs.TryGetValue(p, size))
			return size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool LargeHeap::Owns(void* p) const
	{
		ReadLockScope lock(m_ReadWriteLock);

		size_t size = 0;
		return m_Allocs.TryGetValue(p, size);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t LargeHeap::WriteAllocs()
	{
		ReadLockScope lock(m_ReadWriteLock);

		DebugWrite(_T("-------------------------\n"));
		DebugWrite(_T("LargeHeap\n"));

		int written_allocs = 0;
		int alloc_count = 0;
		size_t allocated_bytes = 0;

		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p = (void*)iter.GetKey().GetAddr();
			size_t size = iter.GetValue();

			if(written_allocs != VMEM_WRITE_ALLOCS_MAX)
			{
				WriteAlloc(p, size);
				++written_allocs;
			}
			++alloc_count;
			allocated_bytes += size;
		}

		if(written_allocs == VMEM_WRITE_ALLOCS_MAX)
			DebugWrite(_T("Only showing first %d allocations\n"), VMEM_WRITE_ALLOCS_MAX);

		DebugWrite(_T("%d bytes allocated across %d allocations\n"), allocated_bytes, alloc_count);

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void LargeHeap::AcquireLockShared() const
	{
		m_ReadWriteLock.EnterShared();
	}

	//------------------------------------------------------------------------
	void LargeHeap::ReleaseLockShared() const
	{
		m_ReadWriteLock.LeaveShared();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void LargeHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		ReadLockScope lock(m_ReadWriteLock);

		MemProStats::LageHeapStats stats;

		stats.m_AllocCount = 0;
		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
			++stats.m_AllocCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_LargeHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats LargeHeap::GetStats() const
	{
		ReadLockScope lock(m_ReadWriteLock);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats LargeHeap::GetStatsNoLock() const
	{
		int map_size = m_Allocs.GetAllocedMemory();

		Stats internal_stats;
		internal_stats.m_Reserved += map_size;
		internal_stats.m_Overhead += map_size;

		return m_Stats + internal_stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PageHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct PageHeapRegion
	{
		PageHeapRegion(int page_count, InternalHeap& internal_heap)
		:	mp_Mem(NULL),
			m_Bitfield(page_count, internal_heap),
			mp_Next(NULL)
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			,m_CommittedPagesBitfield(page_count, internal_heap)
#endif
		{}

		bool Initialise()
		{
			if(!m_Bitfield.Initialise())
				return false;

#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			if(!m_CommittedPagesBitfield.Initialise())
				return false;
#endif
			return true;
		}

		void* mp_Mem;
		Bitfield m_Bitfield;
		PageHeapRegion* mp_Next;
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		Bitfield m_CommittedPagesBitfield;
#endif
	};

	//------------------------------------------------------------------------
	PageHeap::PageHeap(
		int page_size,
		int region_size,
		InternalHeap& internal_heap,
		HeapRegions& heap_regions,
		RegionType::Enum region_type,
		int reserve_flags,
		int commit_flags,
		VirtualMem& virtual_mem)
	:	
		m_PageSize(page_size),
		m_SysPageSize(VMem::VMax(page_size, virtual_mem.GetPageSize())),
		m_RegionSize(AlignUpPow2(region_size, m_PageSize)),
		m_PagesPerRegion(region_size / m_PageSize),
		m_PagesPerSysPage(m_SysPageSize / page_size),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		mp_RegionList(NULL),
		m_HeapRegions(heap_regions),
		m_RegionType(region_type),
		m_InternalHeap(internal_heap),
		m_VirtualMem(virtual_mem)
	{
		VMEM_ASSERT((m_RegionSize % virtual_mem.GetPageSize()) == 0, "regionsize must be a multiple of the system page size");
		VMEM_ASSERT((m_SysPageSize % page_size) == 0, "page size must be a multple of the system page size or vice versa");
		VMEM_ASSERT((m_SysPageSize % virtual_mem.GetPageSize()) == 0, "page size must be a multple of the system page size or vice versa");
	}

	//------------------------------------------------------------------------
	bool PageHeap::Initialise()
	{
		return CreateRegion() != NULL;
	}

	//------------------------------------------------------------------------
	PageHeap::~PageHeap()
	{
		while(mp_RegionList)
		{
			PageHeapRegion* p_region = mp_RegionList;
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void* PageHeap::GetFirstRegion()
	{
		return mp_RegionList->mp_Mem;
	}

	//------------------------------------------------------------------------
	void* PageHeap::Alloc()
	{
		CriticalSectionScope lock(m_CriticalSection);

		// find the first region that has a free page
		PageHeapRegion* p_region = mp_RegionList;
		while(p_region && p_region->m_Bitfield.GetCount() == m_PagesPerRegion)
			p_region = p_region->mp_Next;

		if(!p_region)
			p_region = CreateRegion();

		if(!p_region)
			return NULL;

		int page_index = p_region->m_Bitfield.GetFirstClearBit();
		void* p_page = (byte*)p_region->mp_Mem + page_index * m_PageSize;

		// commit the sys page if it's clear (there can be multiple pages in a system page)
		if(SysPageClear(page_index, p_region))
		{
			if(!m_VirtualMem.Commit(AlignDownPow2(p_page, m_VirtualMem.GetPageSize()), m_SysPageSize, m_CommitFlags))
			{
				p_region->m_Bitfield.Set(page_index);	// must call Set after calling GetFirstClearBit
				p_region->m_Bitfield.Clear(page_index);
				return NULL;		// out of memory
			}

			VMEM_STATS(m_Stats.m_Unused += m_SysPageSize);

			for(int i=0; i<m_PagesPerSysPage; ++i)
				VMEM_MEMSET((byte*)p_page + i*m_PageSize, VMEM_UNUSED_PAGE, m_PageSize);
		}

#ifdef VMEM_ENABLE_MEMSET
		CheckMemory(p_page, m_PageSize, VMEM_UNUSED_PAGE);
#endif

		p_region->m_Bitfield.Set(page_index);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		p_region->m_CommittedPagesBitfield.Set(page_index);
#endif

		VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
		VMEM_STATS(m_Stats.m_Used += m_PageSize);

		return p_page;
	}

	//------------------------------------------------------------------------
	void PageHeap::Free(void* p_page)
	{
		CriticalSectionScope lock(m_CriticalSection);

		// get the region
		PageHeapRegion* p_region = GetRegion(p_page);
		VMEM_ASSERT(p_region, "can't find region for page");
		VMEM_ASSUME(p_region);

		// clear the bitfield bit
		int index = ToInt(((byte*)p_page - (byte*)p_region->mp_Mem) / m_PageSize);
		p_region->m_Bitfield.Clear(index);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		p_region->m_CommittedPagesBitfield.Clear(index);
#endif

		VMEM_STATS(m_Stats.m_Unused += m_PageSize);
		VMEM_STATS(m_Stats.m_Used -= m_PageSize);

		// decommit the sys page if it's clear (there can be multiple pages in a system page)
		if(SysPageClear(index, p_region))
		{
			m_VirtualMem.Decommit(AlignDownPow2(p_page, m_VirtualMem.GetPageSize()), m_SysPageSize, m_CommitFlags);
			VMEM_STATS(m_Stats.m_Unused -= m_SysPageSize);
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
			// don't re-use these pages if we are looking for corruptions
			int start_index = AlignDownPow2(index, m_PagesPerSysPage);
			int end_index = start_index + m_PagesPerSysPage;
			for(int i=start_index; i<end_index; ++i)
				p_region->m_Bitfield.Set(i);
#endif
		}
		else
		{
			VMEM_MEMSET(p_page, VMEM_UNUSED_PAGE, m_PageSize);
		}

		if(!p_region->m_Bitfield.GetCount() && p_region != mp_RegionList)		// do not destroy the first region
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	// create the region and add it to the regions list
	PageHeapRegion* PageHeap::CreateRegion()
	{
		PageHeapRegion* p_region = m_InternalHeap.NewRef2<PageHeapRegion>(m_PagesPerRegion, m_InternalHeap);
		if(!p_region)
			return NULL;

		if(!p_region->Initialise())
		{
			m_InternalHeap.Delete(p_region);
			return NULL;
		}

		p_region->mp_Mem = m_VirtualMem.Reserve(m_RegionSize, m_SysPageSize, m_ReserveFlags);
		if(!p_region->mp_Mem)
		{
			m_InternalHeap.Delete(p_region);
			return NULL;
		}

		p_region->mp_Next = NULL;

		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Overhead += sizeof(PageHeapRegion));
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_Bitfield.GetSizeInBytes());
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		VMEM_STATS(m_Stats.m_Overhead += p_region->m_CommittedPagesBitfield.GetSizeInBytes());
#endif

#if defined(VMEM_ALTERNATE_PAGES)
		int sys_pages_per_region = m_RegionSize / m_SysPageSize;
		for(int i=1; i<sys_pages_per_region; i+=2)		// start from 1 so that we always have at least one valid page
		{
			for(int a=0; a<m_PagesPerSysPage; ++a)
				p_region->m_Bitfield.Set(i*m_PagesPerSysPage + a);
		}
#endif
		// add to heap regions array
		if(mp_RegionList)				// don't add the first region to the heap regions, it is handled differently to avoid locks
		{
		#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			if(!m_HeapRegions.AddRegion(this, p_region->mp_Mem, m_RegionSize, m_RegionType))
		#else
			if(!m_HeapRegions.AddRegion(p_region->mp_Mem, m_RegionSize, m_RegionType))
		#endif
			{
				VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
				VMEM_STATS(m_Stats.m_Overhead -= sizeof(PageHeapRegion));
				VMEM_STATS(m_Stats.m_Overhead -= p_region->m_Bitfield.GetSizeInBytes());
				m_VirtualMem.Release(p_region->mp_Mem);
				m_InternalHeap.Delete(p_region);
				return NULL;
			}
		}

		// add to end of region list
		PageHeapRegion* p_iter = mp_RegionList;
		if(p_iter)
		{
			while(p_iter->mp_Next)
				p_iter = p_iter->mp_Next;
			p_iter->mp_Next = p_region;
		}
		else
		{
			mp_RegionList = p_region;
		}

		return p_region;
	}

	//------------------------------------------------------------------------
	void PageHeap::UnlinkRegion(PageHeapRegion* p_region)
	{
		if(p_region != mp_RegionList)
			m_HeapRegions.RemoveRegion(p_region->mp_Mem);

		// unlink from region list
		if(mp_RegionList == p_region)
		{
			mp_RegionList = p_region->mp_Next;
		}
		else
		{
			VMEM_ASSERT(mp_RegionList, "unable to find region");
			VMEM_ASSUME(mp_RegionList);
			PageHeapRegion* p = mp_RegionList;
			while(p->mp_Next != p_region)
			{
				p = p->mp_Next;
				VMEM_ASSERT(p, "unable to find region");
				VMEM_ASSUME(p);
			}
			p->mp_Next = p_region->mp_Next;
		}
	}

	//------------------------------------------------------------------------
	void PageHeap::DestroyRegion(PageHeapRegion* p_region)
	{
#if defined(VMEM_ALTERNATE_PAGES)
		int sys_pages_per_region = m_RegionSize / m_SysPageSize;
		for(int i=1; i<sys_pages_per_region; i+=2)		// remember to start from 1
		{
			for(int a=0; a<m_PagesPerSysPage; ++a)
				p_region->m_Bitfield.Clear(i*m_PagesPerSysPage + a);
		}
#endif

		// decommit pages
		byte* p_page = (byte*)p_region->mp_Mem;
		for(int i=0; i<m_PagesPerRegion; i+=m_PagesPerSysPage, p_page+=m_SysPageSize)
		{
			if(!SysPageClear(i, p_region))
				m_VirtualMem.Decommit(p_page, m_SysPageSize, m_CommitFlags);
		}

		// release the memory
		m_VirtualMem.Release(p_region->mp_Mem);

		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(PageHeapRegion));
		VMEM_STATS(m_Stats.m_Overhead -= p_region->m_Bitfield.GetSizeInBytes());

		m_InternalHeap.Delete(p_region);
	}

	//------------------------------------------------------------------------
	PageHeapRegion* PageHeap::GetRegion(void* p) const
	{
		for(PageHeapRegion* p_region=mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			void* p_mem = p_region->mp_Mem;
			if(p >= p_mem && p < (byte*)p_mem + m_RegionSize)
				return p_region;
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	// we can always assume that the index passed in is clear, which covers the
	// case of m_PagesPerSysPage being zero.
	bool PageHeap::SysPageClear(int index, PageHeapRegion* p_region) const
	{
#ifdef VMEM_MEMORY_CORRUPTION_FINDER
		const Bitfield& bitfield = p_region->m_CommittedPagesBitfield;
#else
		const Bitfield& bitfield = p_region->m_Bitfield;
#endif

		int start_index = AlignDownPow2(index, m_PagesPerSysPage);
		for(int i=0; i<m_PagesPerSysPage; ++i)
		{
			if(bitfield.Get(start_index + i))
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void PageHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		CriticalSectionScope lock(m_CriticalSection);

		MemProStats::PageHeapStats stats;
		stats.m_PageSize = m_PageSize;
		stats.m_RegionSize = m_RegionSize;

		stats.m_RegionCount = 0;
		for(PageHeapRegion* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
			++stats.m_RegionCount;

		stats.m_Stats = m_Stats;

		SendEnumToMemPro(vmem_PageHeap, send_fn, p_context);
		SendToMemPro(stats, send_fn, p_context);
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PhysicalHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class PhysicalHeap
	{
		// the header is located immediately before the allocation or at the start of the free block in the coalesce heap
		struct Header
		{
#if VMEM_DEBUG_LEVEL >= 1
			unsigned int m_Marker[4];
#endif
			Header* mp_Prev;		// the prev and next free block of memory.
			Header* mp_Next;		// if the header is allocated prev and next pointers will be null.
			int m_Size;				// the size includes the size of the header
			int m_PrevSize;
#ifdef VMEM_X64
			int m_Padding[2];
#endif
		};

		// the coalesce heap region
		struct Region
		{
			Header m_FreeList;
			Region* mp_Next;
#ifdef VMEM_X64
			int m_Padding[2];
#else
			int m_Padding[3];
#endif
		};

		struct PhysicalAllocInfo
		{
			void* mp_Mem;
			size_t m_Size;
		};

		class PhysicalAllocArray
		{
		public:
			PhysicalAllocArray();

			~PhysicalAllocArray();

			bool Add(void* p, size_t size);

			bool Remove(void* p, size_t& size);

			size_t GetSize(void* p);

			bool Contains(void* p);

			int GetMemorySize() const { return m_Capacity * sizeof(PhysicalAllocInfo); }

			int GetCount() const { return m_Count; }

			PhysicalAllocInfo operator[](int index) const
			{
				VMEM_ASSERT(index >= 0 && index < m_Count, "VMem index out of bound in PhysicalAllocArray");
				return mp_Array[index];
			}

		private:
			bool Grow();

			// data
			PhysicalAllocInfo* mp_Array;
			int m_Capacity;
			int m_Count;
		};

		//------------------------------------------------------------------------
	public:
		PhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags);

		~PhysicalHeap();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p);

		bool Owns(void* p);

		void* AllocAligned(size_t size, size_t alignment);

		bool FreeAligned(void* p);

		size_t GetSizeAligned(void* p);

		void CheckIntegrity();

		void DebugWrite() const;

		void Trim();

		size_t GetMaxAllocSize() const { return m_CoalesceHeapMaxSize; }

		void WriteStats();

	private:
		Region* CreateRegion();

		void DestroyRegion(Region* p_region);

		void UnlinkRegion(Region* p_region);

		static void DebugWrite(Header* p_header);

		static void DebugWrite(const PhysicalAllocArray& array);

		void ClearRegion(Region* p_region);

		bool CoalesceOwns(void* p) const;

		static inline void Unlink(Header* p_header);

		static void InitialiseHeader(Header* p_header);

		static void CheckHeader(Header* p_header);

		void* PhysicalAllocDirect(size_t size);

		bool PhysicalFreeDirect(void* p);

		void* AllocatePhysical(size_t size);

		void PhysicalFree(void* p);

		static void* IntrnalAlloc(size_t size);

		static void InternalFree(void* p);

		void* CoalesceAlloc(size_t size);

		bool CoalesceFree(void* p);

		void* PhysicalAllocDirectAligned(size_t size);

		bool PhysicalFreeDirectAligned(void* p);

		bool TrackPhysicalAlloc(void* p, size_t size);

		bool TrackPhysicalAllocAligned(void* p, size_t size);

		bool CoalesceEmpty() const;

		size_t GetSize_NoLock(void* p);

		static void InitialiseAlignmentHeader(struct PhysicalAlignHeader* p_header);

		static void CheckAlignmentHeader(struct PhysicalAlignHeader* p_header);

		//------------------------------------------------------------------------
		// data
	private:
		Region* mp_RegionList;			// the coalesce heap regions
		int m_RegionSize;				// the size of each coalesce region.
		size_t m_CoalesceHeapMaxSize;	// allocations larger than this size get allocated with the platform physical alloc function directly

#ifdef VMEM_PLATFORM_XBOX360
	#error Please contact slynch@puredevsoftware.com for this platform
#endif
		mutable CriticalSection m_CriticalSection;

		PhysicalAllocArray m_PhysicalAllocs;			// allocations too big for the coalesce heap that don't have an alignment header
		PhysicalAllocArray m_AlignedPhysicalAllocs;		// allocations too big for the coalesce heap that do have an alignment header

		// debug stuff to make sure that everything is cleaned up in the destructor
#if VMEM_DEBUG_LEVEL >= 1
		int m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			size_t m_PhysicalAllocTotal;
		#endif
#endif

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
		size_t m_PhysicalAllocDirectTotalSize;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
//
// PhysicalHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
#define VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER	0xbeebbeeb
#define VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER 0xa1a1a1a1
#define VMEM_PHYSICAL_PAGE_SIZE 4096

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	const size_t g_PhysicalHeapAlignment = 16;

	//------------------------------------------------------------------------
	struct PhysicalAlignHeader
	{
		void* p;
#ifdef VMEM_X64
		int m_Padding[2];
#else
		int m_Padding[3];
#endif
	};

	//------------------------------------------------------------------------
	void PhysicalHeap_DebugWriteMem(size_t size)
	{
		float mb = (float)(size/1024.0/1024.0);
#ifdef VMEM_X64
		DebugWrite(_T("%10lld (%0.1fMB)"), size, mb);
#else
		DebugWrite(_T("%10d (%0.1fMB)"), size, mb);
#endif

		if(mb < 10.0f)			DebugWrite(_T("   "));
		else if(mb < 100.0f)	DebugWrite(_T("  "));
		else if(mb < 1000.0f)	DebugWrite(_T(" "));
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
	:	mp_RegionList(NULL),
		m_RegionSize(ToInt(coalesce_heap_region_size)),
		m_CoalesceHeapMaxSize(coalesce_heap_max_size)
#ifdef VMEM_PLATFORM_XBOX360
	#error Please contact slynch@puredevsoftware.com for this platform
#endif
#if VMEM_DEBUG_LEVEL >= 1
		,m_PhysicalAllocCount(0)
		#ifdef VMEM_PLATFORM_WIN
			,m_PhysicalAllocTotal(0)
		#endif
#endif
	{
#ifndef VMEM_PLATFORM_XBOX360
		VMEM_UNREFERENCED_PARAM(flags);
#endif
		VMEM_STATIC_ASSERT((sizeof(Region) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap Region not aligned correctly");
		VMEM_STATIC_ASSERT((sizeof(Header) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap Header not aligned correctly");
		VMEM_STATIC_ASSERT((sizeof(PhysicalAlignHeader) % g_PhysicalHeapAlignment) == 0, "PhysicalHeap PhysicalAlignHeader not aligned correctly");

		VMEM_STATS(m_PhysicalAllocDirectTotalSize = 0);

		VMEM_ASSERT(m_CoalesceHeapMaxSize < m_RegionSize - sizeof(Region) - 2 * sizeof(Header), "coalesce_heap_max_size must be smaller than region size");
	}

	//------------------------------------------------------------------------
	PhysicalHeap::~PhysicalHeap()
	{
		if(!CoalesceEmpty() || m_PhysicalAllocs.GetCount() || m_AlignedPhysicalAllocs.GetCount())
		{
			VMem::DebugWrite(_T("ERROR: VMem PhysicalHeap Memory Leak"));
			DebugWrite();
			VMEM_ASSERT(false, "VMem PhysicalHeap Memory Leak");
		}

		Trim();

#if VMEM_DEBUG_LEVEL >= 1
		VMEM_ASSERT(m_PhysicalAllocCount == 0, "Internal leak in VMem PhysicalHeap");
		#ifdef VMEM_PLATFORM_WIN
			VMEM_ASSERT(m_PhysicalAllocTotal == 0, "Internal leak in VMem PhysicalHeap");
		#endif
#endif
	}

	//------------------------------------------------------------------------
	PhysicalHeap::Region* PhysicalHeap::CreateRegion()
	{
		// commit the region memory
		// the region struct is at the start of the committed memory
		Region* p_region = (Region*)AllocatePhysical(m_RegionSize);

		VMEM_STATS(m_Stats.m_Reserved += m_RegionSize);
		VMEM_STATS(m_Stats.m_Unused += m_RegionSize - sizeof(Region));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Region));

		if(!p_region)
			return NULL;		// out of memory

		// initialise the region
		p_region->mp_Next = NULL;

		// initialise the free list header
		Header& free_list_head = p_region->m_FreeList;
		memset(&free_list_head, 0, sizeof(Header));

		// setup first header 'allocated' header. This header is never used
		// but makes the coalesce logic a simpler.
		Header* p_first_header = (Header*)(p_region + 1);
		InitialiseHeader(p_first_header);
		p_first_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_first_header->mp_Next = NULL;
		p_first_header->m_Size = sizeof(Header);
		p_first_header->m_PrevSize = 0;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		// setup free header and link it to the first header
		Header* p_header = p_first_header + 1;
		InitialiseHeader(p_header);
		p_header->mp_Prev = &free_list_head;		// circular linked list
		p_header->mp_Next = &free_list_head;
		int total_free_size = ToInt(m_RegionSize - sizeof(Region) - 2 * sizeof(Header));
		p_header->m_Size = total_free_size;			// includes size of the header
		p_header->m_PrevSize = sizeof(Header);
		free_list_head.mp_Prev = p_header;
		free_list_head.mp_Next = p_header;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		// setup last header guard
		Header* p_last_header = (Header*)((byte*)p_header + p_header->m_Size);
		InitialiseHeader(p_last_header);
		p_last_header->mp_Prev = NULL;				// NULL prev and next pointers mean header is allocated
		p_last_header->mp_Next = NULL;
		p_last_header->m_Size = sizeof(Header);
		p_last_header->m_PrevSize = total_free_size;
		VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));

		return p_region;
	}

	//------------------------------------------------------------------------
	// WARNING: this deletes p_region, don't use p_region after calling this function.
	// This function calls the standard Free function on each alloc in the region until
	// the region disappears. It's slightly messy, but avoids duplicating the code in
	// the Free function.
	void PhysicalHeap::ClearRegion(Region* p_region)
	{
		// get the first alloc that we can free
		Header* p_header = (Header*)(p_region + 1) + 1;

		// first we work out how many bytes have been allocated so that we
		// know when we are done. This is needed because the last Free call
		// will delete the region and all headers, so we need a simple way
		// of telling when this happens.
		int size = 0;
		Header* p_end_header = (Header*)((byte*)p_region + m_RegionSize) - 1;
		for(Header* p_h = p_header; p_h != p_end_header; p_h=(Header*)((byte*)p_h + p_h->m_Size))
		{
			VMEM_ASSERT_CODE(CheckHeader(p_h));

			if(!p_h->mp_Prev)
				size += p_h->m_Size;
		}

		// now keep deleting the first allocated block in the list until all
		// allocations are freed (which frees the region)
		while(size)
		{
			Header* p_alloc_header = p_header;

			// if p_header is free the next header is garanteed to be allocated
			if(p_alloc_header->mp_Prev)
				p_alloc_header = (Header*)((byte*)p_alloc_header + p_alloc_header->m_Size);
			VMEM_ASSERT(!p_alloc_header->mp_Prev, "expected block to be allocated");

			size -= p_alloc_header->m_Size;
			VMEM_ASSERT(size >= 0, "size negative");

			// warning: this deletes p_region when it's empty, so be careful not to access p_region after last free
			void* p = p_alloc_header + 1;
			Free(p);
		}
	}

	//------------------------------------------------------------------------
	// releases the memory associated with the region. Do not use p_region after this.
	void PhysicalHeap::DestroyRegion(Region* p_region)
	{
		// check it's empty
		#if VMEM_DEBUG_LEVEL >= 1
			int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
			Header* p_header = (Header*)(p_region + 1) + 1;
			VMEM_ASSERT(p_header->m_Size == total_free_size, "Destroying non-empty region");
		#endif

		VMEM_STATS(m_Stats.m_Unused -= m_RegionSize - sizeof(Region) - 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Overhead -= sizeof(Region) + 3*sizeof(Header));
		VMEM_STATS(m_Stats.m_Reserved -= m_RegionSize);

		// release memory
		PhysicalFree(p_region);
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::AllocAligned(size_t size, size_t alignment)
	{
		CriticalSectionScope lock(m_CriticalSection);

		size_t aligned_size = AlignUp(size + sizeof(PhysicalAlignHeader), g_PhysicalHeapAlignment);

		if(aligned_size >= m_CoalesceHeapMaxSize && (alignment == 0 || (VMEM_PHYSICAL_PAGE_SIZE % alignment) == 0))
		{
			void* p = PhysicalAllocDirect(size);

			if(!p || !TrackPhysicalAlloc(p, size))
				return NULL;

			return p;
		}
		else
		{
			if(alignment != g_PhysicalHeapAlignment)
				aligned_size += alignment;

			if(aligned_size < m_CoalesceHeapMaxSize)
			{
				void* p = CoalesceAlloc(aligned_size);
				if(!p)
					return NULL;		// out of memory

				void* aligned_p = AlignUp((byte*)p + sizeof(PhysicalAlignHeader), alignment);

				PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)aligned_p - 1;
				InitialiseAlignmentHeader(p_header);
				p_header->p = p;
				return aligned_p;
			}
			else
			{
				void* p = PhysicalAllocDirectAligned(aligned_size);
				if(!p)
					return NULL;		// out of memory

				void* aligned_p = AlignUp((byte*)p + sizeof(PhysicalAlignHeader), alignment);

				PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)aligned_p - 1;
				InitialiseAlignmentHeader(p_header);
				p_header->p = p;

				if(!TrackPhysicalAllocAligned(aligned_p, size))
					return NULL;		// out of memory

				return aligned_p;
			}
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::Alloc(size_t size)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(size < m_CoalesceHeapMaxSize)
		{
			return CoalesceAlloc(size);
		}
		else
		{
			void* p = PhysicalAllocDirect(size);

			if(!p || !TrackPhysicalAlloc(p, size))
				return NULL;

			return p;
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::CoalesceAlloc(size_t size)
	{
		VMEM_ASSERT(size < m_CoalesceHeapMaxSize, "invalid size passed in to PhysicalHeap::CoalesceAlloc");

		int aligned_size = AlignUp(ToInt(size), ToInt(g_PhysicalHeapAlignment));

		VMEM_STATIC_ASSERT((sizeof(Header) & (g_PhysicalHeapAlignment-1)) == 0, "PhysicalHeap sizeof(Header) not aligned correctly");

		// work out the size of the block
		int total_size = aligned_size + sizeof(Header);

		#if VMEM_DEBUG_LEVEL >= 1
			int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
			VMEM_ASSERT(total_size < total_free_size, "allocation bigger than region size");
		#endif

		// get the first region (or create one)
		Region* p_region = mp_RegionList;
		if(!p_region)
			p_region = mp_RegionList = CreateRegion();

		// linear search through the regions for a big enough free block
		while(p_region)
		{
			Header* p_list_head = &p_region->m_FreeList;
		
			// linear search through the free block list for a big enough block
			for(Header* p_header=p_list_head->mp_Next; p_header != p_list_head; p_header=p_header->mp_Next)
			{
				VMEM_ASSERT_CODE(CheckHeader(p_header));
				VMEM_ASSERT(p_header->mp_Prev, "header already allocated");

				// work out what the offcut would be
				int offcut_size = p_header->m_Size - total_size;

				// if we have found an allocation big enough
				if(offcut_size >= 0)
				{
					Header* p_prev_free_header = p_header->mp_Prev;
					Header* p_next_free_header = p_header->mp_Next;

					// ignore offcut if too small
					if(offcut_size < sizeof(Header))
					{
						total_size += offcut_size;
						offcut_size = 0;
					}

					// get the next header
					Header* p_next_header = (Header*)((byte*)p_header + p_header->m_Size);
					VMEM_ASSERT_CODE(CheckHeader(p_next_header));
					if(offcut_size)
					{
						// create a new header for the offcut
						Header* p_offcut_header = (Header*)((byte*)p_header + total_size);
						VMEM_ASSERT_CODE(InitialiseHeader(p_offcut_header));
						p_offcut_header->m_Size = offcut_size;
						p_offcut_header->m_PrevSize = total_size;

						// link new offcut header into free list
						p_prev_free_header->mp_Next = p_offcut_header;
						p_offcut_header->mp_Prev = p_prev_free_header;
						p_offcut_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_offcut_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = offcut_size;

						VMEM_STATS(m_Stats.m_Unused -= sizeof(Header));
						VMEM_STATS(m_Stats.m_Overhead += sizeof(Header));
					}
					else
					{
						// no offcut, so remove the original header from the free list
						p_prev_free_header->mp_Next = p_next_free_header;
						p_next_free_header->mp_Prev = p_prev_free_header;

						// update the prev size of the next header in memory
						p_next_header->m_PrevSize = total_size;
					}

					// setup the header for this alloc
					p_header->mp_Prev = NULL;		// NULL prev and next pointers mean header is allocated
					p_header->mp_Next = NULL;
					p_header->m_Size = total_size;

					void* p = p_header + 1;
					memset(p, VMEM_ALLOCATED_MEM, aligned_size);

					VMEM_STATS(m_Stats.m_Unused -= aligned_size);
					VMEM_STATS(m_Stats.m_Used += aligned_size);

					return p;
				}
			}

			// region full, try the next one
			if(!p_region->mp_Next)
				p_region->mp_Next = CreateRegion();
			p_region = p_region->mp_Next;
		}

		return NULL;	// out of memory
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::FreeAligned(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(CoalesceOwns(p))
		{
			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;
			CheckAlignmentHeader(p_header);

			return CoalesceFree(p_header->p);
		}
		else
		{
			return PhysicalFreeDirectAligned(p);
		}
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(!m_AlignedPhysicalAllocs.Contains(p), "Free call on aligned pointer. Use FreeAligned.");

		return CoalesceFree(p) || PhysicalFreeDirect(p);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceFree(void* p)
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;
		
		if(!p_region)
			return false;

		// check the header
		Header* p_header = (Header*)p - 1;
		VMEM_ASSERT_CODE(CheckHeader(p_header));
		VMEM_ASSERT(!p_header->mp_Prev, "allocation already freed");
		VMEM_ASSERT(!p_header->mp_Next, "header already on free list");

		int size = p_header->m_Size;

		// clear the memory
		memset(p, VMEM_FREED_MEM, size - sizeof(Header));

		// get the prev and next headers
		VMEM_ASSERT(p_header->m_PrevSize, "p_header->m_PrevSize is 0");
		VMEM_ASSERT(size, "size is 0");
		Header* p_prev_header = (Header*)((byte*)p_header - p_header->m_PrevSize);
		Header* p_next_header = (Header*)((byte*)p_header + size);
		int merged_size = size;

		//----------------------------
		// merge with prev header if it's free (mp_Prev!=NULL means free)
		if(p_prev_header && p_prev_header->mp_Prev)
		{
			VMEM_ASSERT_CODE(CheckHeader(p_prev_header));

			// merge with prev header
			merged_size += p_prev_header->m_Size;

			p_prev_header->m_Size = merged_size;
			p_next_header->m_PrevSize = merged_size;

			memset(p_header, VMEM_FREED_MEM, sizeof(Header));

			p_header = p_prev_header;

			// remove from free list
			Unlink(p_header);

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
		}

		//----------------------------
		// merge with next header if it's free (mp_Prev!=NULL means free)
		if(p_next_header->mp_Prev)
		{
			VMEM_ASSERT_CODE(CheckHeader(p_next_header));

			merged_size += p_next_header->m_Size;

			Header* p_next_next_header = (Header*)((byte*)p_next_header + p_next_header->m_Size);
			if(p_next_next_header != (Header*)((byte*)p_region + m_RegionSize))
			{
				VMEM_ASSERT_CODE(CheckHeader(p_next_next_header));
				p_next_next_header->m_PrevSize = merged_size;
			}

			p_header->m_Size = merged_size;

			// remove next header from free list
			Unlink(p_next_header);

			memset(p_next_header, VMEM_FREED_MEM, sizeof(Header));

			VMEM_STATS(m_Stats.m_Overhead -= sizeof(Header));
			VMEM_STATS(m_Stats.m_Unused += sizeof(Header));
		}

		//----------------------------
		// add header to free list
		Header* p_prev = &p_region->m_FreeList;
		Header* p_next = p_prev->mp_Next;
		p_prev->mp_Next = p_header;
		p_header->mp_Prev = p_prev;
		p_header->mp_Next = p_next;
		p_next->mp_Prev = p_header;

		//----------------------------
		// remove the region if it's empty
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);
		if(merged_size == total_free_size && !(p_region == mp_RegionList && !p_region->mp_Next))
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}

		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Unused += size);

		return true;
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSizeAligned(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(CoalesceOwns(p))
		{
			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;
			void* p_alloc = p_header->p;
			size_t aligned_size = GetSize_NoLock(p_alloc);
			return aligned_size - ((byte*)p - (byte*)p_alloc);
		}
		else
		{
			size_t size = m_AlignedPhysicalAllocs.GetSize(p);

			// if the size was >= m_CoalesceHeapMaxSize and the alignment was 16 or
			// VMEM_PHYSICAL_PAGE_SIZE then it will have been allocated directly without an
			// alignment header
			if(size == VMEM_INVALID_SIZE)
				size = m_PhysicalAllocs.GetSize(p);

			return size;
		}
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSize(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		return GetSize_NoLock(p);
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::GetSize_NoLock(void* p)
	{
		if (CoalesceOwns(p))
		{
			Header* p_header = (Header*)p - 1;
			return p_header->m_Size - sizeof(Header);
		}
		else
		{
			return m_PhysicalAllocs.GetSize(p);
		}
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::Owns(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		return CoalesceOwns(p) || m_PhysicalAllocs.Contains(p) || m_AlignedPhysicalAllocs.Contains(p);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceOwns(void* p) const
	{
		// find the region that contains this alloc
		Region* p_region = mp_RegionList;
		while(p_region && (p < p_region || p >= (byte*)p_region + m_RegionSize))
			p_region = p_region->mp_Next;

		return p_region ? true : false;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::UnlinkRegion(Region* p_region)
	{
		Region* p_prev_region = NULL;
		for(Region* p_srch_region = mp_RegionList; p_srch_region!=p_region; p_srch_region=p_srch_region->mp_Next)
			p_prev_region = p_srch_region;

		Region* p_next_region = p_region->mp_Next;
		if(p_prev_region)
			p_prev_region->mp_Next = p_next_region;
		else
			mp_RegionList = p_next_region;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InitialiseHeader(Header* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		int marker_count = sizeof(p_header->m_Marker)/sizeof(unsigned int);
		for(int i=0; i<marker_count; ++i)
			p_header->m_Marker[i] = VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER;
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckHeader(Header* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		int marker_count = sizeof(p_header->m_Marker)/sizeof(unsigned int);
		VMEM_ASSERT(*(int*)&p_header->mp_Next != VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER, "This looks like an allocation allocated with AllocAligned. Please free with FreeAligned");
		for(int i=0; i<marker_count; ++i)
			VMEM_ASSERT(p_header->m_Marker[i] == VMEM_PHYSICAL_HEAP_HEADER_MAGIC_MARKER, "header is corrupt");
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::Unlink(Header* p_header)
	{
		// prev and next pointers always guaranteed to be valid because of the fixed start and end headers
		Header* p_prev = p_header->mp_Prev;
		Header* p_next = p_header->mp_Next;
		p_prev->mp_Next = p_next;
		p_next->mp_Prev = p_prev;
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckIntegrity()
	{
#if VMEM_DEBUG_LEVEL >= 1
		CriticalSectionScope lock(m_CriticalSection);

		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			int total_size = sizeof(Region);

			//----------------------------
			// check all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			bool last_free = false;
			int prev_size = 0;
			while(p_header != p_last_header)
			{
				// check marker
				CheckHeader(p_header);

				int size = p_header->m_Size;
				total_size += size;

				// check prev size
				VMEM_ASSERT(p_header->m_PrevSize == prev_size, "header has incorrect prev size");
				prev_size = size;

				// check no two consecutive free blocks
				bool free = p_header->mp_Prev != NULL;
				VMEM_ASSERT(!(free && last_free), "two consecutive free blocks, they should have been coalesced");
				last_free = free;

				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			VMEM_ASSERT(total_size == m_RegionSize, "blocks don't add up to region size");

			//----------------------------
			// check free list
			Header* p_head = &p_region->m_FreeList;
			Header* p_prev_header = p_head;
			for(Header* p_check_header = p_head->mp_Next; p_check_header!=p_head; p_check_header=p_check_header->mp_Next)
			{
				// check marker
				CheckHeader(p_check_header);

				// check prev header pointer
				VMEM_ASSERT(p_check_header->mp_Prev == p_prev_header, "prev header pointer incorrect");
				p_prev_header = p_check_header;
			}
		}

		// check stats
		#ifdef VMEM_ENABLE_STATS
			int region_count = 0;
			for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
				++region_count;
			
			size_t total_mem =
				region_count * m_RegionSize +
				m_PhysicalAllocs.GetMemorySize() +
				m_AlignedPhysicalAllocs.GetMemorySize() +
				m_PhysicalAllocDirectTotalSize;

			VMEM_ASSERT(m_Stats.GetCommittedBytes() == total_mem, "VMem PhysicalHeap Stats incorrect");
			VMEM_ASSERT(m_Stats.m_Reserved == total_mem, "VMem PhysicalHeap Stats incorrect");
		#endif
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite(Header* p_header)
	{
		int size = p_header->m_Size;
		void* p_start = p_header;
		void* p_end = (byte*)p_header + size;

		VMem::DebugWrite(_T("0x%08x - 0x%08x\t%7d\t%s\n"), p_start, p_end, size, p_header->mp_Prev ? _T("free") : _T("alloc"));
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite() const
	{
		CriticalSectionScope lock(m_CriticalSection);

		for(Region* p_region = mp_RegionList; p_region!=NULL; p_region=p_region->mp_Next)
		{
			VMem::DebugWrite(_T("--------------------------------\n"), p_region);
			VMem::DebugWrite(_T("Region 0x%08x\n"), p_region);

			// write all headers
			Header* p_last_header = (Header*)((byte*)p_region + m_RegionSize);
			Header* p_header = (Header*)(p_region + 1);
			while(p_header != p_last_header)
			{
				DebugWrite(p_header);
				p_header = (Header*)((byte*)p_header + p_header->m_Size);
			}

			// write free list
			VMem::DebugWrite(_T("\nFree list\n"));

			Header* p_list_head = &p_region->m_FreeList;
			for(Header* p_write_header=p_list_head->mp_Next; p_write_header!=p_list_head; p_write_header=p_write_header->mp_Next)
			{
				DebugWrite(p_write_header);
			}
		}

		VMem::DebugWrite(_T("\n-------------------\nPhysical Allocs:\n"));
		DebugWrite(m_PhysicalAllocs);

		VMem::DebugWrite(_T("\n-------------------\nAligned Physical Allocs:\n"));
		DebugWrite(m_AlignedPhysicalAllocs);
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::DebugWrite(const PhysicalAllocArray& array)
	{
		int physical_alloc_count = array.GetCount();
		for(int i=0; i<physical_alloc_count; ++i)
		{
			PhysicalAllocInfo alloc = array[i];
			VMem::DebugWrite(_T("0x%p : size %lld\n"), alloc.mp_Mem, (long long)alloc.m_Size);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::Trim()
	{
		CriticalSectionScope lock(m_CriticalSection);

		Region* p_region = mp_RegionList;
		int total_free_size = m_RegionSize - sizeof(Region) - 2 * sizeof(Header);

		// if there is only 1 region left and it's empty, destroy it
		if (p_region && !p_region->mp_Next && p_region->m_FreeList.mp_Next->m_Size == total_free_size)
		{
			UnlinkRegion(p_region);
			DestroyRegion(p_region);
		}
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::AllocatePhysical(size_t size)
	{
		size_t aligned_size = AlignUp(size, (size_t)VMEM_PHYSICAL_PAGE_SIZE);

#if VMEM_DEBUG_LEVEL >= 1
		++m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			m_PhysicalAllocTotal += aligned_size;
		#endif
#endif

#ifdef VMEM_PLATFORM_XBOX360
	#error Please contact slynch@puredevsoftware.com for this platform
#else
		return VirtualAlloc(NULL, aligned_size, MEM_COMMIT, PAGE_READWRITE);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::PhysicalFree(void* p)
	{
#if VMEM_DEBUG_LEVEL >= 1
		--m_PhysicalAllocCount;
		#ifdef VMEM_PLATFORM_WIN
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(p, &info, sizeof(info));
			VMEM_ASSERT(m_PhysicalAllocTotal >= info.RegionSize, "Internal VMem error");
			m_PhysicalAllocTotal -= info.RegionSize;
		#endif
#endif

#ifdef VMEM_PLATFORM_XBOX360
	#error Please contact slynch@puredevsoftware.com for this platform
#else
		VirtualFree(p, 0, MEM_RELEASE);
#endif
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::IntrnalAlloc(size_t size)
	{
		return new char[size];
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InternalFree(void* p)
	{
		char* pc = (char*)p;
		delete[] pc;
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalAllocArray::PhysicalAllocArray()
	:	mp_Array(NULL),
		m_Capacity(0),
		m_Count(0)
	{
	}

	//------------------------------------------------------------------------
	PhysicalHeap::PhysicalAllocArray::~PhysicalAllocArray()
	{
		PhysicalHeap::InternalFree(mp_Array);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Grow()
	{
		int new_capacity = m_Capacity ? 2 * m_Capacity : 1024;
		PhysicalAllocInfo* p_new_array = (PhysicalAllocInfo*)PhysicalHeap::IntrnalAlloc(new_capacity * sizeof(PhysicalAllocInfo));
		if(!p_new_array)
			return false;

		if(m_Count)
		{
			memcpy(p_new_array, mp_Array, m_Count*sizeof(PhysicalAllocInfo));
			PhysicalHeap::InternalFree(mp_Array);
		}
		mp_Array = p_new_array;
		m_Capacity = new_capacity;

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Add(void* p, size_t size)
	{
		if(m_Capacity == m_Count)
			if(!Grow())
				return false;

		PhysicalAllocInfo alloc;
		alloc.mp_Mem = p;
		alloc.m_Size = size;

		mp_Array[m_Count++] = alloc;
		return true;
	}

	//------------------------------------------------------------------------
	// it is expected that there are no more than a few hundred pointers in the
	// array, so a cache friendly linear search should suffice.
	bool PhysicalHeap::PhysicalAllocArray::Remove(void* p, size_t& size)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
		{
			if(mp_Array[i].mp_Mem == p)
			{
				size = mp_Array[i].m_Size;
				mp_Array[i] = mp_Array[--m_Count];
				return true;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------
	size_t PhysicalHeap::PhysicalAllocArray::GetSize(void* p)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
			if(mp_Array[i].mp_Mem == p)
				return mp_Array[i].m_Size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalAllocArray::Contains(void* p)
	{
		int count = m_Count;
		for(int i=0; i<count; ++i)
		{
			if(p == mp_Array[i].mp_Mem)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::PhysicalAllocDirect(size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved += size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_PhysicalAllocDirectTotalSize += size);

		return AllocatePhysical(size);
	}

	//------------------------------------------------------------------------
	void* PhysicalHeap::PhysicalAllocDirectAligned(size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved += size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_PhysicalAllocDirectTotalSize += size);

		return AllocatePhysical(size);
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalFreeDirect(void* p)
	{
		size_t size = 0;
		if(m_PhysicalAllocs.Remove(p, size))
		{
			VMEM_STATS(m_Stats.m_Reserved -= size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_PhysicalAllocDirectTotalSize -= size);

			PhysicalFree(p);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::PhysicalFreeDirectAligned(void* p)
	{
		size_t size = 0;
		if(m_AlignedPhysicalAllocs.Remove(p, size))
		{
			VMEM_STATS(m_Stats.m_Reserved -= size);
			VMEM_STATS(m_Stats.m_Used -= size);
			VMEM_STATS(m_PhysicalAllocDirectTotalSize -= size);

			PhysicalAlignHeader* p_header = (PhysicalAlignHeader*)p - 1;

			PhysicalFree(p_header->p);
			return true;
		}
		else
		{
			return PhysicalFreeDirect(p);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::WriteStats()
	{
#ifdef VMEM_ENABLE_STATS
		CriticalSectionScope lock(m_CriticalSection);

		VMem::DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		VMem::DebugWrite(_T(" Physical:  "));	

		size_t committed_bytes = m_Stats.GetCommittedBytes();
		int usage_percent = committed_bytes ? (int)((100 * (long long)m_Stats.m_Used) / committed_bytes) : 0;
		VMem::DebugWrite(_T("%3d%% "), usage_percent);

		PhysicalHeap_DebugWriteMem(m_Stats.m_Used);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Unused);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Overhead);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(committed_bytes);
		VMem::DebugWrite(_T("  "));
		PhysicalHeap_DebugWriteMem(m_Stats.m_Reserved);
		VMem::DebugWrite(_T("\n"));
#endif
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::TrackPhysicalAlloc(void* p, size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved -= m_PhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead -= m_PhysicalAllocs.GetMemorySize());

		if(!m_PhysicalAllocs.Add(p, size))
			return false;		// out of memory

		VMEM_STATS(m_Stats.m_Reserved += m_PhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead += m_PhysicalAllocs.GetMemorySize());

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::TrackPhysicalAllocAligned(void* p, size_t size)
	{
		VMEM_STATS(m_Stats.m_Reserved -= m_AlignedPhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead -= m_AlignedPhysicalAllocs.GetMemorySize());

		if(!m_AlignedPhysicalAllocs.Add(p, size))
			return false;		// out of memory

		VMEM_STATS(m_Stats.m_Reserved += m_AlignedPhysicalAllocs.GetMemorySize());
		VMEM_STATS(m_Stats.m_Overhead += m_AlignedPhysicalAllocs.GetMemorySize());

		return true;
	}

	//------------------------------------------------------------------------
	bool PhysicalHeap::CoalesceEmpty() const
	{
		return
			!mp_RegionList ||
			mp_RegionList->m_FreeList.mp_Next->m_Size == (int)(m_RegionSize - sizeof(Region) - 2 * sizeof(Header));
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::InitialiseAlignmentHeader(PhysicalAlignHeader* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		for(int i=0; i<sizeof(p_header->m_Padding)/sizeof(int); ++i)
			p_header->m_Padding[i] = VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER;
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}

	//------------------------------------------------------------------------
	void PhysicalHeap::CheckAlignmentHeader(PhysicalAlignHeader* p_header)
	{
#if VMEM_DEBUG_LEVEL >= 1
		for(int i=0; i<sizeof(p_header->m_Padding)/sizeof(int); ++i)
			VMEM_ASSERT(p_header->m_Padding[i] == VMEM_PHYSICAL_HEAP_ALIGNED_ALLOC_MARKER, "Corrupt memory on physical aligned alloc, or alloc not allocated with AllocAligned");
#else
		VMEM_UNREFERENCED_PARAM(p_header);
#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PHYSICAL_HEAP_SUPPORTED

//------------------------------------------------------------------------
//
// PhysicalPageHeap.h
//
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------

//------------------------------------------------------------------------
//
// PhysicalFSA.h
//
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class PhysicalFSA
	{
		struct Block
		{
			Block* mp_Next;
			PhysicalAlloc m_PhysicalAlloc;
		};

	public:
		//------------------------------------------------------------------------
		PhysicalFSA()
		:	mp_FreeList(NULL)
		{
		}

		//------------------------------------------------------------------------
		bool IsEmpty() const
		{
			return mp_FreeList == NULL;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE T* Alloc()
		{
			T* p = mp_FreeList;

			if (p)
				mp_FreeList = *(T**)p;

			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT - 1)) == 0, "BasicFSA Alloc returned misaligned pointer");

			return p;
		}

		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE void Free(T* p)
		{
			*(void**)p = mp_FreeList;
			mp_FreeList = p;
		}

		//------------------------------------------------------------------------
		void GiveMemory(void* p, size_t size)
		{
			VMEM_ASSERT(((uint64)p & (VMEM_INTERNAL_ALIGNMENT - 1)) == 0, "BasicFSA Alloc returned misaligned pointer");

			int count = ToInt(size / ItemSize());
			VMEM_ASSERT(count, "bad size passed to GiveMemory");

			T* p_first_item = (T*)p;
			T* p_item = p_first_item;
			for (int i = 1; i<count; ++i, ++p_item)
				*(void**)p_item = p_item + 1;
			*(void**)p_item = mp_FreeList;

			mp_FreeList = p_first_item;
		}

	private:
		//------------------------------------------------------------------------
		VMEM_FORCE_INLINE static int ItemSize()
		{
			return ToInt(AlignSizeUpPow2(sizeof(T), VMEM_INTERNAL_ALIGNMENT));
		}

		//------------------------------------------------------------------------
		// data
	private:
		T* mp_FreeList;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct Range
	{
		Range()
		:	mp_Mem(0),
			m_Size(0)
		{
		}

		Range(void* p, size_t size)
		:	mp_Mem(p),
			m_Size(size)
		{
		}

		// data
		void* mp_Mem;
		size_t m_Size;
	};

	//------------------------------------------------------------------------
	class PhysicalPageHeap
	{
		struct RangeNode
		{
			RangeNode* mp_Prev;
			RangeNode* mp_Next;
			Range m_Range;
		};

	public:
		PhysicalPageHeap();

		void Initialise(int page_size, int commit_flags);

		bool Initialised() const { return m_Initialised; }

		void Clear();

		void* Alloc(size_t size);

		bool Free(void* p, size_t size);

		void GiveMemory(void* p_mem, size_t size);

		size_t GetMaxFreeRangeSize();

		int GetCommitFlags() const { return m_CommitFlags; }

#ifdef VMEM_ENABLE_STATS
		size_t GetMemorySize() const;
#endif

	private:
		void AddRange(RangeNode* p_node);

		void RemoveRange(RangeNode* p_node);

		bool FindAndRemoveRange(size_t size, Range& range);

		int GetRangeNodeListIndex(size_t size);

		void FindMaxFreeRangeSize();

		//------------------------------------------------------------------------
		// data
	private:
		static const int m_RangeNodeListCount = 128;

		bool m_Initialised;

		int m_PageSize;

		int m_CommitFlags;

		RangeMap<RangeNode*> m_RangeMap;

		List<RangeNode> m_RangeNodeLists[m_RangeNodeListCount];	// the last list holds multiple sizes, all other lists hold one size only

		PhysicalFSA<RangeNode> m_RangeNodeFSA;

		size_t m_MaxFreeRangeSize;
		bool m_MaxFreeRangeSizeDirty;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PhysicalPageHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
namespace VMem
{
	//-----------------------------------------------------------------------------
	PhysicalPageHeap::PhysicalPageHeap()
	:	m_Initialised(false),
		m_PageSize(0),
		m_CommitFlags(0),
		m_MaxFreeRangeSize(0),
		m_MaxFreeRangeSizeDirty(0)
	{
	}

	//-----------------------------------------------------------------------------
	void PhysicalPageHeap::Initialise(int page_size, int commit_flags)
	{
		m_Initialised = true;

		m_PageSize = page_size;

		m_CommitFlags = commit_flags;
	}

	//-----------------------------------------------------------------------------
	void PhysicalPageHeap::Clear()
	{
		for (int i = 0; i<m_RangeNodeListCount; ++i)
		{
			List<RangeNode>& list = m_RangeNodeLists[i];
			while (!list.Empty())
				m_RangeNodeFSA.Free(list.RemoveHead());
		}

		m_RangeMap.Clear();

		m_MaxFreeRangeSize = 0;
	}

	//-----------------------------------------------------------------------------
	bool PhysicalPageHeap::FindAndRemoveRange(size_t size, Range& range)
	{
		int range_list_index = GetRangeNodeListIndex(size);

		while (range_list_index < m_RangeNodeListCount - 1 && m_RangeNodeLists[range_list_index].Empty())
			++range_list_index;

		List<RangeNode>& list = m_RangeNodeLists[range_list_index];

		if(range_list_index < m_RangeNodeListCount - 1)
		{
			RangeNode* p_node = list.RemoveHead();
			VMEM_ASSERT(p_node->m_Range.m_Size >= size, "RangeHeap has been corrupted");
			range = p_node->m_Range;
			m_RangeNodeFSA.Free(p_node);
			return true;
		}
		else
		{
			RangeNode* p_node = list.GetHead();
			while(p_node != list.GetIterEnd() && p_node->m_Range.m_Size < size)
				p_node = p_node->mp_Next;

			if(p_node != list.GetIterEnd())
			{
				range = p_node->m_Range;
				list.Remove(p_node);
				m_RangeNodeFSA.Free(p_node);
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	void* PhysicalPageHeap::Alloc(size_t size)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "invalid size in PhysicalPageHeap");

		Range range;
		if(FindAndRemoveRange(size, range))
		{
			RangeMap<RangeNode*>::Range range_map_range(range.mp_Mem, (byte*)range.mp_Mem + range.m_Size, 0);
			m_RangeMap.Remove(range_map_range);

			VMEM_ASSERT(range.m_Size >= size, "PhysicalPageHeap has been corrupted");
			size_t offcut_size = range.m_Size - size;
			if (offcut_size)
			{
				bool added = Free((byte*)range.mp_Mem + size, offcut_size);
				VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
				VMEM_UNREFERENCED_PARAM(added);
			}

			if (range.m_Size == m_MaxFreeRangeSize)
				m_MaxFreeRangeSizeDirty = true;

			return range.mp_Mem;
		}

		return NULL;
	}

	//-----------------------------------------------------------------------------
	bool PhysicalPageHeap::Free(void* p, size_t size)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "invalid size in PhysicalPageHeap");

		RangeMap<RangeNode*>::Range prev_range;
		bool coalesce_prev = m_RangeMap.TryGetRange((byte*)p - 1, prev_range);

		void* p_end = (byte*)p + size;
		RangeMap<RangeNode*>::Range next_range;
		bool coalesce_next = m_RangeMap.TryGetRange(p_end, next_range);
	
		if(coalesce_prev)
		{
			// coalesce prev

			if(coalesce_next)
			{
				// coalesce prev and next
				p_end = next_range.mp_End;
				m_RangeMap.Remove(next_range);
				RemoveRange(next_range.m_Value);
			}

			m_RangeMap.Remove(prev_range);
			prev_range.mp_End = p_end;
			bool added = m_RangeMap.Add(prev_range);
			VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
			VMEM_UNREFERENCED_PARAM(added);

			RangeNode* p_node = prev_range.m_Value;
			RemoveRange(p_node);
			p_node->m_Range.m_Size = (byte*)p_end - (byte*)prev_range.mp_Start;
			AddRange(p_node);
		}
		else if(coalesce_next)
		{
			// coalesce next
			m_RangeMap.Remove(next_range);
			next_range.mp_Start = p;
			bool added = m_RangeMap.Add(next_range);
			VMEM_ASSERT(added, "m_RangeMap has been corrupted");	// should always succeed because we've just removed an item
			VMEM_UNREFERENCED_PARAM(added);

			RangeNode* p_node = next_range.m_Value;
			RemoveRange(p_node);
			p_node->m_Range.mp_Mem = p;
			p_node->m_Range.m_Size += size;
			AddRange(p_node);
		}
		else
		{
			// no coalesce, just add
			RangeNode* p_node = m_RangeNodeFSA.Alloc();
			if(!p_node)
				return false;

			p_node->mp_Prev = p_node->mp_Next = NULL;
			p_node->m_Range.mp_Mem = p;
			p_node->m_Range.m_Size = size;
			AddRange(p_node);

			RangeMap<RangeNode*>::Range range;
			range.mp_Start = p;
			range.mp_End = (byte*)p + size;
			range.m_Value = p_node;

			if(!m_RangeMap.Add(range))
			{
				RemoveRange(p_node);
				m_RangeNodeFSA.Free(p_node);
				return false;
			}
		}

		if(size > m_MaxFreeRangeSize)
			m_MaxFreeRangeSize = size;

		return true;
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::AddRange(RangeNode* p_node)
	{
		size_t size = p_node->m_Range.m_Size;

		if (size > m_MaxFreeRangeSize)
			m_MaxFreeRangeSize = size;

		int range_list_index = GetRangeNodeListIndex(size);
		m_RangeNodeLists[range_list_index].AddHead(p_node);
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::RemoveRange(RangeNode* p_node)
	{
		size_t size = p_node->m_Range.m_Size;

		if (size == m_MaxFreeRangeSize)
			m_MaxFreeRangeSizeDirty = true;

		int range_list_index = GetRangeNodeListIndex(size);
		m_RangeNodeLists[range_list_index].Remove(p_node);
	}

	//------------------------------------------------------------------------
	int PhysicalPageHeap::GetRangeNodeListIndex(size_t size)
	{
		return VMin(ToInt(size / m_PageSize), m_RangeNodeListCount - 1);
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::GiveMemory(void* p_mem, size_t size)
	{
		bool range_map_full = m_RangeMap.IsFull();
		bool fsa_empty = m_RangeNodeFSA.IsEmpty();

		if(range_map_full && fsa_empty)
		{
			int map_size = AlignDownPow2(ToInt(size / 2), VMEM_INTERNAL_ALIGNMENT);
			m_RangeMap.GiveMemory(p_mem, map_size);

			int fsa_size = AlignDownPow2(ToInt(size - map_size), VMEM_INTERNAL_ALIGNMENT);
			m_RangeNodeFSA.GiveMemory((byte*)p_mem + map_size, fsa_size);
		}
		else if(range_map_full)
		{
			m_RangeMap.GiveMemory(p_mem, size);
		}
		else
		{
			m_RangeNodeFSA.GiveMemory(p_mem, size);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageHeap::FindMaxFreeRangeSize()
	{
		m_MaxFreeRangeSize = 0;

		// the last list holds multiple sizes, all other lists hold one size only
		int last_index = m_RangeNodeListCount - 1;
		List<RangeNode>& end_list = m_RangeNodeLists[last_index];
		if(!end_list.Empty())
		{
			for (RangeNode* p_node = end_list.GetHead(); p_node != end_list.GetIterEnd(); p_node = p_node->mp_Next)
			{
				size_t node_size = p_node->m_Range.m_Size;
				if (node_size > m_MaxFreeRangeSize)
					m_MaxFreeRangeSize = node_size;
			}
		}
		else
		{
			for(int i = last_index-1; i >= 0; --i)
			{
				List<RangeNode>& list = m_RangeNodeLists[i];
				RangeNode* p_node = list.GetHead();
				if(p_node != list.GetIterEnd())
				{
					m_MaxFreeRangeSize = p_node->m_Range.m_Size;
					break;
				}
			}
		}

		m_MaxFreeRangeSizeDirty = false;
	}

	//------------------------------------------------------------------------
	size_t PhysicalPageHeap::GetMaxFreeRangeSize()
	{
		if(m_MaxFreeRangeSizeDirty)
			FindMaxFreeRangeSize();

		#ifdef VMEM_ASSERTS
			size_t check_max = 0;
			for(int i=0; i<m_RangeNodeListCount; ++i)
			{
				List<RangeNode>& list = m_RangeNodeLists[i];
				for(RangeNode* p_node = list.GetHead(); p_node != list.GetIterEnd(); p_node = p_node->mp_Next)
					if(p_node->m_Range.m_Size > check_max)
						check_max = p_node->m_Range.m_Size;
			}
			VMEM_ASSERT(m_MaxFreeRangeSize == check_max, "m_RangeNodeLists has been corrupted");
		#endif

		return m_MaxFreeRangeSize;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t PhysicalPageHeap::GetMemorySize() const
	{
		return m_RangeMap.GetMemorySize();
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PhysicalPageMapper.h
//
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------

//------------------------------------------------------------------------
//
// PhysicalArray.h
//
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	template<typename T>
	class PhysicalArray
	{
	public:
		//------------------------------------------------------------------------
		struct Page
		{
			PhysicalAlloc m_Alloc;
			Page* mp_Next;
		};

		//------------------------------------------------------------------------
		PhysicalArray(int page_size)
		:	mp_Pages(NULL),
			m_CountPerPage((page_size - sizeof(Page)) / sizeof(T)),
			m_Count(0),
			m_Capacity(0),
			m_PageSize(page_size)
		{
		}

		//------------------------------------------------------------------------
		~PhysicalArray()
		{
			VMEM_ASSERT(mp_Pages == NULL, "PhysicalArray has been corrupted");
		}

		//------------------------------------------------------------------------
		int GetCount() const
		{
			return m_Count;
		}

		//------------------------------------------------------------------------
		int GetCapacity() const
		{
			return m_Capacity;
		}

		//------------------------------------------------------------------------
		size_t GetMemorySize() const
		{
			size_t size = 0;

			for(Page* p_page = mp_Pages; p_page != NULL; p_page = p_page->mp_Next)
				size += m_PageSize;

			return size;
		}

		//------------------------------------------------------------------------
		bool IsFull() const
		{
			return m_Count == m_Capacity;
		}

		//------------------------------------------------------------------------
		T& operator[](int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "PhysicalArray index out of range");
			return Get(index);
		}

		//------------------------------------------------------------------------
		// should always check IsFull first before calling add and call GiveMemory if full
		void Add(const T& value)
		{
			VMEM_ASSERT(m_Count != m_Capacity, "PhysicalArray has been corrupted");
			Get(m_Count++) = value;
		}

		//------------------------------------------------------------------------
		void RemoveLast()
		{
			VMEM_ASSERT(m_Count > 0, "VMemArray has been corrupted");
			--m_Count;
		}

		//------------------------------------------------------------------------
		void GiveMemory(PhysicalAlloc alloc)
		{
			Page* p_page = (Page*)alloc.mp_Virtual;
			p_page->m_Alloc = alloc;
			p_page->mp_Next = NULL;

			if (mp_Pages)
			{
				Page* p_last_page = mp_Pages;
				while (p_last_page->mp_Next)
					p_last_page = p_last_page->mp_Next;

				p_last_page->mp_Next = p_page;
			}
			else
			{
				mp_Pages = p_page;
			}

			m_Capacity += m_CountPerPage;
		}

		//------------------------------------------------------------------------
		void SetCount(int new_count)
		{
			VMEM_ASSERT(new_count <= m_Count, "PhysicalArray has been corrupted");
			m_Count = new_count;
		}

		//------------------------------------------------------------------------
		void Trim(Page*& p_unused_pages)
		{
			if(!m_Count)
			{
				p_unused_pages = mp_Pages;
				mp_Pages = NULL;
				m_Capacity = 0;
				return;
			}

			p_unused_pages = NULL;

			if (!mp_Pages)
				return;

			int last_page_index = (m_Count - 1) / m_CountPerPage;

			Page* p_page = mp_Pages;
			for (int i = 0; i<last_page_index; ++i)
				p_page = p_page->mp_Next;

			p_unused_pages = p_page->mp_Next;
			p_page->mp_Next = NULL;

			m_Capacity = (last_page_index + 1) * m_CountPerPage;
		}

		//------------------------------------------------------------------------
		bool PhysicalMemoryInUse(PhysicalAlloc alloc)
		{
			void* p_alloc_start = alloc.mp_Physical;
			void* p_alloc_end = (byte*)alloc.mp_Physical + alloc.m_Size;

			int index = 0;
			for (Page* p_page = mp_Pages; p_page != NULL; p_page = p_page->mp_Next)
			{
				PhysicalAlloc page_alloc = p_page->m_Alloc;
				if(page_alloc.mp_Physical >= p_alloc_start && page_alloc.mp_Physical < p_alloc_end)
					return true;

				index += m_CountPerPage;
				if(index >= m_Count)
					break;
			}

			return false;
		}

	private:
		//------------------------------------------------------------------------
		T& Get(int index)
		{
			VMEM_ASSERT(index >= 0 && index < m_Count, "PhysicalArray index out of range");

			int page_index = index / m_CountPerPage;

			Page* p_page = mp_Pages;
			for(int i=0; i<page_index; ++i)
				p_page = p_page->mp_Next;

			int index_in_page = index - (page_index * m_CountPerPage);
			T* p_array = (T*)(p_page + 1);

			return p_array[index_in_page];
		}

		//------------------------------------------------------------------------
		PhysicalArray<T>(const PhysicalArray<T>&);
		PhysicalArray<T>& operator=(const PhysicalArray<T>&);

		//------------------------------------------------------------------------
		// data
	private:
		Page* mp_Pages;

		int m_CountPerPage;

		int m_Count;
		int m_Capacity;

		int m_PageSize;

		PhysicalAlloc m_PhysicalAlloc;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class PhysicalPageMapper
	{
	public:
		PhysicalPageMapper(int page_size);

		~PhysicalPageMapper();

		bool MapPhysicalPages(void* p_virtual, size_t size, int commit_flags);

		void UnMapPhysicalPages(void* p_virtual, size_t size, int commit_flags);

		void CheckIntegrity();

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
#endif

	private:
		PhysicalPageHeap* GetPhysicalPageHeap(int commit_flags);

		PhysicalAlloc InternalAlloc(size_t size, int commit_flags);

		void InternalFree(PhysicalAlloc alloc);

		void AddMappedRange(void* p_virtual, size_t size, void* p_physical, void* p_internal_page_physical, bool& used_internal_page);

		void FreePhysical(void* p_physical, size_t size, int commit_flags);

		void AddExcessMemoryToHeap();

		void AddInternalPage(void* p_internal_page);

		//------------------------------------------------------------------------
		// data
	private:
		int m_PageSize;

		RangeMap<void*> m_MappedRangeMap;	// virtual range => physical range

		static const int m_MaxPhysicalPageHeaps = 2;
		PhysicalPageHeap m_PhysicalPageHeaps[m_MaxPhysicalPageHeaps];

		PhysicalArray<PhysicalAlloc> m_AllocatedRanges;

		struct ExcessRange
		{
			ExcessRange(void* p, size_t size, int commit_flags)
			:	mp_Mem(p),
				m_Size(size),
				m_CommitFlags(commit_flags)
			{
			}

			void* mp_Mem;
			size_t m_Size;
			int m_CommitFlags;
		};
		PhysicalArray<ExcessRange> m_ExcessPhysicalMemory;

		void* mp_InternalPages;		// pages given to m_MappedRangeMap and m_PhysicalPageHeaps

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// PhysicalPageMapper.cpp
//
#ifdef VMEM_PLATFORM_PS4
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#define VMEM_CHECK_FOR_MAPPED_RANGE_DUPLICATES

//------------------------------------------------------------------------
#ifdef VMEM_SIMULATE_PS4
	namespace PS4Simulator { typedef long long off_t; }
	#define off_t PS4Simulator::off_t
	int sceKernelReserveVirtualRange(void**, size_t, int, int);
	int sceKernelAllocateMainDirectMemory(size_t, size_t, int, off_t*);
	int sceKernelCheckedReleaseDirectMemory(off_t, size_t);
	int sceKernelMapDirectMemory(void**, size_t, int, int, off_t, size_t);
	#define SCE_OK						0
	#define SCE_KERNEL_WB_ONION			0
	#define SCE_KERNEL_WC_GARLIC		1
	#define SCE_KERNEL_MAP_FIXED		(1<<1)
	#define SCE_KERNEL_MAP_NO_COALESCE	(1<<2)
	#define SCE_KERNEL_MAP_NO_OVERWRITE	(1<<3)
	#define SCE_KERNEL_PROT_CPU_RW		(1<<4)
	#define SCE_KERNEL_PROT_GPU_RW		(1<<5)
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_CUSTOM_PHYSICAL_ALLOCATOR)
	// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is defined then you must implement these 2 functions:
	//
	// VMem_AllocatePhysical must return a pointer to physical memory (for example, using 
	// sceKernelAllocateMainDirectMemory on PS4). The memory can be mapped or unmapped.
	//
	// If the memory is mapped p_virtual must be set to the mapped virtual address. VMem
	// will unmap the memory before it starts using it, and re-map it back to this address
	// when the memory is given back. If the memory is not mapped p_virtual must be set to NULL.
	// 
	// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is not defined then VMem will allocate the
	// physical memory itself from the OS (using sceKernelAllocateMainDirectMemory on PS4).
	//
	// You can return more memory than VMem asked for and VMem will use this memory. Make
	// sure that you modify the size reference passed in.
	//
	// VMEM_CUSTOM_PHYSICAL_ALLOCATOR can be useful when you already allocate memory up-front
	// for your application, and you want VMem to use that memory.
	void* VMem_AllocatePhysical(size_t& size, size_t alignment, int commit_flags, void*& p_virtual);
	void VMem_FreePhysical(void* p_physical, void* p_virtual, size_t size);
#else
	void* VMem_AllocatePhysical(size_t& size, size_t alignment, int commit_flags, void*& p_virtual)
	{
		p_virtual = NULL;

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(alignment);
			VMEM_UNREFERENCED_PARAM(commit_flags);

			VMEM_BREAK("Physical page mapping not supported on this platform");
			return NULL;
		#endif
	}

	void VMem_FreePhysical(void* p_physical, void* p_virtual, size_t size)
	{
		VMEM_UNREFERENCED_PARAM(p_virtual);

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			VMEM_UNREFERENCED_PARAM(p_physical);
			VMEM_UNREFERENCED_PARAM(size);

			VMEM_BREAK("Physical page mapping not supported on this platform");
		#endif
	}
#endif

//-----------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	size_t g_PhysicalBytes = 0;

	//------------------------------------------------------------------------
#if VMEM_SIMULATE_OOM
	bool SimulateOOM()
	{
		int simulate_oom_count = GetPhysicalOOMCount();
		if (simulate_oom_count > 0)
		{
			if (simulate_oom_count == 1)
				return true;
			else
				SetPhysicalOOMCount(simulate_oom_count - 1);
		}

		return false;
	}
#endif

	//------------------------------------------------------------------------
	//-------------------------- OS Specific START ---------------------------
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// *pp_virtual can be NULL to let the OS choose where to map to. pp_virtual will
	// then be changed to the mapping address.
	void OS_MapPages(void** pp_virtual, size_t size, void* p_physical, int page_size)
	{
		VMEM_ASSERT(size && p_physical, "VMem internal error. Bad args passed to OS_MapPages");
		VMEM_ASSERT((((uint64)*pp_virtual) % page_size) == 0, "p_virtual not aligned to page boundary");

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else

			VMEM_UNREFERENCED_PARAM(pp_virtual);
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(p_physical);
			VMEM_UNREFERENCED_PARAM(page_size);

			VMEM_BREAK("page mapping not supported on this platform");

		#endif
	}

	//------------------------------------------------------------------------
	void OS_UnmapPages(void* p_virtual, size_t size, int page_size)
	{
		VMEM_ASSERT(p_virtual && size, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");
		VMEM_ASSERT((size % page_size) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else

			VMEM_UNREFERENCED_PARAM(p_virtual);
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(page_size);

			VMEM_BREAK("page mapping not supported on this platform");

		#endif
	}

	//------------------------------------------------------------------------
	//--------------------------- OS Specific End ----------------------------
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	PhysicalAlloc OS_AllocatePhysical(size_t size, size_t alignment, int page_size, int commit_flags)
	{
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_ASSERT((size % page_size) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		PhysicalAlloc alloc;

		#if VMEM_SIMULATE_OOM
			if(SimulateOOM())
				return alloc;
		#endif

		alloc.m_Size = size;
		alloc.mp_Physical = VMem_AllocatePhysical(alloc.m_Size, alignment, commit_flags, alloc.mp_OriginalMappedAddr);

		if(alloc.mp_Physical)
		{
			VMEM_ASSERT(alloc.m_Size >= size, "AllocatePhysical must return at least size bytes");
			VMEM_ASSERT((alloc.m_Size % page_size) == 0, "AllocatePhysical must return at least size bytes");
		}

		g_PhysicalBytes += alloc.m_Size;

		return alloc;
	}

	//------------------------------------------------------------------------
	void OS_FreePhysical(void* p_physical, void* p_virtual, size_t size)
	{
		VMem_FreePhysical(p_physical, p_virtual, size);

		g_PhysicalBytes -= size;
	}

	//------------------------------------------------------------------------
	size_t GetPhysicalBytes()
	{
		return g_PhysicalBytes;
	}

	//------------------------------------------------------------------------
	template<class T>
	void TrimPhysicalArray(T& physical_array, int page_size)
	{
		typename T::Page* p_unused_pages = NULL;
		physical_array.Trim(p_unused_pages);

		typename T::Page* p_page = p_unused_pages;
		while (p_page)
		{
			typename T::Page* p_next = p_page->mp_Next;
			OS_UnmapPages(p_page, page_size, page_size);
			p_page = p_next;
		}
	}

	//------------------------------------------------------------------------
	PhysicalPageMapper::PhysicalPageMapper(int page_size)
	:	m_PageSize(page_size),
		m_AllocatedRanges(page_size),
		m_ExcessPhysicalMemory(page_size),
		mp_InternalPages(NULL)
	{
	}

	//------------------------------------------------------------------------
	PhysicalPageMapper::~PhysicalPageMapper()
	{
		m_MappedRangeMap.Clear();
		for(int i=0; i<m_MaxPhysicalPageHeaps; ++i)
			m_PhysicalPageHeaps[i].Clear();

		VMEM_ASSERT(!m_ExcessPhysicalMemory.GetCount(), "m_ExcessPhysicalMemory has been corrupted");
		TrimPhysicalArray(m_ExcessPhysicalMemory, m_PageSize);

		void* p_internal_page = mp_InternalPages;
		while(p_internal_page)
		{
			void* p_next = *(void**)p_internal_page;
			OS_UnmapPages(p_internal_page, m_PageSize, m_PageSize);
			p_internal_page = p_next;
		}

		// this is a slightly tricky operation. Some of the pointers stored
		// in the array actually point to the arrays own memory. We need to
		// cleanly delete all of this memory, but we need to do it without
		// allocating memory ourselves. The idea is to move all pointers that
		// point to the arrays memory to the start of the list and then we
		// can safely remove the pages at the end of the array. We keep
		// doing this until there is only one page left, which can be dealt
		// with simply.
		int count = m_AllocatedRanges.GetCount();
		while(count > 1)
		{
			// trim and unmap unused pages
			TrimPhysicalArray(m_AllocatedRanges, m_PageSize);

			// free all allocations that are not being used by the array
			// the allocations that are being used move to the start of the array
			int new_count = 0;
			for(int i=0; i<count; ++i)
			{
				PhysicalAlloc alloc = m_AllocatedRanges[i];
				if(m_AllocatedRanges.PhysicalMemoryInUse(alloc))
					m_AllocatedRanges[new_count++] = alloc;
				else
					InternalFree(alloc);
			}

			count = new_count;
			m_AllocatedRanges.SetCount(count);
		}

		VMEM_ASSERT(m_AllocatedRanges.GetCount() == count, "PhysicalPageMapper has been corrupted");
		VMEM_ASSERT(count <= 1, "PhysicalPageMapper has been corrupted");

		if(count == 1)
		{
			PhysicalAlloc alloc = m_AllocatedRanges[0];

			m_AllocatedRanges.SetCount(0);
			TrimPhysicalArray(m_AllocatedRanges, m_PageSize);

			InternalFree(alloc);
		}
	}

	//------------------------------------------------------------------------
	PhysicalPageHeap* PhysicalPageMapper::GetPhysicalPageHeap(int commit_flags)
	{
		for (int i = 0; i < m_MaxPhysicalPageHeaps; ++i)
		{
			PhysicalPageHeap* p_physical_page_heap = &m_PhysicalPageHeaps[i];

			if (p_physical_page_heap->Initialised() &&
				p_physical_page_heap->GetCommitFlags() == commit_flags)
			{
				return p_physical_page_heap;
			}
		}

		for (int i = 0; i < m_MaxPhysicalPageHeaps; ++i)
		{
			PhysicalPageHeap* p_physical_page_heap = &m_PhysicalPageHeaps[i];
			if (!p_physical_page_heap->Initialised())
			{
				p_physical_page_heap->Initialise(m_PageSize, commit_flags);

#ifdef VMEM_PRE_ALLOCATE_PHYSICAL_PAGES
				PhysicalAlloc alloc = AllocatePhysical(VMEM_PRE_ALLOCATE_PHYSICAL_PAGES, false);
				VMEM_ASSERT(alloc.mp_Physical, "OOM initialising VMem!");
				FreePhysical(alloc.mp_Physical, alloc.m_Size);
#endif
				return p_physical_page_heap;
			}
		}

		VMEM_ASSERT(false, "Run out of physical page heaps. Please increase m_MaxPhysicalPageHeaps");
		return NULL;
	}

	//------------------------------------------------------------------------
	PhysicalAlloc PhysicalPageMapper::InternalAlloc(size_t size, int commit_flags)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		size_t alloc_size = size;

		PhysicalAlloc alloc;

		PhysicalAlloc internal_alloc;
		void* p_internal_mem = NULL;
		size_t intermal_mem_size = 0;
		if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
		{
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
				alloc_size += m_PageSize;

			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
				alloc_size += m_PageSize;
		}
		else
		{
			size_t internal_size = 0;
			
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
				internal_size += m_PageSize;

			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
				internal_size += m_PageSize;

			if(internal_size)
			{
				internal_alloc = OS_AllocatePhysical(alloc_size, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
				if(internal_alloc.mp_Physical)
				{
					// if the memory we got back was mapped unmap it
					if (internal_alloc.mp_OriginalMappedAddr)
						OS_UnmapPages(alloc.mp_OriginalMappedAddr, internal_alloc.m_Size, m_PageSize);

					p_internal_mem = internal_alloc.mp_Physical;
					intermal_mem_size = internal_alloc.m_Size;
				}
				else
				{
					return alloc;
				}
			}
		}

		alloc = OS_AllocatePhysical(alloc_size, m_PageSize, m_PageSize, commit_flags);

		if (alloc.mp_Physical)
		{
			VMEM_ASSERT(alloc.m_Size >= size, "AllocatePhysical must return at least size bytes");
			VMEM_ASSERT((alloc.m_Size % m_PageSize) == 0, "AllocatePhysical must return at least size bytes");

			// if the memory we got back was mapped unmap it
			if (alloc.mp_OriginalMappedAddr)
				OS_UnmapPages(alloc.mp_OriginalMappedAddr, alloc.m_Size, m_PageSize);

			size_t new_alloc_size = alloc.m_Size;

			// take off the allocated ranges size
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					void* p_physical_array_mem = (byte*)alloc.mp_Physical + new_alloc_size - m_PageSize;
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_physical_array_mem, m_PageSize);

					PhysicalAlloc array_alloc(p_physical_array_mem, m_PageSize, p_array_mem, NULL);
					m_AllocatedRanges.GiveMemory(array_alloc);

					new_alloc_size -= m_PageSize;
				}
				else
				{
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_internal_mem, m_PageSize);
					
					PhysicalAlloc array_alloc(p_internal_mem, m_PageSize, p_array_mem, NULL);
					m_AllocatedRanges.GiveMemory(array_alloc);
					
					p_internal_mem = (byte*)p_internal_mem + m_PageSize;
					intermal_mem_size -= m_PageSize;
				}
			}

			// take off the excess array size
			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					void* p_physical_array_mem = (byte*)alloc.mp_Physical + new_alloc_size - m_PageSize;
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_physical_array_mem, m_PageSize);
					
					PhysicalAlloc array_alloc(p_physical_array_mem, m_PageSize, p_array_mem, NULL);
					m_ExcessPhysicalMemory.GiveMemory(array_alloc);

					new_alloc_size -= m_PageSize;
				}
				else
				{
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_internal_mem, m_PageSize);

					PhysicalAlloc array_alloc(p_internal_mem, m_PageSize, p_array_mem, NULL);
					m_ExcessPhysicalMemory.GiveMemory(array_alloc);

					p_internal_mem = (byte*)p_internal_mem + m_PageSize;
					intermal_mem_size -= m_PageSize;
				}
			}

			// take off the excess memory
			size_t excess_size = new_alloc_size - size;
			if (excess_size)
			{
				ExcessRange excess_range((byte*)alloc.mp_Physical + size, excess_size, commit_flags);
				m_ExcessPhysicalMemory.Add(excess_range);
				new_alloc_size -= excess_size;
				VMEM_STATS(m_Stats.m_Unused += excess_size);
			}

			if(intermal_mem_size)
			{
				ExcessRange excess_range(p_internal_mem, intermal_mem_size, VMEM_DEFAULT_COMMIT_FLAGS);
				m_ExcessPhysicalMemory.Add(excess_range);
				VMEM_STATS(m_Stats.m_Unused += intermal_mem_size);
			}

			m_AllocatedRanges.Add(alloc);

			if(internal_alloc.mp_Physical)
				m_AllocatedRanges.Add(internal_alloc);

			VMEM_ASSERT(new_alloc_size == size, "something went wrong");
			alloc.m_Size = new_alloc_size;
		}
		else
		{
			if(internal_alloc.mp_Physical)
				InternalFree(internal_alloc);
		}

		return alloc;
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::InternalFree(PhysicalAlloc alloc)
	{
		if (alloc.mp_Virtual != alloc.mp_OriginalMappedAddr)
		{
			if (alloc.mp_Virtual)
				OS_UnmapPages(alloc.mp_Virtual, alloc.m_Size, m_PageSize);

			if (alloc.mp_OriginalMappedAddr)
				OS_MapPages(&alloc.mp_OriginalMappedAddr, alloc.m_Size, alloc.mp_Physical, m_PageSize);
		}

		OS_FreePhysical(alloc.mp_Physical, alloc.mp_OriginalMappedAddr, alloc.m_Size);
	}

	//------------------------------------------------------------------------
	bool PhysicalPageMapper::MapPhysicalPages(void* p_virtual, size_t size, int commit_flags)
	{
		RangeMap<void*>::Range check_range;
		VMEM_ASSERT(!m_MappedRangeMap.TryGetRange(p_virtual, check_range), "overlapping range");
		VMEM_ASSERT(!m_MappedRangeMap.TryGetRange((byte*)p_virtual + size - 1, check_range), "overlapping range");
		VMEM_UNREFERENCED_PARAM(check_range);

		PhysicalPageHeap* p_physical_page_heap = GetPhysicalPageHeap(commit_flags);

		size_t max_free_size = p_physical_page_heap->GetMaxFreeRangeSize();

		size_t size_remaining = size;
		void* p = p_virtual;
		size_t alloc_size = max_free_size ? VMin(max_free_size, size) : size;
		bool alloc_from_os = max_free_size == 0;

		while (size_remaining)
		{
			void* p_physical = NULL;
			PhysicalAlloc physical_alloc;

			size_t actual_alloc_size = alloc_size;
			if(m_MappedRangeMap.IsFull())
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					actual_alloc_size += m_PageSize;
				}
				else
				{
					PhysicalAlloc internal_alloc = InternalAlloc(m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
					void* p_internal_page_physical = internal_alloc.mp_Physical;
					if(!p_internal_page_physical)
						return false;

					void* p_internal_page = NULL;
					OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

					AddInternalPage(p_internal_page);

					m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));
				}
			}

			if(alloc_from_os)
			{
				physical_alloc = InternalAlloc(actual_alloc_size, VMEM_DEFAULT_COMMIT_FLAGS);
				p_physical = physical_alloc.mp_Physical;
			}
			else
			{
				p_physical = p_physical_page_heap->Alloc(actual_alloc_size);
				if(p_physical)
					VMEM_STATS(m_Stats.m_Unused -= actual_alloc_size);
			}

			if (p_physical)
			{
				VMEM_STATS(m_Stats.m_Used += alloc_size);

				if (m_MappedRangeMap.IsFull())
				{
					VMEM_ASSERT(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS, "Should have been allocated from CPU physical memory");

					void* p_internal_page_physical = p_physical;
					p_physical = (byte*)p_physical + m_PageSize;

					void* p_internal_page = NULL;
					OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

					AddInternalPage(p_internal_page);

					m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));
				}

				RangeMap<void*>::Range range(p, (byte*)p + alloc_size, p_physical);
				bool add_result = m_MappedRangeMap.Add(range);
				VMEM_ASSERT(add_result, "m_MappedRangeMap has been corrupted");
				VMEM_UNREFERENCED_PARAM(add_result);

				OS_MapPages(&p, alloc_size, p_physical, m_PageSize);

				size_remaining -= alloc_size;
				p = (byte*)p + alloc_size;

				alloc_size = VMin(alloc_size, size_remaining);
			}
			else
			{
				// try a smaller size
				VMEM_ASSERT(alloc_size > 0 && (alloc_size % m_PageSize) == 0, "PhysicalPageMapper has been corrupted");
				alloc_size = VMin(alloc_size - m_PageSize, size_remaining);

				// if PhysicalPageHeap has totally run out of memory allocate from the system
				if(!alloc_size && size_remaining)
				{
					if(alloc_from_os)
					{
						size_t mapped_size = size - size_remaining;
						if (mapped_size)
							UnMapPhysicalPages(p_virtual, mapped_size, commit_flags);
						return false;
					}
					else
					{
						alloc_from_os = true;
						alloc_size = size_remaining;
					}
				}
			}
		}

		AddExcessMemoryToHeap();

		return true;
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::UnMapPhysicalPages(void* p_virtual, size_t size, int commit_flags)
	{
		OS_UnmapPages(p_virtual, size, m_PageSize);

		VMEM_STATS(VMEM_ASSERT(size <= m_Stats.m_Used, "PhysicalPageMapper has been corrupted"));
		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Unused += size);

		void* p = p_virtual;
		void* p_virtual_end = (byte*)p_virtual + size;

		size_t size_remaining = size;
		while (size_remaining)
		{
			RangeMap<void*>::Range range;
			bool found_range = m_MappedRangeMap.TryGetRange(p, range);
			VMEM_ASSERT(found_range, "m_MappedRangeMap has been corrupted");
			VMEM_UNREFERENCED_PARAM(found_range);

			void* p_start = VMax(p, range.mp_Start);
			void* p_end = VMin(p_virtual_end, range.mp_End);
			size_t size_unmapped = (byte*)p_end - (byte*)p_start;

			m_MappedRangeMap.Remove(range);

			void* p_physical = range.m_Value;

			size_t prev_offcut_size = (byte*)p_start - (byte*)range.mp_Start;
			VMEM_ASSERT(prev_offcut_size >= 0, "PhysicalPageMapper has been corrupted");
			if (prev_offcut_size)
			{
				RangeMap<void*>::Range offcut_range(range.mp_Start, p_start, p_physical);
				bool offcut_add_result = m_MappedRangeMap.Add(offcut_range);
				VMEM_ASSERT(offcut_add_result, "shouldn't fail here due to oom becase we just removed an item");
				VMEM_UNREFERENCED_PARAM(offcut_add_result);
			}

			void* p_physical_start = (byte*)p_physical + prev_offcut_size;
			size_t physical_size = size_unmapped;

			size_t next_offcut_size = (byte*)range.mp_End - (byte*)p_end;
			VMEM_ASSERT(next_offcut_size >= 0, "PhysicalPageMapper has been corrupted");
			if (next_offcut_size)
			{
				void* p_internal_page = p_physical_start;
				bool used_internal_page = false;
				AddMappedRange(p_end, next_offcut_size, (byte*)p_physical + ((byte*)p_end - (byte*)range.mp_Start), p_internal_page, used_internal_page);

				if(used_internal_page)
				{
					p_physical_start = (byte*)p_physical_start + m_PageSize;
					physical_size -= m_PageSize;
					VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
				}
			}

			// free the physical range (minus the internal page if used)
			if(physical_size)
				FreePhysical(p_physical_start, physical_size, commit_flags);

			size_remaining -= size_unmapped;
			p = (byte*)p + size_unmapped;
		}

		VMEM_ASSERT(!m_ExcessPhysicalMemory.GetCount(), "m_ExcessPhysicalMemory has been corrupted");
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddMappedRange(void* p_virtual, size_t size, void* p_physical, void* p_internal_page_physical, bool& used_internal_page)
	{
		RangeMap<void*>::Range range(p_virtual, (byte*)p_virtual + size, p_physical);
		if (!m_MappedRangeMap.Add(range))
		{
			used_internal_page = true;

			void* p_internal_page = NULL;
			OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

			AddInternalPage(p_internal_page);

			m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));

			bool result = m_MappedRangeMap.Add(range);
			VMEM_ASSERT(result, "m_MappedRangeMap.Add failed even after giving memory!");
			VMEM_UNREFERENCED_PARAM(result);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddInternalPage(void* p_internal_page)
	{
		*(void**)p_internal_page = mp_InternalPages;
		mp_InternalPages = p_internal_page;
	}

	//------------------------------------------------------------------------
	#ifdef _WIN32
		#pragma warning(push)
		#pragma warning(disable : 4133)
	#endif

	//------------------------------------------------------------------------
	void PhysicalPageMapper::FreePhysical(void* p_physical, size_t size, int commit_flags)
	{
		PhysicalPageHeap* p_physical_page_heap = GetPhysicalPageHeap(commit_flags);

		if(!p_physical_page_heap->Free(p_physical, size))
		{
			// give PhysicalPageHeap some memory and try again
			void* p_virtual = NULL;
			OS_MapPages(&p_virtual, m_PageSize, p_physical, m_PageSize);

			AddInternalPage(p_virtual);

			VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
			p_physical_page_heap->GiveMemory((byte*)p_virtual + sizeof(void*), m_PageSize - sizeof(void*));

			size_t new_size = size - m_PageSize;
			if (new_size)
			{
				bool result = p_physical_page_heap->Free((byte*)p_physical + m_PageSize, new_size);
				VMEM_ASSERT(result, "PhysicalPageHeap has been corrupted");
				VMEM_UNREFERENCED_PARAM(result);
			}
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddExcessMemoryToHeap()
	{
		int count = m_ExcessPhysicalMemory.GetCount();
		while(count)
		{
			ExcessRange range = m_ExcessPhysicalMemory[count - 1];
			m_ExcessPhysicalMemory.RemoveLast();

			FreePhysical(range.mp_Mem, range.m_Size, range.m_CommitFlags);

			count = m_ExcessPhysicalMemory.GetCount();
		}
	}
	
	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats PhysicalPageMapper::GetStats() const
	{
		Stats stats = m_Stats;
		
		stats.m_Overhead += m_MappedRangeMap.GetMemorySize();

		for(int i=0; i<m_MaxPhysicalPageHeaps; ++i)
			stats.m_Overhead += m_PhysicalPageHeaps[i].GetMemorySize();

		stats.m_Overhead += m_AllocatedRanges.GetMemorySize();
		stats.m_Overhead += m_ExcessPhysicalMemory.GetMemorySize();

		for(void* p_internal_page = mp_InternalPages; p_internal_page != NULL; p_internal_page = *(void**)p_internal_page)
			stats.m_Overhead += m_PageSize;

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void PhysicalPageMapper::CheckIntegrity()
	{
#if defined(VMEM_ENABLE_STATS) && defined(VMEM_ASSERTS)
		Stats stats = GetStats();
		VMEM_ASSERT(g_PhysicalBytes == stats.GetCommittedBytes(), "Stats doesn't match g_PhysicalBytes");
#endif
	}

	//------------------------------------------------------------------------
	#ifdef _WIN32
		#pragma warning(pop)
	#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// ProtectedHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP


#ifdef VMEM_OS_LINUX
	#include <pthread.h>
	#include <sys/time.h>
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class ProtectedHeap
	{
	public:
		ProtectedHeap(int page_size, int reserve_flags, int commit_flags);
		
		~ProtectedHeap();

		void* Alloc(size_t size);

		bool Free(void* p);

		size_t GetSize(void* p) const;

		void CheckIntegrity();

		void Lock() const { m_CriticalSection.Enter(); }

		void Release() const { m_CriticalSection.Leave(); }

#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const;
		Stats GetStatsNoLock() const;
#endif
	private:
		bool ReleaseOldestProtectedAlloc();

		bool AddUnreleasedPage(void* p_page, size_t size);

		void* AllocInternal(size_t size);

		ProtectedHeap& operator=(const ProtectedHeap&);

		//------------------------------------------------------------------------
		// data
	private:
		HashMap<AddrKey, size_t> m_Allocs;

		int m_PageSize;

		int m_ReserveFlags;
		int m_CommitFlags;

		mutable CriticalSection m_CriticalSection;

		int m_AllocIndex;

		size_t m_Overhead;

		struct UnreleasedAllocsPage
		{
			UnreleasedAllocsPage* mp_Prev;
			UnreleasedAllocsPage* mp_Next;
			int m_UsedCount;
			int m_ReleasedCount;
		};
		struct UnreleasedAlloc
		{
			void* mp_Page;
			size_t m_Size;
		};
		UnreleasedAllocsPage m_UnreleasedAllocsPageList;		// list of linked pages contains lists of unreleased memory allocs

#ifdef VMEM_ENABLE_STATS
		Stats m_Stats;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// ProtectedHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace VMem
{
	const int g_ProtectedAllocAlignment = VMEM_NATURAL_ALIGNMENT;

	extern size_t g_ReservedBytes;

	typedef bool (*VMemSHouldProtectFn)(int i, size_t size);
	extern VMemSHouldProtectFn g_VMemSHouldProtectFn;

	inline bool ShouldProtect(int i, size_t size)
	{
		if(g_VMemSHouldProtectFn)
			return g_VMemSHouldProtectFn(i, size);
		else
			return VMemShouldProtect(i, size);
	}

	//------------------------------------------------------------------------
	inline void CheckMemoryBytes(void* p, size_t size, unsigned int value)
	{
#ifdef VMEM_ASSERTS
		byte* p_check = (byte*)p;
		for(size_t i=0; i<size; ++i, ++p_check)
			VMEM_MEM_CHECK(p_check, (byte)value);
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(value);
#endif
	}

	//------------------------------------------------------------------------
	ProtectedHeap::ProtectedHeap(int page_size, int reserve_flags, int commit_flags)
	:	m_Allocs(page_size),
		m_PageSize(page_size),
		m_ReserveFlags(reserve_flags),
		m_CommitFlags(commit_flags),
		m_AllocIndex(rand()),
		m_Overhead(0)
	{
		m_UnreleasedAllocsPageList.mp_Prev = &m_UnreleasedAllocsPageList;
		m_UnreleasedAllocsPageList.mp_Next = &m_UnreleasedAllocsPageList;
	}

	//------------------------------------------------------------------------
	ProtectedHeap::~ProtectedHeap()
	{
		// free any unfreed allocs
		HashMap<AddrKey, size_t>::Iterator iter=m_Allocs.GetIterator();
		while(iter.MoveNext())
			Free(iter.GetKey().GetAddr());

		while(ReleaseOldestProtectedAlloc())
			;
	}

	//------------------------------------------------------------------------
	void* ProtectedHeap::Alloc(size_t size)
	{
		CriticalSectionScope lock(m_CriticalSection);

		if(!ShouldProtect(m_AllocIndex++, size))
			return NULL;

		return AllocInternal(size);
	}

	//------------------------------------------------------------------------
	void* ProtectedHeap::AllocInternal(size_t size)
	{
		if(size == 0)
			size = 4;

		size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);

		size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);
		if(m_Overhead + page_aligned_size - size >= VMEM_PROTECTED_HEAP_MAX_OVERHEAD)
			return NULL;

		void* p_page = VMem::VirtualReserve(page_aligned_size, m_PageSize, m_ReserveFlags);
		while (!p_page && ReleaseOldestProtectedAlloc())
			p_page = VMem::VirtualReserve(page_aligned_size, m_PageSize, m_ReserveFlags);
		if(!p_page)
			return NULL;

		if(!VMem::VirtualCommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags))
		{
			VMem::VirtualRelease(p_page, page_aligned_size);
			return NULL;
		}

		size_t alloc_offset = page_aligned_size - aligned_size;
		void* p_alloc = (byte*)p_page + alloc_offset;
		if(alloc_offset)
			memset(p_page, VMEM_PROTECTED_PAGE, alloc_offset);

		size_t end_guard_size = aligned_size - size;
		if(end_guard_size)
			memset((byte*)p_page + page_aligned_size - end_guard_size, VMEM_PROTECTED_PAGE, end_guard_size);

		if(!m_Allocs.Add(p_alloc, size))
		{
			VMem::VirtualDecommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags);
			VMem::VirtualRelease(p_page, page_aligned_size);
			return NULL;
		}

		size_t overhead = page_aligned_size - size;
		m_Overhead += overhead;

		VMEM_STATS(m_Stats.m_Reserved += page_aligned_size);
		VMEM_STATS(m_Stats.m_Used += size);
		VMEM_STATS(m_Stats.m_Overhead += overhead);
		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used -= sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead += sizeof(VMemCustomAllocInfo));
		#endif

		return p_alloc;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::Free(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		void* p_alloc = p;

		size_t size = 0;
		AddrKey key(p_alloc);
		if(!m_Allocs.TryGetValue(key, size))
			return false;

		m_Allocs.Remove(key);

		size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);
		size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);

		size_t alloc_offset = page_aligned_size - aligned_size;

		void* p_page = (byte*)p_alloc - alloc_offset;

		if(alloc_offset)
			CheckMemory(p_page, alloc_offset, VMEM_PROTECTED_PAGE);

		size_t end_guard_size = aligned_size - size;
		if(end_guard_size)
			CheckMemoryBytes((byte*)p_page + page_aligned_size - end_guard_size, end_guard_size, VMEM_PROTECTED_PAGE);

		// we only decommit, do not release. This protects the page and
		// stops the virtual address being used again
		VMem::VirtualDecommit(p_page, page_aligned_size, m_PageSize, m_CommitFlags);

		size_t overhead = page_aligned_size - size;
		m_Overhead -= overhead;

		#ifdef VMEM_CUSTOM_ALLOC_INFO
			VMEM_STATS(m_Stats.m_Used += sizeof(VMemCustomAllocInfo));
			VMEM_STATS(m_Stats.m_Overhead -= sizeof(VMemCustomAllocInfo));
		#endif
		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Overhead -= overhead);

		if(!AddUnreleasedPage(p_page, page_aligned_size))
		{
			// in an out of memory situation release the memory immediatley (and return true because we freed it)
			VirtualRelease(p_page, page_aligned_size);
			VMEM_STATS(m_Stats.m_Reserved -= page_aligned_size);
		}

		return true;
	}

	//------------------------------------------------------------------------
	size_t ProtectedHeap::GetSize(void* p) const
	{
		CriticalSectionScope lock(m_CriticalSection);

		size_t size = 0;
		if(m_Allocs.TryGetValue(p, size))
			return size;

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::AddUnreleasedPage(void* p_page, size_t size)
	{
		UnreleasedAllocsPage* p_unreleased_allocs_page = m_UnreleasedAllocsPageList.mp_Prev;

		int unreleased_allocs_per_page = ((int)((m_PageSize - sizeof(UnreleasedAllocsPage)) / sizeof(UnreleasedAlloc)));
		if(p_unreleased_allocs_page == &m_UnreleasedAllocsPageList || p_unreleased_allocs_page->m_UsedCount == unreleased_allocs_per_page)
		{
			void* p_new_page = VMem::VirtualReserve(m_PageSize, m_PageSize, m_ReserveFlags);
			if(!p_new_page)
				return false;

			if(!VMem::VirtualCommit(p_new_page, m_PageSize, m_PageSize, m_CommitFlags))
			{
				VMem::VirtualRelease(p_new_page, m_PageSize);
				return false;
			}

			memset(p_new_page, 0, m_PageSize);

			p_unreleased_allocs_page = (UnreleasedAllocsPage*)p_new_page;

			p_unreleased_allocs_page->m_UsedCount = 0;
			p_unreleased_allocs_page->m_ReleasedCount = 0;

			// add to the end of the list
			p_unreleased_allocs_page->mp_Next = &m_UnreleasedAllocsPageList;
			p_unreleased_allocs_page->mp_Prev = m_UnreleasedAllocsPageList.mp_Prev;
			p_unreleased_allocs_page->mp_Prev->mp_Next = p_unreleased_allocs_page;
			m_UnreleasedAllocsPageList.mp_Prev = p_unreleased_allocs_page;

			m_Overhead += m_PageSize;

			VMEM_STATS(m_Stats.m_Reserved += m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead += m_PageSize);
		}

		// add to the last block page
		UnreleasedAlloc* p_unreleased_blocks = (UnreleasedAlloc*)(p_unreleased_allocs_page + 1);
		UnreleasedAlloc* p_unreleased_block = p_unreleased_blocks + p_unreleased_allocs_page->m_UsedCount;

		p_unreleased_block->mp_Page = p_page;
		p_unreleased_block->m_Size = size;

		++p_unreleased_allocs_page->m_UsedCount;

		return true;
	}

	//------------------------------------------------------------------------
	bool ProtectedHeap::ReleaseOldestProtectedAlloc()
	{
		// check if there are any block pages
		UnreleasedAllocsPage* p_unreleased_allocs_page = m_UnreleasedAllocsPageList.mp_Next;
		if(p_unreleased_allocs_page == &m_UnreleasedAllocsPageList)
			return false;

		// get the next block to release from the first block page
		UnreleasedAlloc* p_unreleased_blocks = (UnreleasedAlloc*)(p_unreleased_allocs_page + 1);
		UnreleasedAlloc unreleased_block = p_unreleased_blocks[p_unreleased_allocs_page->m_ReleasedCount];

		// release the block
		VMEM_ASSERT(unreleased_block.mp_Page, "unreleased_block has been corrupted");
		VirtualRelease(unreleased_block.mp_Page, unreleased_block.m_Size);
		VMEM_STATS(m_Stats.m_Reserved -= unreleased_block.m_Size);

		++p_unreleased_allocs_page->m_ReleasedCount;
		VMEM_ASSERT(p_unreleased_allocs_page->m_ReleasedCount <= p_unreleased_allocs_page->m_UsedCount, "p_unreleased_allocs_page->m_UsedCount has been corrupted");

		// if all blocks in the current page have been totally released release the page
		if(p_unreleased_allocs_page->m_ReleasedCount == p_unreleased_allocs_page->m_UsedCount)
		{
			// unlink the page
			p_unreleased_allocs_page->mp_Prev->mp_Next = p_unreleased_allocs_page->mp_Next;
			p_unreleased_allocs_page->mp_Next->mp_Prev = p_unreleased_allocs_page->mp_Prev;

			// free the page
			VMem::VirtualDecommit(p_unreleased_allocs_page, m_PageSize, m_PageSize, m_CommitFlags);
			VMem::VirtualRelease(p_unreleased_allocs_page, m_PageSize);

			m_Overhead -= m_PageSize;

			VMEM_STATS(m_Stats.m_Reserved -= m_PageSize);
			VMEM_STATS(m_Stats.m_Overhead -= m_PageSize);
		}

		return true;
	}

	//------------------------------------------------------------------------
	void ProtectedHeap::CheckIntegrity()
	{
		CriticalSectionScope lock(m_CriticalSection);

		HashMap<AddrKey, size_t>::Iterator iter = m_Allocs.GetIterator();
		while(iter.MoveNext())
		{
			void* p_alloc = (void*)iter.GetKey().GetAddr();
			size_t size = iter.GetValue();

			size_t aligned_size = AlignSizeUpPow2(size, g_ProtectedAllocAlignment);
			size_t page_aligned_size = AlignSizeUpPow2(aligned_size, m_PageSize);

			size_t alloc_offset = page_aligned_size - aligned_size;

			void* p_page = (byte*)p_alloc - alloc_offset;

			if(alloc_offset)
				CheckMemory(p_page, alloc_offset, VMEM_PROTECTED_PAGE);

			size_t end_guard_size = aligned_size - size;
			if(end_guard_size)
				CheckMemoryBytes((byte*)p_page + page_aligned_size - end_guard_size, end_guard_size, VMEM_PROTECTED_PAGE);
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats ProtectedHeap::GetStats() const
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats ProtectedHeap::GetStatsNoLock() const
	{
		int map_size = m_Allocs.GetAllocedMemory();

		Stats internal_stats;
		internal_stats.m_Reserved += map_size;
		internal_stats.m_Overhead += map_size;

		return m_Stats + internal_stats;
	}
#endif
}

//------------------------------------------------------------------------
#endif			// #ifdef VMEM_PROTECTED_HEAP

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// TrailGuard.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	inline int GetAllocSize(void* p, int fixed_alloc_size)
	{
		return fixed_alloc_size != -1 ? fixed_alloc_size : *(int*)((VMem::byte*)p + sizeof(void*));
	}

	//------------------------------------------------------------------------
	TrailGuard::TrailGuard()
	:	m_Size(0),
		m_CurSize(0),
		m_FixedAllocSize(0),
		mp_Head(NULL),
		mp_Tail(NULL),
		m_FullCheckFreq(0),
		m_FullCheckCounter(0)
	{
	}

	//------------------------------------------------------------------------
	void TrailGuard::Initialise(int size, int check_freq, int alloc_size)
	{
		m_Size = size;
		m_FullCheckFreq = check_freq;
		m_FixedAllocSize = alloc_size;
	}

	//------------------------------------------------------------------------
	void* TrailGuard::Shutdown()
	{
		void* p_trail = mp_Head;
		mp_Head = NULL;
		mp_Tail = NULL;
		m_Size = 0;
		m_CurSize = 0;
		return p_trail;
	}

	//------------------------------------------------------------------------
	void* TrailGuard::Add(void* p, int size)
	{
		if(!m_Size) return NULL;

		// add alloc to tail
		if(mp_Tail)
		{
			*(void**)mp_Tail = p;
			mp_Tail = p;
		}
		else
		{
			mp_Head = mp_Tail = p;
		}
		*(void**)p = NULL;

		// set the guard
		int guard_size = size - sizeof(void*);
		void* p_guard = (byte*)p + sizeof(void*);
		if(m_FixedAllocSize == -1)
		{
			VMEM_ASSERT((unsigned int)size >= sizeof(void*) + sizeof(int), "Variable size guards must be >= sizeof(void*) + sizeof(int)");
			guard_size -= sizeof(int);
			*(int*)p_guard = size;
			p_guard = (byte*)p_guard + sizeof(int);

		}
		if(guard_size)
			memset(p_guard, VMEM_TRAIL_GUARD_MEM, guard_size);

		// increase trail size
		m_CurSize += size;

		// check the entire trail
		if(m_FullCheckCounter == m_FullCheckFreq)
		{
			m_FullCheckCounter = 0;
			CheckIntegrity();
		}
		++m_FullCheckCounter;

		// return NULL if trail has not reached limit yet
		if(m_CurSize < m_Size)
			return NULL;

		// remove alloc from head
		void* new_p = mp_Head;
		mp_Head = *(void**)new_p;

		int new_alloc_size = GetAllocSize(new_p, m_FixedAllocSize);
		m_CurSize -= new_alloc_size;

		int new_guard_size = new_alloc_size - sizeof(void*);
		void* p_new_guard = (byte*)new_p + sizeof(void*);
		if(m_FixedAllocSize == -1)
		{
			new_guard_size -= sizeof(int);
			p_new_guard = (byte*)p_new_guard + sizeof(int);
		}

		// check that the guard is still intact
		CheckMemory(p_new_guard, new_guard_size, VMEM_TRAIL_GUARD_MEM);

		return new_p;
	}

	//------------------------------------------------------------------------
	void TrailGuard::CheckIntegrity() const
	{
		VMEM_ASSERT_CODE(int total_trail_size = 0);
		void* p_alloc = mp_Head;

		while(p_alloc)
		{
			int size = GetAllocSize(p_alloc, m_FixedAllocSize);

			void* p_guard = (byte*)p_alloc + sizeof(void*);
			int guard_size = size - sizeof(void*);

			if(m_FixedAllocSize == -1)
			{
				guard_size -= sizeof(int);
				p_guard = (byte*)p_guard + sizeof(int);
			}

			CheckMemory(p_guard, guard_size, VMEM_TRAIL_GUARD_MEM);

			VMEM_ASSERT_CODE(total_trail_size += size);
			VMEM_ASSERT(total_trail_size <= m_CurSize, "Trail guard fail");

			// get the next alloc
			p_alloc = *(void**)p_alloc;
		}

		VMEM_ASSERT(total_trail_size == m_CurSize, "Trail guard fail");
	}

	//------------------------------------------------------------------------
	bool TrailGuard::Contains(void* p) const
	{
		void* p_alloc = mp_Head;

		while(p_alloc)
		{
			if(p_alloc == p)
				return true;
			p_alloc = *(void**)p_alloc;
		}

		return false;
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemThread.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

#ifdef VMEM_OS_LINUX
	#include <pthread.h>
	#include <sys/time.h>
	#include <errno.h> 
#endif

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(push)
	#pragma warning(disable : 4100)
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class InternalHeap;

	//------------------------------------------------------------------------
	typedef int (*ThreadMain)(void*);

#if defined(VMEM_OS_WIN)
	typedef HANDLE ThreadHandle;
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_LINUX)
	typedef pthread_t ThreadHandle;
#else
	#error	// OS not defined
#endif

	//------------------------------------------------------------------------
	namespace Thread
	{
		void Initialise();

		void Destroy();

		ThreadHandle CreateThread(ThreadMain p_thread_main, void* p_context);

		void TerminateThread(ThreadHandle thread);

		void WaitForThreadToTerminate(ThreadHandle thread);
	}

	//------------------------------------------------------------------------
	class Event
	{
	public:
		//------------------------------------------------------------------------
		Event(bool initial_state, bool auto_reset)
		{
#ifdef VMEM_OS_WIN
			m_Handle = CreateEvent(NULL, !auto_reset, initial_state, NULL);
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
			pthread_cond_init(&m_Cond, NULL);
			pthread_mutex_init(&m_Mutex, NULL);
			m_Signalled = false;
			m_AutoReset = auto_reset;
	
			if(initial_state)
				Set();
#endif
		}

		//------------------------------------------------------------------------
		~Event()
		{
#ifdef VMEM_OS_WIN
			CloseHandle(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
			pthread_mutex_destroy(&m_Mutex);
			pthread_cond_destroy(&m_Cond);
#endif
		}

		//------------------------------------------------------------------------
		void Set() const
		{
#ifdef VMEM_OS_WIN
			SetEvent(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
			pthread_mutex_lock(&m_Mutex);
			m_Signalled = true;
			pthread_mutex_unlock(&m_Mutex);
			pthread_cond_signal(&m_Cond);
#endif
		}

		//------------------------------------------------------------------------
		void Reset()
		{
#ifdef VMEM_OS_WIN
			ResetEvent(m_Handle);
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
			pthread_mutex_lock(&m_Mutex);
			m_Signalled = false;
			pthread_mutex_unlock(&m_Mutex);
#endif
		}

		//------------------------------------------------------------------------
		int Wait(int timeout=-1) const
		{
#ifdef VMEM_OS_WIN
			VMEM_STATIC_ASSERT(INFINITE == -1, "INFINITE == -1");
			return WaitForSingleObject(m_Handle, timeout) == 0/*WAIT_OBJECT_0*/;
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
			pthread_mutex_lock(&m_Mutex);
	
			if(m_Signalled)
			{
				m_Signalled = false;
				pthread_mutex_unlock(&m_Mutex);
				return true;
			}
	
			if(timeout == -1)
			{
				while(!m_Signalled)
					pthread_cond_wait(&m_Cond, &m_Mutex);
		
				if(!m_AutoReset)
					m_Signalled = false;

				pthread_mutex_unlock(&m_Mutex);
		
				return true;
			}
			else
			{		
				timeval curr;
				gettimeofday(&curr, NULL);
		
				timespec time;
				time.tv_sec  = curr.tv_sec + timeout / 1000;
				time.tv_nsec = (curr.tv_usec * 1000) + ((timeout % 1000) * 1000000);
		
				time.tv_sec += time.tv_nsec / 1000000000L;
				time.tv_nsec = time.tv_nsec % 1000000000L;

				int ret = 0;
				do
				{
					ret = pthread_cond_timedwait(&m_Cond, &m_Mutex, &time);

				} while (!m_Signalled && ret != ETIMEDOUT);

				if(m_Signalled)
				{
					if(!m_AutoReset)
						m_Signalled = false;

					pthread_mutex_unlock(&m_Mutex);
					return true;
				}
		
				pthread_mutex_unlock(&m_Mutex);
				return false;
			}
#endif
		}

		//------------------------------------------------------------------------
		// data
	private:
#ifdef VMEM_OS_WIN
		HANDLE m_Handle;
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
		mutable pthread_cond_t  m_Cond;
		mutable pthread_mutex_t m_Mutex;
		mutable volatile bool m_Signalled;
		bool m_AutoReset;
#endif
	};
}

//------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(pop)
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VirtualMem.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	VirtualMem::VirtualMem(int page_size)
	:	m_PageSize(page_size),
		m_Reservations(page_size)
#ifdef VMEM_ENABLE_STATS
		,m_ReservedOverhead(0)
#endif
#ifdef VMEM_DECOMMIT_CACHE
		,m_PendingDecommitBytes(0)
		,m_PendingReleaseBytes(0)
		,m_ReleaseNodePool(page_size)
		,m_DecommitNodePool(page_size)
		,m_RangeMapAllocator(page_size)
		,m_DecommitNodeMap(&m_RangeMapAllocator, page_size)
		,m_CurrentFrame(0)
#endif
	{
	}

	//------------------------------------------------------------------------
	VirtualMem::~VirtualMem()
	{
#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);
		FlushMemory(true);
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	int VirtualMem::GetReleaseNodeBucketIndex(size_t size)
	{
		return VMin((int)(size / (1024*1024)), m_ReleaseNodeBucketCount-1);
	}
#endif

	//------------------------------------------------------------------------
	void* VirtualMem::Reserve(size_t size, int align, int reserve_flags)
	{
		VMEM_ASSERT(size > 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualReserve");

		void* p = NULL;

		size_t aligned_size = size;
		bool needs_aligning = align != m_PageSize;

#ifdef VMEM_DECOMMIT_CACHE
		int bucket_index = GetReleaseNodeBucketIndex(size);
#endif
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_DECOMMIT_CACHE
		// try ad find an existing released node in the release node buckets
		List<ReleaseNode>& node_list = m_ReleaseNodeBuckets[bucket_index];
		ReleaseNode* p_node_end = node_list.GetIterEnd();

		for(ReleaseNode* p_node=node_list.GetHead(); p_node!=p_node_end; p_node=p_node->mp_Next)
		{
			if(p_node->m_Size == size && p_node->m_Align == align && p_node->m_ReserveFlags == reserve_flags)
			{
				p = p_node->mp_Mem;
				node_list.Remove(p_node);
				aligned_size = p_node->m_AlignedSize;
				m_ReleaseNodePool.Free(p_node);
				m_PendingReleaseBytes -= size;
				break;
			}
		}
#endif
		// if we didn't find a pre-existing released node reserve from the OS
		if(!p)
		{
			if(needs_aligning)
			{
				VMEM_ASSERT((align & (m_PageSize-1)) == 0, "bad alignment");
				aligned_size = AlignSizeUpPow2(aligned_size + align-1, m_PageSize);
			}

			p = VMem::VirtualReserve(aligned_size, m_PageSize, reserve_flags);

			if(!p)
			{
#ifdef VMEM_DECOMMIT_CACHE
				FlushMemory(true);
				p = VMem::VirtualReserve(aligned_size, m_PageSize, reserve_flags);
				if(!p)
#endif
					return NULL;
			}

			VMEM_STATS(m_ReservedOverhead += aligned_size - size);
		}

		// record the virtual alloc and hash it based on the ALIGNED address
		Reservation reservation;
		reservation.mp_BaseAddr = p;
		reservation.m_Align = align;
		reservation.m_ReserveFlags = reserve_flags;
		reservation.m_Size = size;
		reservation.m_AlignedSize = aligned_size;

		if(needs_aligning)
			p = AlignUp((byte*)p, align);

		if(!m_Reservations.Add(p, reservation))
		{
			VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
			return NULL;		// out of memory
		}

		return p;
	}

	//------------------------------------------------------------------------
	void VirtualMem::Release(void* p)
	{
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualRelease");

		Reservation reservation = m_Reservations.Remove(p);

#ifdef VMEM_DECOMMIT_CACHE
		p = reservation.mp_BaseAddr;

		size_t size = reservation.m_AlignedSize;
		VMEM_ASSERT(size > 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualRelease");

		int bucket_index = GetReleaseNodeBucketIndex(size);

		ReleaseNode* p_node = m_ReleaseNodePool.Alloc();
		if(!p_node)
		{
			// out of memory so release immediately
			DecommitNodesInRange(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
			VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
			return;
		}

		p_node->mp_Mem = p;
		p_node->m_Size = reservation.m_Size;
		p_node->m_AlignedSize = reservation.m_AlignedSize;
		p_node->m_Align = reservation.m_Align;
		p_node->m_ReserveFlags = reservation.m_ReserveFlags;
		p_node->mp_Next = NULL;
		p_node->mp_Prev = NULL;
		p_node->m_FlushFrame = m_CurrentFrame + VMEM_DECOMMIT_CACHE_RELEASE_FRAME_DELAY;

		m_ReleaseNodeBuckets[bucket_index].AddHead(p_node);

		m_PendingReleaseBytes += reservation.m_Size;
#else
		VMem::VirtualRelease(reservation.mp_BaseAddr, reservation.m_AlignedSize);
		VMEM_STATS(m_ReservedOverhead -= reservation.m_AlignedSize - reservation.m_Size);
#endif
	}

	//------------------------------------------------------------------------
	bool VirtualMem::Commit(void* p, size_t size, int commit_flags)
	{
#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);

		VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualCommit");
		VMEM_ASSERT(size >= 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualCommit");

		bool succeeded = TryCommit(p, size, commit_flags);
		if(!succeeded)
		{
			FlushMemory(true);
			succeeded = TryCommit(p, size, commit_flags);
		}

		if(succeeded)
		{
			VMEM_MEMSET(p, VMEM_COMMITTED_MEM, size);
			VMEM_ASSERT(VMem::Committed(p, size), "Commit succeeded but memory is not committed!");
		}

		return succeeded;
#else
		return VMem::VirtualCommit(p, size, m_PageSize, commit_flags);
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::DecommitNodesInRange(void* p, size_t size)
	{
		DecommitNode* p_node = m_DecommitNodeList.GetHead();
		DecommitNode* p_node_end = m_DecommitNodeList.GetIterEnd();
		while(p_node != p_node_end)
		{
			DecommitNode* p_next = p_node->mp_Next;

			// if there is any overlap decommit it
			if((byte*)p_node->mp_Mem + p_node->m_Size > p && p_node->mp_Mem < (byte*)p + size)
			{
				DecommitDecommitNode(p_node);
			}

			p_node = p_next;
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	bool VirtualMem::TryCommit(void* p_commit, size_t commit_size, int commit_flags)
	{
		DecommitNode* p_decommit_node = NULL;

		// try and find a node that overlaps the commit range
		if (!m_DecommitNodeMap.TryRemoveRange(p_commit, commit_size, p_decommit_node))
		{
			// if we didn't find a node simply allocate from the OS and we are done
			return VMem::VirtualCommit(p_commit, commit_size, m_PageSize, commit_flags);
		}

		VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

		// handle the common case of re-committing exactly the same range
		if(p_decommit_node->mp_Mem == p_commit && p_decommit_node->m_Size == commit_size)
		{
			#ifdef VMEM_ENABLE_MEMSET
				CheckMemory(p_commit, commit_size, VMEM_DECOMMITTED_MEM);
			#endif

			m_DecommitNodeList.Remove(p_decommit_node);
			m_DecommitNodePool.Free(p_decommit_node);

			m_PendingDecommitBytes -= commit_size;

			return true;
		}
		else
		{
			// the decommit node is not exactly the same but overlaps, so we must take just the ranges that we need

			size_t committed_bytes = 0;
			void* p = p_commit;
			size_t total_bytes_to_commit = commit_size;

			// handle the case where p_commit is part way into the node
			if(p_decommit_node->mp_Mem < p)
			{
				#ifdef VMEM_ENABLE_MEMSET
					CheckMemory(p_decommit_node->mp_Mem, p_decommit_node->m_Size, VMEM_DECOMMITTED_MEM);
				#endif

				// only commit as much of the node as we need
				void* p_node_end = (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size;
				size_t size_to_commit = VMem::VMin((size_t)((byte*)p_node_end - (byte*)p), total_bytes_to_commit);

				// reduce the size of the commit node
				p_decommit_node->m_Size = (byte*)p - (byte*)p_decommit_node->mp_Mem;
				p_decommit_node->m_CommitFlags = commit_flags;

				// re-add range to map
				DecommitNodeMap::Range range(p_decommit_node->mp_Mem, p, p_decommit_node);
				bool add_result = m_DecommitNodeMap.Add(range);
				VMEM_ASSERT(add_result, "add shouldn't fail because we've just removed an item");
				VMEM_UNREFERENCED_PARAM(add_result);

				VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

				p = (byte*)p + size_to_commit;
				total_bytes_to_commit -= size_to_commit;
				committed_bytes += size_to_commit;

				m_PendingDecommitBytes -= size_to_commit;

				// if we have some left over, create a new node
				if(p < p_node_end)
				{
					// allocate new node
					DecommitNode* p_new_node = m_DecommitNodePool.Alloc();
					
					size_t new_node_size = (byte*)p_node_end - (byte*)p;

					if(p_new_node)
					{
						// setup new node
						p_new_node->mp_Mem = p;
						p_new_node->m_Size = new_node_size;
						p_new_node->m_CommitFlags = commit_flags;
						p_new_node->m_FlushFrame = p_decommit_node->m_FlushFrame;
						VMEM_ASSERT_CODE(p_new_node->mp_Prev = p_new_node->mp_Next = NULL);
					
						// insert new node into list
						m_DecommitNodeList.Insert(p_decommit_node, p_new_node);

						// add new range to map
						DecommitNodeMap::Range offcut_range(p, (byte*)p + new_node_size, p_new_node);
						if(!m_DecommitNodeMap.Add(offcut_range))
						{
							m_DecommitNodeList.Remove(p_new_node);
							m_DecommitNodePool.Free(p_new_node);
							if(committed_bytes)
								VirtualMem::Decommit(p_commit, committed_bytes, commit_flags);
							return false;
						}

						VMEM_MEMSET(p_new_node->mp_Mem, VMEM_DECOMMITTED_MEM, p_new_node->m_Size);
					}
					else
					{
						// OOM so decommit the offcut range instead of adding it to the map
						VMem::VirtualDecommit(p, new_node_size, m_PageSize, commit_flags);
					}

					return true;
				}

				if(!total_bytes_to_commit)
					return true;

				// try and get the next range
				p_decommit_node = NULL;
				m_DecommitNodeMap.TryRemoveRange(p, total_bytes_to_commit, p_decommit_node);
			}

			// keep committing nodes and the gaps until we have committed enough
			while(total_bytes_to_commit && p_decommit_node)
			{
				#ifdef VMEM_ENABLE_MEMSET
					CheckMemory(p_decommit_node->mp_Mem, p_decommit_node->m_Size, VMEM_DECOMMITTED_MEM);
				#endif

				// if the node starts part way through the block commit the range before the node
				void* p_node_start = p_decommit_node->mp_Mem;
				if(p < p_node_start)
				{
					size_t size_to_commit = (byte*)p_node_start - (byte*)p;
					VMEM_ASSERT(size_to_commit < total_bytes_to_commit, "m_DecommitNodeMap has been corrupted");

					if(!VMem::VirtualCommit(p, size_to_commit, m_PageSize, commit_flags))
					{
						if(committed_bytes)
							VMem::VirtualDecommit(p_commit, committed_bytes, m_PageSize, commit_flags);
						bool add_result = m_DecommitNodeMap.Add(DecommitNodeMap::Range(p_decommit_node->mp_Mem, (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size, p_decommit_node));
						VMEM_ASSERT(add_result, "add should never fail due to oom because we just removed an item");
						VMEM_UNREFERENCED_PARAM(add_result);
						return false;
					}

					p = (byte*)p + size_to_commit;
					total_bytes_to_commit -= size_to_commit;
					committed_bytes += size_to_commit;
				}

				VMEM_ASSERT(total_bytes_to_commit, "node doesn't overlap?");
				VMEM_ASSERT(p_decommit_node->mp_Mem == p, "Decommit node has been corrupted.");

				// commit as much of the node as we need
				size_t size_to_commit = VMem::VMin(p_decommit_node->m_Size, total_bytes_to_commit);

				p_decommit_node->mp_Mem = (byte*)p_decommit_node->mp_Mem + size_to_commit;
				p_decommit_node->m_Size = p_decommit_node->m_Size - size_to_commit;
				p_decommit_node->m_CommitFlags = commit_flags;

				VMEM_ASSERT(p_decommit_node->m_CommitFlags == commit_flags, "can't re-commit with different physical flags");

				p = (byte*)p + size_to_commit;
				total_bytes_to_commit -= size_to_commit;
				committed_bytes += size_to_commit;
				m_PendingDecommitBytes -= size_to_commit;

				if(p_decommit_node->m_Size)
				{
					DecommitNodeMap::Range range(p_decommit_node->mp_Mem, (byte*)p_decommit_node->mp_Mem + p_decommit_node->m_Size, p_decommit_node);
					bool add_result = m_DecommitNodeMap.Add(range);
					VMEM_ASSERT(add_result, "add should never fail due to oom because we just removed an item");
					VMEM_UNREFERENCED_PARAM(add_result);
				}
				else
				{
					m_DecommitNodeList.Remove(p_decommit_node);
					m_DecommitNodePool.Free(p_decommit_node);
				}
			
				if(!total_bytes_to_commit)
					return true;

				// try and get the next range
				p_decommit_node = NULL;
				m_DecommitNodeMap.TryRemoveRange(p, total_bytes_to_commit, p_decommit_node);
			}

			if(total_bytes_to_commit)
			{
				if(!VMem::VirtualCommit(p, total_bytes_to_commit, m_PageSize, commit_flags))
				{
					VirtualMem::Decommit(p_commit, committed_bytes, commit_flags);
					return false;
				}
			}

			return true;
		}
	}
#endif

	//------------------------------------------------------------------------
	void VirtualMem::Decommit(void* p, size_t size, int commit_flags)
	{
#ifdef VMEM_DECOMMIT_CACHE
		if(size > VMEM_DECOMMIT_CACHE_MAX_SIZE)
		{
			VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);
		}
		else
		{
			CriticalSectionScope lock(m_CriticalSection);

			VMEM_ASSERT(p != NULL && ((size_t)p & (m_PageSize-1)) == 0, "Invalid address passed to VirtualDecommit");
			VMEM_ASSERT(size >= 0 && (size & (m_PageSize-1)) == 0, "Invalid size passed to VirtualDecommit");
			VMEM_ASSERT(VMem::Committed(p, size), "Can't decommit range because it is not committed");
	
			VMEM_MEMSET(p, VMEM_DECOMMITTED_MEM, size);

			DecommitNode* p_new_node = m_DecommitNodePool.Alloc();
			if(!p_new_node)
			{
				VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);		// out of memory so decommit immediately
				return;
			}

			m_PendingDecommitBytes += size;

			p_new_node->mp_Mem = p;
			p_new_node->m_Size = size;
			p_new_node->m_CommitFlags = commit_flags;
			p_new_node->mp_Prev = NULL;
			p_new_node->mp_Next = NULL;
			p_new_node->m_FlushFrame = m_CurrentFrame + VMEM_DECOMMIT_CACHE_RELEASE_FRAME_DELAY;
			m_DecommitNodeList.AddHead(p_new_node);

			DecommitNodeMap::Range range(p, (byte*)p + size, p_new_node);
			if(!m_DecommitNodeMap.Add(range))
			{
				m_PendingDecommitBytes -= size;
				m_DecommitNodeList.Remove(p_new_node);
				m_DecommitNodePool.Free(p_new_node);
				VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);		// out of memory so decommit immediately
				return;
			}

			VMEM_MEMSET(p_new_node->mp_Mem, VMEM_DECOMMITTED_MEM, p_new_node->m_Size);

			if(m_PendingDecommitBytes > VMEM_DECOMMIT_CACHE_SIZE)
				FlushDecommitCache();
		}
#else
		VMem::VirtualDecommit(p, size, m_PageSize, commit_flags);
#endif
	}

	//------------------------------------------------------------------------
	void VirtualMem::Flush()
	{
		CriticalSectionScope lock(m_CriticalSection);

#ifdef VMEM_DECOMMIT_CACHE
		FlushMemory(true);
#endif
	}

	//------------------------------------------------------------------------
	void VirtualMem::Update()
	{
	#ifdef VMEM_DECOMMIT_CACHE
		CriticalSectionScope lock(m_CriticalSection);

		FlushMemory(false);

		++m_CurrentFrame;
	#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushDecommitCache(bool force)
	{
		DecommitNode* p_node = m_DecommitNodeList.GetTail();
		DecommitNode* p_node_end = m_DecommitNodeList.GetIterEnd();
		
		while(p_node != p_node_end && (m_PendingDecommitBytes > VMEM_DECOMMIT_CACHE_SIZE || m_CurrentFrame >= p_node->m_FlushFrame || force))
		{
			DecommitNode* p_prev = p_node->mp_Prev;

			DecommitDecommitNode(p_node);

			p_node = p_prev;
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::DecommitDecommitNode(DecommitNode* p_node)
	{
		size_t size = p_node->m_Size;

		VMem::VirtualDecommit(p_node->mp_Mem, size, m_PageSize, p_node->m_CommitFlags);

		m_PendingDecommitBytes -= size;

		DecommitNodeMap::Range range(p_node->mp_Mem, (byte*)p_node->mp_Mem + size, p_node);
		m_DecommitNodeMap.Remove(range);

		m_DecommitNodeList.Remove(p_node);
		m_DecommitNodePool.Free(p_node);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushMemory(bool force)
	{
		// if m_FlushFrame wraps around then all flush frames will be invalid, so simply flush everything
		if(!m_CurrentFrame)
			force = true;

		// must flush the decommit cache first so that we don't release while still committed
		FlushDecommitCache(force);

		FlushReleaseCache(force);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_DECOMMIT_CACHE
	void VirtualMem::FlushReleaseCache(bool force)
	{
		for(int i=0; i<m_ReleaseNodeBucketCount; ++i)
		{
			List<ReleaseNode>& node_list = m_ReleaseNodeBuckets[i];
			ReleaseNode* p_node = node_list.GetTail();
			ReleaseNode* p_node_end = node_list.GetIterEnd();
			while(p_node != p_node_end && (m_CurrentFrame >= p_node->m_FlushFrame || force))
			{
				ReleaseNode* p_prev = p_node->mp_Prev;

				size_t size = p_node->m_Size;
				size_t aligned_size = p_node->m_AlignedSize;

				VMem::VirtualRelease(p_node->mp_Mem, aligned_size);

				m_PendingReleaseBytes -= size;
				VMEM_STATS(m_ReservedOverhead -= aligned_size - size);

				node_list.Remove(p_node);

				m_ReleaseNodePool.Free(p_node);

				p_node = p_prev;
			}
		}
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats VirtualMem::GetStats()
	{
		CriticalSectionScope lock(m_CriticalSection);
		return GetStatsNoLock();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats VirtualMem::GetStatsNoLock() const
	{
		size_t allocs_overhead = m_Reservations.GetAllocedMemory();

		Stats stats;
		stats.m_Reserved = m_ReservedOverhead + allocs_overhead;
		stats.m_Overhead = allocs_overhead;

#ifdef VMEM_DECOMMIT_CACHE
		stats.m_Unused = m_PendingDecommitBytes;
		stats.m_Reserved += m_PendingReleaseBytes;
		stats += m_DecommitNodeMap.GetStats();
		stats += m_ReleaseNodePool.GetStats();
		stats += m_DecommitNodePool.GetStats();
#endif
		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void VirtualMem::Lock() const
	{
		m_CriticalSection.Enter();
	}

	//------------------------------------------------------------------------
	void VirtualMem::Release() const
	{
		m_CriticalSection.Leave();
	}

	//------------------------------------------------------------------------
	void VirtualMem::CheckIntegrity()
	{
		{
			CriticalSectionScope lock(m_CriticalSection);

			#if defined(VMEM_ASSERTS) && defined(VMEM_DECOMMIT_CACHE) && defined(VMEM_ENABLE_MEMSET)
				DecommitNode* p_node = m_DecommitNodeList.GetHead();
				DecommitNode* p_end_node = m_DecommitNodeList.GetIterEnd();
				while (p_node != p_end_node)
				{
					CheckMemory(p_node->mp_Mem, p_node->m_Size, VMEM_DECOMMITTED_MEM);
					p_node = p_node->mp_Next;
				}
			#endif
		}

		VMemSysCheckIntegrity();
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// AlignedCoalesceHeap.h
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class AlignedCoalesceHeap
	{
	public:
		//------------------------------------------------------------------------
		AlignedCoalesceHeap(
			int region_size,
			int min_size,
			int max_size,
			InternalHeap& internal_heap,
			HeapRegions& heap_regions,
			RegionType::Enum region_type,
			VirtualMem& virtual_mem,
			int reserve_flags,
			int commit_flags)
		:
			m_CoalesceHeap(
				region_size,
				min_size,
				max_size,
				internal_heap,
				heap_regions,
				region_type,
				reserve_flags,
				commit_flags,
				virtual_mem)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			return m_CoalesceHeap.Initialise();
		}

		//------------------------------------------------------------------------
		void* Alloc(size_t size, size_t alignment)
		{
			size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

			void* p = m_CoalesceHeap.Alloc(aligned_size);
			if(!p)
				return NULL;

#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* aligned_p = (byte*)AlignUp((byte*)p + sizeof(AlignedHeader) + sizeof(VMemCustomAllocInfo), alignment) - sizeof(VMemCustomAllocInfo);
#else
			void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);
#endif
			AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
			p_header->p = p;

			return aligned_p;
		}

		//------------------------------------------------------------------------
		void Free(void* p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			m_CoalesceHeap.Free(p_header->p);
		}

		//------------------------------------------------------------------------
		static size_t GetSize(void* p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			return CoalesceHeap::GetSize(p_header->p) - ((byte*)p - (byte*)p_header->p);
		}

		//------------------------------------------------------------------------
		void Lock() const { m_CoalesceHeap.Lock(); }
		void Release() const { m_CoalesceHeap.Release(); }

		//------------------------------------------------------------------------
		void Update()
		{
			m_CoalesceHeap.Update();
		}

		//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
		Stats GetStats() const { return m_CoalesceHeap.GetStats(); }
		Stats GetStatsNoLock() const { return m_CoalesceHeap.GetStatsNoLock(); }
		size_t WriteAllocs() const { return m_CoalesceHeap.WriteAllocs(); }
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context) { m_CoalesceHeap.SendStatsToMemPro(send_fn, p_context); }
#endif
		//------------------------------------------------------------------------
		// data
	private:
		CoalesceHeap m_CoalesceHeap;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemHeap.h
//
#ifdef VMEM_FSA_HEAP_PER_THREAD
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//#define VMEM_HEAP_GLOBAL_LOCK

//------------------------------------------------------------------------
#ifdef VMEM_HEAP_GLOBAL_LOCK
	#define VMEM_GLOBAL_LOCK CriticalSectionScope lock(m_CriticalSection);
#else
	#define VMEM_GLOBAL_LOCK
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#if defined(VMEM_FSA_HEAP_PER_THREAD) || defined(VMEM_COALESCE_HEAP_PER_THREAD)
	class VMemHeapTLS
	{
	public:
		VMemHeapTLS(PageHeap* p_fsa1_page_heap, InternalHeap& internal_heap)
		:	m_FSAHeap1(p_fsa1_page_heap, internal_heap),
			mp_CoalesceHeap(NULL),
			mp_Next(NULL)
		{
		}

		FSAHeap m_FSAHeap1;

		std::atomic<CoalesceHeap*> mp_CoalesceHeap;

		VMemHeapTLS* mp_Next;
	};
#endif

	//------------------------------------------------------------------------
	class VMemHeap
	{
	public:
		//------------------------------------------------------------------------
		VMemHeap(int page_size, int reserve_flags, int commit_flags);

		~VMemHeap();

		bool Initialise();

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		void CreateCoalesceHeapForThisThread();
#endif
		void* Alloc(size_t size, size_t alignment);

		void Free(void* p);

		size_t GetSize(void* p);

		bool Owns(void* p);

		void Flush();

		void CheckIntegrity();

#ifdef VMEM_ENABLE_STATS
		VMemHeapStats GetStatsNoLock() const;
		size_t WriteAllocs();
		void SendStatsToMemPro(MemProSendFn send_fn, void* p_context);
#endif
		InternalHeap& GetInternalHeap() { return m_InternalHeap; }

#ifdef VMEM_SERVICE_THREAD
		void Update();
#endif

		void AcquireLockShared() const;
		void ReleaseLockShared() const;

	private:
		bool InitialiseFSAHeap1(FSAHeap& fsa_heap);
		bool InitialiseFSAHeap2(FSAHeap& fsa_heap);

		void InitialiseTrailGuards();

		VMEM_NO_INLINE void* AlignedAlloc(size_t size, size_t alignment);

#ifdef VMEM_INC_INTEG_CHECK
		void IncIntegrityCheck();
#endif

#ifdef VMEM_FSA_HEAP_PER_THREAD
		VMEM_NO_INLINE VMemHeapTLS* CreateVMemHeapTLS();

		VMEM_FORCE_INLINE VMemHeapTLS* GetVMemHeapTLS();
#endif

		//------------------------------------------------------------------------
		// data
	private:
		VirtualMem m_VirtualMem;

		int m_PageSize;

		void* mp_FSA1PageHeapRegion;
		void* mp_FSA2PageHeapRegion;
		void* mp_CoalesceHeap1Region;
		void* mp_CoalesceHeap2Region;

		InternalHeap m_InternalHeap;

		HeapRegions m_HeapRegions;

		PageHeap m_FSA1PageHeap;
		PageHeap m_FSA2PageHeap;

#ifdef VMEM_FSA_HEAP_PER_THREAD
		mutable CriticalSection m_VMemHeapTLSListLock;
		VMemHeapTLS* mp_VMemHeapTLSList;
#else
		FSAHeap m_FSAHeap1;
#endif
		FSAHeap m_FSAHeap2;

		CoalesceHeap m_CoalesceHeap1;
		CoalesceHeap m_CoalesceHeap2;

		LargeHeap m_LargeHeap;

		AlignedCoalesceHeap m_AlignedCoalesceHeap;
		AlignedLargeHeap m_AlignedLargeHeap;

		Stats m_Stats;

#ifdef VMEM_HEAP_GLOBAL_LOCK
		CriticalSection m_CriticalSection;
#endif

#ifdef VMEM_PROTECTED_HEAP
		ProtectedHeap m_ProtectedHeap;
#endif
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// ServiceThread.h
//
#ifdef VMEM_SERVICE_THREAD
	#include <atomic>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class ServiceThread
	{
	public:
		typedef void (*UpdateFn)();

		//------------------------------------------------------------------------
		ServiceThread(UpdateFn update_fn)
		:	m_UpdateFn(update_fn),
			m_ServiceThread(ThreadHandle()),
			m_Running(true),
			m_WakeEvent(false, true),
			m_ThreadFinishedEvent(false, false)
		{
		}

		//------------------------------------------------------------------------
		bool Initialise()
		{
			m_ServiceThread = Thread::CreateThread(ThreadMainStatic, this);

			if(!m_ServiceThread)
			{
				m_Running = false;
				m_ThreadFinishedEvent.Set();
				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------
		static int ThreadMainStatic(void* p_context)
		{
			ServiceThread* p_this = (ServiceThread*)p_context;
			p_this->ThreadMain();
			return 0;
		}

		//------------------------------------------------------------------------
		void ThreadMain()
		{
			while(m_Running)
			{
				m_UpdateFn();

				m_WakeEvent.Wait(VMEM_SERVICE_THREAD_PERIOD);
			}

			m_ThreadFinishedEvent.Set();
		}

		//------------------------------------------------------------------------
		void Stop()
		{
			m_Running = false;
			m_WakeEvent.Set();
			m_ThreadFinishedEvent.Wait();
		}

		//------------------------------------------------------------------------
		// data
	private:
		UpdateFn m_UpdateFn;

		ThreadHandle m_ServiceThread;
		std::atomic<bool> m_Running;
		Event m_WakeEvent;
		Event m_ThreadFinishedEvent;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemAlloc.cpp
//
#include <stdio.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
// write each allocation and free out to a binary file. Each operation is of
// the form <pointer (4 bytes), size (4 bytes)>
// Free operations are identified by a size of VMEM_INVALID_SIZE
// Playing back the exact allocations and frees can be useful for tracking down bugs.
//#define VMEM_RECORD_ALLOCS

//------------------------------------------------------------------------
#if defined(VMEM_RECORD_ALLOCS)
	#if defined(VMEM_PLATFORM_LINUX) || defined(VMEM_PLATFORM_APPLE) || defined(VMEM_PLATFORM_ANDROID)
		#ifdef __GLIBC__
			#include <sched.h>
		#else
			#include <linux/getcpu.h>
		#endif
	#endif

	#if defined(VMEM_PLATFORM_SWITCH)
		#error Please contact slynch@puredevsoftware.com for this platform
	#endif
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	#define VMEM_CUSTOM_HEAP_MARKER 0xc5084ea6c5084ea6ULL
	#define VMEM_PHYSICAL_CUSTOM_HEAP_MARKER 0x64ea440064ea4400ULL

	//------------------------------------------------------------------------
	struct VMemMain
	{
		VMemMain()
		:	m_MainHeap(VMEM_SYS_PAGE_SIZE, VMEM_DEFAULT_RESERVE_FLAGS, VMEM_DEFAULT_COMMIT_FLAGS),
			mp_CustomHeaps(NULL),
			mp_PhysicalCustomHeaps(NULL)
		{
		}

		VMemHeap m_MainHeap;

		CriticalSection m_CriticalSection;

		struct CustomHeap* mp_CustomHeaps;

		struct PhysicalCustomHeap* mp_PhysicalCustomHeaps;
	};

	//------------------------------------------------------------------------
	VMEM_THREAD_LOCAL HeapHandle g_ThreadHeapHandle;

	//------------------------------------------------------------------------
	VMemHeapStats GetStats_NoLock(VMemMain* p_vmem);

	//------------------------------------------------------------------------
	// we do it this way to avoid order of static initialisation problems
	VMem::byte g_VMemMem[sizeof(VMemMain)] VMEM_ALIGN_8;
	VMemMain* gp_VMem = NULL;

#ifdef VMEM_SERVICE_THREAD
	VMem::byte g_ServiceThreadMem[sizeof(ServiceThread)] VMEM_ALIGN_8;
	ServiceThread* gp_ServiceThread = NULL;
#endif

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE VMemMain* GetVMem()
	{
		VMemMain* p_vmem = gp_VMem;
		VMEM_ASSERT(p_vmem, "VMem has not been initialised!");
		return p_vmem;
	}

	//------------------------------------------------------------------------
	struct CustomHeap
	{
		CustomHeap(
			int page_size,
			int reserve_flags,
			int commit_flags)
		:
			m_Heap(page_size, reserve_flags, commit_flags)
#ifdef VMEM_ASSERTS
			,m_Marker(VMEM_CUSTOM_HEAP_MARKER)
#endif
		{
		}
		VMemHeap m_Heap;
		CustomHeap* mp_Next;

		#ifdef VMEM_ASSERTS
			uint64 m_Marker;
		#endif
	};

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	struct PhysicalCustomHeap
	{
		PhysicalCustomHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
		:	m_Heap(coalesce_heap_region_size, coalesce_heap_max_size, flags)
			#ifdef VMEM_ASSERTS
			,m_Marker(VMEM_PHYSICAL_CUSTOM_HEAP_MARKER)
			#endif
		{
		}

		PhysicalHeap m_Heap;
		PhysicalCustomHeap* mp_Next;

		#ifdef VMEM_ASSERTS
			uint64 m_Marker;
		#endif
	};
#endif

	//------------------------------------------------------------------------
	VMEM_NO_INLINE void OutOfMemoryHandler()
	{
		DebugWrite(_T("Out of memory!\n"));

#ifdef VMEM_ENABLE_STATS
		WriteStats();
#endif
		DumpSystemMemory();
	}

	//------------------------------------------------------------------------
	class TeardownIntegrityCheck
	{
	public:
		~TeardownIntegrityCheck() { CheckIntegrity(); }
	};

	TeardownIntegrityCheck g_TeardownIntegrityCheck;

	//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP
	VMemSHouldProtectFn g_VMemSHouldProtectFn = NULL;
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	VMem::byte g_RecordCritSecMem[sizeof(CriticalSection)] VMEM_ALIGN_8;
	CriticalSection& g_RecordCritSec = (CriticalSection&)g_RecordCritSecMem;
	FILE* gp_RecordFile = NULL;
	void InitialiseRecording()
	{
		new (&g_RecordCritSec)CriticalSection();

		#ifdef VMEM_PLATFORM_PS4
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			const char* p_filename = "allocs.bin";
		#endif

		#ifdef VMEM_PLATFORM_PS4
			#error Please contact slynch@puredevsoftware.com for this platform
		#elif defined(VMEM_OS_WIN)
			fopen_s(&gp_RecordFile, p_filename, "wb");
		#else
			gp_RecordFile = fopen(p_filename, "wb");
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void UninitialiseRecording()
	{
		#ifdef VMEM_PLATFORM_PS4
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			fclose(gp_RecordFile);
		#endif
		g_RecordCritSec.~CriticalSection();
	}
#endif


	//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD
	void ServiceThreadUpdate()
	{
		VMemMain* p_vmem = gp_VMem;

		if(p_vmem)
		{
			CriticalSectionScope lock(p_vmem->m_CriticalSection);

			p_vmem->m_MainHeap.Update();

			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
				p_heap->m_Heap.Update();
		}
	}
#endif

	//------------------------------------------------------------------------
	bool InitialiseVMem()
	{
#ifdef VMEM_RECORD_ALLOCS
		InitialiseRecording();
#endif
		// only one thhread will make the lock, so when another thread gets
		// the lock it must check that it hasn't already been initialised
		if(!gp_VMem)
		{
			InitialiseCore();

			#ifdef VMEM_SERVICE_THREAD
				Thread::Initialise();
			#endif

			VMemMain* p_vmem = (VMemMain*)g_VMemMem;
			VMEM_MEMSET((void*)p_vmem, VMEM_ALLOCATED_MEM, sizeof(g_VMemMem));

			new (p_vmem) VMemMain();

			gp_VMem = p_vmem;

			if(!p_vmem->m_MainHeap.Initialise())
			{
				p_vmem->~VMemMain();
				UninitialiseCore();
				#ifdef VMEM_SERVICE_THREAD
					Thread::Destroy();
				#endif
				gp_VMem = NULL;
				return false;
			}

			// must do this LAST because creating a thread can call malloc, and if malloc
			// is overridden we can get into an inifnite loop!
			// as long as gp_VMem has been set we should avoid infinity.
#ifdef VMEM_SERVICE_THREAD
			gp_ServiceThread = (ServiceThread*)g_ServiceThreadMem;
			new (gp_ServiceThread) ServiceThread(ServiceThreadUpdate);

			if(!gp_ServiceThread->Initialise())
				return false;
#endif
		}

		return true;
	}

	//------------------------------------------------------------------------
	// we need to initialise VMem in global startup because the initialise is
	// not thread safe. Global startup should happen before any threads are
	// created. If you are creating threads in global startup then the results
	// are undefined.
	class ForceInitialisationDuringGlobalStartup
	{
	public:
		ForceInitialisationDuringGlobalStartup()
		{
			if(!gp_VMem)
			{
				if(!InitialiseVMem())
					VMem::Break();		// failed to initialise VMem
			}
		}
	} g_ForceInitialisationDuringGlobalStartup;

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void WriteToRecordingFile(void* p_data, size_t size)
	{
		#if defined(VMEM_PLATFORM_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			fwrite(p_data, size, 1, gp_RecordFile);
		#endif
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_RECORD_ALLOCS
	void Record(void* p, size_t size, size_t alignment, void* p_new)
	{
		uint64 time = 0;
		int cpuid = 0;

		#ifdef VMEM_OS_WIN
			QueryPerformanceCounter((LARGE_INTEGER*)&time);
			cpuid = GetCurrentProcessorNumber();
		#elif defined(__GLIBC__)
			cpuid = sched_getcpu();
		#elif defined(VMEM_PLATFORM_PS4)
			#error Please contact slynch@puredevsoftware.com for this platform
		#else
			unsigned int node = 0;
			getcpu((unsigned int*)&cpuid, &node, NULL);
		#endif

		g_RecordCritSec.Enter();
		WriteToRecordingFile(&p, sizeof(p));
		WriteToRecordingFile(&size, sizeof(size));
		WriteToRecordingFile(&alignment, sizeof(size));
		WriteToRecordingFile(&p_new, sizeof(p_new));
		WriteToRecordingFile(&time, sizeof(time));
		WriteToRecordingFile(&cpuid, sizeof(cpuid));
		g_RecordCritSec.Leave();
	}
	#define VMEM_RECORD(addr, byte_count, alignment, new_addr) Record(addr, byte_count, alignment, new_addr);
#else
	#define VMEM_RECORD(addr, byte_count, alignment, new_addr)
#endif

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* Alloc(size_t size, size_t alignment)
	{
		VMemMain* p_vmem = gp_VMem;
		if(!p_vmem)
		{
			if(!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		size_t size_with_info;
#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_STATIC_ASSERT((sizeof(VMemCustomAllocInfo) % VMEM_NATURAL_ALIGNMENT) == 0, "invalid size for VMemCustomAllocInfo");
		size_with_info = size + sizeof(VMemCustomAllocInfo);
#else
		size_with_info = size;
#endif
		void* p = p_vmem->m_MainHeap.Alloc(size_with_info, alignment);

		if(!p)
		{
			Flush();
			p = p_vmem->m_MainHeap.Alloc(size_with_info, alignment);
			if(!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		memset(p, 0, sizeof(VMemCustomAllocInfo));
		p = (VMem::byte*)p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, size, alignment, NULL);

		return p;
	}

	//------------------------------------------------------------------------
	void Free(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		GetVMem()->m_MainHeap.Free(p_alloc);

		VMEM_RECORD(p, VMEM_INVALID_SIZE, VMEM_INVALID_SIZE, NULL);
	}

	//------------------------------------------------------------------------
	size_t GetSize(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetVMem()->m_MainHeap.GetSize(p_alloc);
	}

	//------------------------------------------------------------------------
	bool Owns(void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetVMem()->m_MainHeap.Owns(p_alloc);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* Realloc(void* p, size_t new_size, size_t alignment)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		size_t new_alloc_size = new_size + sizeof(VMemCustomAllocInfo);
#else
		size_t new_alloc_size = new_size;
#endif
		VMemMain* p_vmem = gp_VMem;
		if(!p_vmem)
		{
			if(!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		void* new_p = NULL;
		if(p)
		{
#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
			void* p_alloc = p;
#endif
			size_t old_size = p_vmem->m_MainHeap.GetSize(p_alloc);

			bool new_alloc = true;
			if(new_alloc_size <= old_size)
			{
				if(new_alloc_size <= 512)
					new_alloc = new_alloc_size > old_size || new_alloc_size < old_size/2;
				else
					new_alloc = new_alloc_size > old_size || new_alloc_size < 6*old_size/8;
			}

			if(new_alloc)
			{
				new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
				if(!new_p)
				{
					p_vmem->m_MainHeap.Flush();
					new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
					if(!new_p)
					{
						OutOfMemoryHandler();
						return NULL;
					}
				}

				VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
				size_t copy_size = VMin(old_size, new_alloc_size);
				memcpy(new_p, p_alloc, copy_size);
				p_vmem->m_MainHeap.Free(p_alloc);
			}
			else
			{
				new_p = p_alloc;
			}
		}
		else
		{
			new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
			if(!new_p)
			{
				Flush();
				new_p = p_vmem->m_MainHeap.Alloc(new_alloc_size, alignment);
				if(!new_p)
				{
					OutOfMemoryHandler();
					return NULL;
				}
			}

			#ifdef VMEM_CUSTOM_ALLOC_INFO
				memset(new_p, 0, sizeof(VMemCustomAllocInfo));
			#endif
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		new_p = (VMem::byte*)new_p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, new_size, alignment, new_p);

		return new_p;
	}

	//------------------------------------------------------------------------
	void* AllocAligned(size_t size, size_t alignment)
	{
		if (alignment == 0)
			alignment = VMem::VMax<size_t>(VMem::VMin<size_t>(size, VMEM_NATURAL_ALIGNMENT), 4);

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

		void* p = Alloc(aligned_size);
		if(!p)
		{
			Flush();
			p = Alloc(aligned_size);
			if(!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		return aligned_p;
	}

	//------------------------------------------------------------------------
	void* ReallocAligned(void* p, size_t new_size, size_t alignment)
	{
		void* new_p = AllocAligned(new_size, alignment);
		if(p)
		{
			size_t old_size = GetSizeAligned(p);
			VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
			size_t copy_size = VMem::VMin(old_size, new_size);
			if(new_p)
				memcpy_s(new_p, new_size, p, copy_size);
			FreeAligned(p);
		}
		return new_p;
	}

	//------------------------------------------------------------------------
	void FreeAligned(void* p)
	{
		if (p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			Free(p_header->p);
		}
	}

	//------------------------------------------------------------------------
	size_t GetSizeAligned(void* p)
	{
		if(!p)
			return VMEM_INVALID_SIZE;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;
		size_t aligned_size = GetSize(p_alloc);
		return aligned_size - ((byte*)p - (byte*)p_alloc);
	}

	//------------------------------------------------------------------------
	bool Initialise()
	{
		return gp_VMem ? true : InitialiseVMem();
	}

	//------------------------------------------------------------------------
	void DestroyAllCustomHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		CustomHeap* p_heap = p_vmem->mp_CustomHeaps;
		while (p_heap)
		{
			CustomHeap* p_next = p_heap->mp_Next;
			p_heap->~CustomHeap();
			Free(p_heap);
			p_heap = p_next;
		}
		p_vmem->mp_CustomHeaps = NULL;
	}

	//------------------------------------------------------------------------
	void DestroyAllPhysicalCustomHeaps()
	{
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
		VMemMain* p_vmem = GetVMem();

		PhysicalCustomHeap* p_heap = p_vmem->mp_PhysicalCustomHeaps;
		while (p_heap)
		{
			PhysicalCustomHeap* p_next = p_heap->mp_Next;
			p_heap->~PhysicalCustomHeap();
			Free(p_heap);
			p_heap = p_next;
		}
		p_vmem->mp_PhysicalCustomHeaps = NULL;
#endif
	}

	//------------------------------------------------------------------------
	void Destroy()
	{
#ifdef VMEM_SERVICE_THREAD
		if(gp_ServiceThread)
		{
			gp_ServiceThread->Stop();
			gp_ServiceThread->~ServiceThread();
			gp_ServiceThread = NULL;
		}
#endif
		if(gp_VMem)
		{
			DestroyAllCustomHeaps();
			DestroyAllPhysicalCustomHeaps();

			gp_VMem->~VMemMain();
			gp_VMem = NULL;

			UninitialiseCore();

			#ifdef VMEM_SERVICE_THREAD
				Thread::Destroy();
			#endif
		}

#ifdef VMEM_RECORD_ALLOCS
		UninitialiseRecording();
#endif
	}

	//------------------------------------------------------------------------
	void Flush()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		p_vmem->m_MainHeap.Flush();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.Flush();
	}

	//------------------------------------------------------------------------
	void LockAllHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		p_vmem->m_MainHeap.AcquireLockShared();
		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.AcquireLockShared();
	}

	//------------------------------------------------------------------------
	void UnlockAllHeaps()
	{
		VMemMain* p_vmem = GetVMem();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			p_heap->m_Heap.ReleaseLockShared();
		p_vmem->m_MainHeap.ReleaseLockShared();
	}

	//------------------------------------------------------------------------
	void CheckStats_NoLock(VMemMain* p_vmem)
	{
#ifdef VMEM_ENABLE_STATS
		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			stats += p_heap->m_Heap.GetStatsNoLock();

		// check
		VMEM_ASSERT(stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
		VMEM_ASSERT(stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

		UnlockAllHeaps();
#else
		VMEM_UNREFERENCED_PARAM(p_vmem);
#endif
	}

	//------------------------------------------------------------------------
	void CheckStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CheckStats_NoLock(p_vmem);
	}

	//------------------------------------------------------------------------
	void CheckIntegrity()
	{
		VMemMain* p_vmem = gp_VMem;

		if(p_vmem)
		{
			CriticalSectionScope lock(p_vmem->m_CriticalSection);

			p_vmem->m_MainHeap.CheckIntegrity();

			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
				p_heap->m_Heap.CheckIntegrity();

			#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
				for (PhysicalCustomHeap* p_heap = p_vmem->mp_PhysicalCustomHeaps; p_heap; p_heap = p_heap->mp_Next)
					p_heap->m_Heap.CheckIntegrity();
			#endif

			CheckStats_NoLock(p_vmem);
		}
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		return GetStats_NoLock(p_vmem);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetStats_NoLock(VMemMain* p_vmem)
	{
		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			stats += p_heap->m_Heap.GetStatsNoLock();

		UnlockAllHeaps();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats GetMainHeapStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		UnlockAllHeaps();

		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		LockAllHeaps();

		VMemHeapStats stats = p_vmem->m_MainHeap.GetStatsNoLock();

		if(p_vmem->mp_CustomHeaps)
		{
			DebugWrite(_T("MainHeap ---------------------------------------------------------------------------------------------------------\n"));
			WriteStats(stats);

			VMemHeapStats total_stats = stats;

			int index = 0;
			for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next, ++index)
			{
				VMemHeapStats heap_stats = p_heap->m_Heap.GetStatsNoLock();
				DebugWrite(_T("\nHeap %d 0x%p --------------------------------------------------------------------------------------\n"), index, p_heap);
				WriteStats(heap_stats);
				total_stats += heap_stats;
			}

			// check
			VMEM_ASSERT(total_stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
			VMEM_ASSERT(total_stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

			DebugWrite(_T("\nTotal ------------------------------------------------------------------------------------------------------------\n"));
			WriteStats(total_stats);
		}
		else
		{
			VMEM_ASSERT(stats.m_Total.m_Reserved == VMem::GetReservedBytes(), "total reserved bytes doesn't match");
			VMEM_ASSERT(stats.m_Total.GetCommittedBytes() == VMem::GetCommittedBytes(), "total committed bytes doesn't match");

			WriteStats(stats);
		}

		VMemSysWriteStats();

		UnlockAllHeaps();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t WriteAllocs()
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		size_t size = p_vmem->m_MainHeap.WriteAllocs();

		for (CustomHeap* p_heap = p_vmem->mp_CustomHeaps; p_heap; p_heap = p_heap->mp_Next)
			size += p_heap->m_Heap.WriteAllocs();

		return size;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void SendStatsToMemPro(void (*send_fn)(void*, int, void*), void* p_context)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		return p_vmem->m_MainHeap.SendStatsToMemPro(send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
	void SetDebugBreak(void (*DebugBreakFn)(const _TCHAR* p_message))
	{
		SetDebugBreakFunction(DebugBreakFn);
	}

	//------------------------------------------------------------------------
	void SetLogFunction(LogFn log_fn)
	{
		VMemSysSetLogFunction(log_fn);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_PROTECTED_HEAP
	void SetVMemShouldProtectFn(bool (*should_protect_fn)(int i, size_t size))
	{
		g_VMemSHouldProtectFn = should_protect_fn;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
	void CreateCoalesceHeapForThisThread()
	{
		return GetVMem()->m_MainHeap.CreateCoalesceHeapForThisThread();
	}
#endif

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE VMemHeap* GetHeap(HeapHandle heap)
	{
		CustomHeap* p_heap = (CustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_CUSTOM_HEAP_MARKER, "Not a VMem heap!");
		return &p_heap->m_Heap;
	}

	//------------------------------------------------------------------------
	HeapHandle CreateHeap(int page_size, int reserve_flags, int commit_flags)
	{
		VMemMain* p_vmem = gp_VMem;
		if (!p_vmem)
		{
			if (!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CustomHeap* p_heap = (CustomHeap*)Alloc(sizeof(CustomHeap));

		VMEM_MEMSET((void*)p_heap, VMEM_ALLOCATED_MEM, sizeof(CustomHeap));

		new (p_heap)CustomHeap(page_size, reserve_flags, commit_flags);

		if(!p_heap->m_Heap.Initialise())
		{
			p_heap->~CustomHeap();
			Free(p_heap);
			return NULL;
		}

		p_heap->mp_Next = p_vmem->mp_CustomHeaps;
		p_vmem->mp_CustomHeaps = p_heap;

		return p_heap;
	}

	//------------------------------------------------------------------------
	void DestroyHeap(HeapHandle heap)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		CustomHeap* p_heap = (CustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_CUSTOM_HEAP_MARKER, "Not a VMem heap!");

		if(p_heap == p_vmem->mp_CustomHeaps)
		{
			p_vmem->mp_CustomHeaps = p_heap->mp_Next;
		}
		else
		{
			bool found = false;
			for(CustomHeap* p_node = p_vmem->mp_CustomHeaps; p_node; p_node = p_node->mp_Next)
			{
				if(p_node->mp_Next == p_heap)
				{
					found = true;
					p_node->mp_Next = p_heap->mp_Next;
					break;
				}
			}
			VMEM_ASSERT(found, "bad heap handle passed in to DestroyHeap");
			VMEM_UNREFERENCED_PARAM(found);
		}

		p_heap->~CustomHeap();

		Free(p_heap);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* HeapAlloc(HeapHandle heap, size_t size, size_t alignment)
	{
		VMemHeap* p_heap = GetHeap(heap);

		size_t size_with_info;
#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_STATIC_ASSERT((sizeof(VMemCustomAllocInfo) % VMEM_NATURAL_ALIGNMENT) == 0, "invalid size for VMemCustomAllocInfo");
		size_with_info = size + sizeof(VMemCustomAllocInfo);
#else
		size_with_info = size;
#endif
		void* p = p_heap->Alloc(size_with_info, alignment);

		if (!p)
		{
			Flush();
			p = p_heap->Alloc(size, alignment);
			if (!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		memset(p, 0, sizeof(VMemCustomAllocInfo));
		p = (VMem::byte*)p + sizeof(VMemCustomAllocInfo);
#endif
		return p;
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* HeapRealloc(HeapHandle heap, void* p, size_t new_size, size_t alignment)
	{
		VMemHeap* p_heap = GetHeap(heap);

#ifdef VMEM_CUSTOM_ALLOC_INFO
		size_t new_alloc_size = new_size + sizeof(VMemCustomAllocInfo);
#else
		size_t new_alloc_size = new_size;
#endif
		void* new_p = NULL;
		if (p)
		{
#ifdef VMEM_CUSTOM_ALLOC_INFO
			void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
			void* p_alloc = p;
#endif
			size_t old_size = p_heap->GetSize(p_alloc);

			bool new_alloc = true;
			if (new_alloc_size <= old_size)
			{
				if (new_alloc_size <= 512)
					new_alloc = new_alloc_size > old_size || new_alloc_size < old_size / 2;
				else
					new_alloc = new_alloc_size > old_size || new_alloc_size < 6 * old_size / 8;
			}

			if (new_alloc)
			{
				new_p = p_heap->Alloc(new_alloc_size, alignment);
				if (!new_p)
				{
					Flush();
					new_p = p_heap->Alloc(new_alloc_size, alignment);
					if (!new_p)
					{
						OutOfMemoryHandler();
						return NULL;
					}
				}

				VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
				size_t copy_size = VMin(old_size, new_alloc_size);
				memcpy(new_p, p_alloc, copy_size);
				p_heap->Free(p_alloc);
			}
			else
			{
				new_p = p_alloc;
			}
		}
		else
		{
			new_p = p_heap->Alloc(new_alloc_size, alignment);
			if (!new_p)
			{
				Flush();
				new_p = p_heap->Alloc(new_alloc_size, alignment);
				if (!new_p)
				{
					OutOfMemoryHandler();
					return NULL;
				}
			}

#ifdef VMEM_CUSTOM_ALLOC_INFO
			memset(new_p, 0, sizeof(VMemCustomAllocInfo));
#endif
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		new_p = (VMem::byte*)new_p + sizeof(VMemCustomAllocInfo);
#endif
		VMEM_RECORD(p, new_size, alignment, new_p);

		return new_p;
	}

	//------------------------------------------------------------------------
	void HeapFree(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		GetHeap(heap)->Free(p_alloc);
	}

	//------------------------------------------------------------------------
	size_t HeapGetSize(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetHeap(heap)->GetSize(p_alloc);
	}

	//------------------------------------------------------------------------
	bool HeapOwns(HeapHandle heap, void* p)
	{
#ifdef VMEM_CUSTOM_ALLOC_INFO
		void* p_alloc = (VMem::byte*)p - sizeof(VMemCustomAllocInfo);
#else
		void* p_alloc = p;
#endif
		return GetHeap(heap)->Owns(p_alloc);
	}

	//------------------------------------------------------------------------
	void* HeapAllocAligned(HeapHandle heap, size_t size, size_t alignment)
	{
		if (alignment == 0)
			alignment = VMem::VMax<size_t>(VMem::VMin<size_t>(size, 16), 4);

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

		void* p = HeapAlloc(heap, aligned_size);
		if (!p)
		{
			Flush();
			p = HeapAlloc(heap, aligned_size);
			if (!p)
			{
				OutOfMemoryHandler();
				return NULL;
			}
		}

		void* aligned_p = AlignUp((byte*)p + sizeof(AlignedHeader), alignment);

		AlignedHeader* p_header = (AlignedHeader*)aligned_p - 1;
		p_header->p = p;

		return aligned_p;
	}

	//------------------------------------------------------------------------
	void* HeapReallocAligned(HeapHandle heap, void* p, size_t new_size, size_t alignment)
	{
		void* new_p = HeapAllocAligned(heap, new_size, alignment);
		if (p)
		{
			size_t old_size = HeapGetSizeAligned(heap, p);
			VMEM_ASSERT(old_size != VMEM_INVALID_SIZE, "trying to realloc invalid pointer");
			size_t copy_size = VMem::VMin(old_size, new_size);
			if (new_p)
				memcpy_s(new_p, new_size, p, copy_size);
			HeapFreeAligned(heap, p);
		}
		return new_p;
	}

	//------------------------------------------------------------------------
	void HeapFreeAligned(HeapHandle heap, void* p)
	{
		if (p)
		{
			AlignedHeader* p_header = (AlignedHeader*)p - 1;
			HeapFree(heap, p_header->p);
		}
	}

	//------------------------------------------------------------------------
	size_t HeapGetSizeAligned(HeapHandle heap, void* p)
	{
		if (!p)
			return VMEM_INVALID_SIZE;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;
		size_t aligned_size = HeapGetSize(heap, p_alloc);
		return aligned_size - ((byte*)p - (byte*)p_alloc);
	}

	//------------------------------------------------------------------------
	size_t HeapOwnsAligned(HeapHandle heap, void* p)
	{
		if (!p)
			return false;

		AlignedHeader* p_header = (AlignedHeader*)p - 1;
		void* p_alloc = p_header->p;

		return HeapOwns(heap, p_alloc);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats HeapGetStats(HeapHandle heap)
	{
		CriticalSectionScope lock(GetVMem()->m_CriticalSection);

		VMemHeap* p_heap = GetHeap(heap);
		p_heap->AcquireLockShared();
		VMemHeapStats stats = p_heap->GetStatsNoLock();
		p_heap->ReleaseLockShared();
		return stats;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void HeapWriteStats(HeapHandle heap)
	{
		VMemHeapStats stats = HeapGetStats(heap);
		WriteStats(stats);
	}
#endif

	//------------------------------------------------------------------------
	void HeapCheckIntegrity(HeapHandle heap)
	{
		GetHeap(heap)->CheckIntegrity();
	}

	//------------------------------------------------------------------------
	VMEM_FORCE_INLINE HeapHandle GetThreadHeap()
	{
		HeapHandle heap = g_ThreadHeapHandle;
		VMEM_ASSERT(heap, "Heap not created for this thread");
		return heap;
	}

	//------------------------------------------------------------------------
	void CreateThreadHeap()
	{
		VMEM_ASSERT(!g_ThreadHeapHandle, "Heap already created for this thread");
		g_ThreadHeapHandle = CreateHeap();
	}

	//------------------------------------------------------------------------
	void DestroyThreadHeap()
	{
		DestroyHeap(GetThreadHeap());
		g_ThreadHeapHandle = NULL;
	}

	//------------------------------------------------------------------------
	void ThreadHeapCheckIntegrity()
	{
		HeapCheckIntegrity(GetThreadHeap());
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* ThreadHeapAlloc(size_t size, size_t alignment)
	{
		return HeapAlloc(GetThreadHeap(), size, alignment);
	}

	//------------------------------------------------------------------------
	VMEM_RESTRICT_RETURN void* ThreadHeapRealloc(void* p, size_t new_size, size_t alignment)
	{
		return HeapRealloc(GetThreadHeap(), p, new_size, alignment);
	}

	//------------------------------------------------------------------------
	void ThreadHeapFree(void* p)
	{
		HeapFree(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	size_t ThreadHeapGetSize(void* p)
	{
		return HeapGetSize(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	void* ThreadHeapAllocAligned(size_t size, size_t alignment)
	{
		return HeapAllocAligned(GetThreadHeap(), size, alignment);
	}

	//------------------------------------------------------------------------
	void* ThreadHeapReallocAligned(void* p, size_t new_size, size_t alignment)
	{
		return HeapReallocAligned(GetThreadHeap(), p, new_size, alignment);
	}

	//------------------------------------------------------------------------
	void ThreadHeapFreeAligned(void* p)
	{
		HeapFreeAligned(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
	size_t ThreadHeapGetSizeAligned(void* p)
	{
		return HeapGetSizeAligned(GetThreadHeap(), p);
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats ThreadHeapGetStats()
	{
		return HeapGetStats(GetThreadHeap());
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void ThreadHeapWriteStats()
	{
		HeapWriteStats(GetThreadHeap());
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	VMEM_FORCE_INLINE PhysicalHeap* GetPhysicalHeap(HeapHandle heap)
	{
		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_PHYSICAL_CUSTOM_HEAP_MARKER, "Not a Physical heap!");
		return &p_heap->m_Heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	HeapHandle CreatePhysicalHeap(size_t coalesce_heap_region_size, size_t coalesce_heap_max_size, int flags)
	{
		VMemMain* p_vmem = gp_VMem;
		if (!p_vmem)
		{
			if (!InitialiseVMem())
				return NULL;
			p_vmem = gp_VMem;
		}

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)Alloc(sizeof(PhysicalCustomHeap));

		VMEM_MEMSET(p_heap, VMEM_ALLOCATED_MEM, sizeof(PhysicalCustomHeap));

		new (p_heap)PhysicalCustomHeap(coalesce_heap_region_size, coalesce_heap_max_size, flags);

		p_heap->mp_Next = p_vmem->mp_PhysicalCustomHeaps;
		p_vmem->mp_PhysicalCustomHeaps = p_heap;

		return p_heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void DestroyPhysicalHeap(HeapHandle heap)
	{
		VMemMain* p_vmem = GetVMem();

		CriticalSectionScope lock(p_vmem->m_CriticalSection);

		PhysicalCustomHeap* p_heap = (PhysicalCustomHeap*)heap;
		VMEM_ASSERT(p_heap->m_Marker == VMEM_PHYSICAL_CUSTOM_HEAP_MARKER, "Not a physical heap!");

		if (p_heap == p_vmem->mp_PhysicalCustomHeaps)
		{
			p_vmem->mp_PhysicalCustomHeaps = p_heap->mp_Next;
		}
		else
		{
			bool found = false;
			for (PhysicalCustomHeap* p_node = p_vmem->mp_PhysicalCustomHeaps; p_node; p_node = p_node->mp_Next)
			{
				if (p_node->mp_Next == p_heap)
				{
					found = true;
					p_node->mp_Next = p_heap->mp_Next;
					break;
				}
			}
			VMEM_ASSERT(found, "bad heap handle passed in to DestroyPhysicalHeap");
			VMEM_UNREFERENCED_PARAM(found);
		}

		p_heap->~PhysicalCustomHeap();

		Free(p_heap);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void* PhysicalHeapAlloc(HeapHandle heap, size_t size)
	{
		return GetPhysicalHeap(heap)->Alloc(size);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapFree(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->Free(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetSize(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->GetSize(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapOwns(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->Owns(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void* PhysicalHeapAllocAligned(HeapHandle heap, size_t size, size_t alignment)
	{
		return GetPhysicalHeap(heap)->AllocAligned(size, alignment);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	bool PhysicalHeapFreeAligned(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->FreeAligned(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetSizeAligned(HeapHandle heap, void* p)
	{
		return GetPhysicalHeap(heap)->GetSizeAligned(p);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapCheckIntegrity(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->CheckIntegrity();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapTrim(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->Trim();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	size_t PhysicalHeapGetMaxAllocSize(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->GetMaxAllocSize();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_PHYSICAL_HEAP_SUPPORTED
	void PhysicalHeapWriteStats(HeapHandle heap)
	{
		return GetPhysicalHeap(heap)->WriteStats();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_CUSTOM_ALLOC_INFO
	VMemCustomAllocInfo* GetCustomAllocInfo(void* p)
	{
		return (VMemCustomAllocInfo*)((VMem::byte*)p - sizeof(VMemCustomAllocInfo));
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemCore.cpp
//
#include <memory.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	void (*g_DebugBreakFn)(const _TCHAR* p_message) = NULL;

	void (*g_MemProDebugBreakFn)() = NULL;

	//------------------------------------------------------------------------
	void DebugWriteMem(size_t size)
	{
		float mb = (float)(size / 1024.0 / 1024.0);
#ifdef VMEM_X64
		DebugWrite(_T("%10lld (%0.1fMB)"), size, mb);
#else
		DebugWrite(_T("%10d (%0.1fMB)"), size, mb);
#endif

		if (mb < 10.0f)			DebugWrite(_T("   "));
		else if (mb < 100.0f)	DebugWrite(_T("  "));
		else if (mb < 1000.0f)	DebugWrite(_T(" "));
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats(const VMem::Stats& stats)
	{
		size_t committed_bytes = stats.GetCommittedBytes();
		int usage_percent = committed_bytes ? (int)((100 * (long long)stats.m_Used) / committed_bytes) : 0;
		DebugWrite(_T("%3d%% "), usage_percent);

		DebugWriteMem(stats.m_Used);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Unused);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Overhead);
		DebugWrite(_T("  "));
		DebugWriteMem(committed_bytes);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Reserved);
		DebugWrite(_T("\n"));
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats(const VMemHeapStats& stats)
	{
		DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		DebugWrite(_T("     FSA1:  "));	VMem::WriteStats(stats.m_FSAHeap1);
		DebugWrite(_T("     FSA2:  "));	VMem::WriteStats(stats.m_FSAHeap2);
		DebugWrite(_T("Coalesce1:  "));	VMem::WriteStats(stats.m_CoalesceHeap1);
		DebugWrite(_T("Coalesce2:  "));	VMem::WriteStats(stats.m_CoalesceHeap2);
		DebugWrite(_T("    Large:  "));	VMem::WriteStats(stats.m_LargeHeap);
		DebugWrite(_T("  Aligned:  "));	VMem::WriteStats(stats.m_AlignedCoalesceHeap + stats.m_AlignedLargeHeap);
		DebugWrite(_T(" Internal:  "));	VMem::WriteStats(stats.m_Internal);
		DebugWrite(_T("    TOTAL:  "));	VMem::WriteStats(stats.m_Total);
	}
#endif

	//------------------------------------------------------------------------
	#if VMEM_SIMULATE_OOM
		struct VMemCoreGlobals
		{
			VMemCoreGlobals()
			:	m_SimulateOOM(0),
				m_InternalHeapSimulateOOM(0)
			{
			}

			RelaxedAtomic32<int> m_SimulateOOM;
			RelaxedAtomic32<int> m_InternalHeapSimulateOOM;
			RelaxedAtomic32<int> m_PhysicalSimulateOOM;
		};
		byte gp_VMemCoreGlobalsMem[sizeof(VMemCoreGlobals)] VMEM_ALIGN_8;
		VMemCoreGlobals* gp_VMemCoreGlobals = NULL;

		void InitialiseOOMGlobals()
		{
			if(!gp_VMemCoreGlobals)
			{
				gp_VMemCoreGlobals = (VMemCoreGlobals*)gp_VMemCoreGlobalsMem;
				VMEM_MEMSET(gp_VMemCoreGlobals, VMEM_ALLOCATED_MEM, sizeof(gp_VMemCoreGlobalsMem));
				new (gp_VMemCoreGlobals) VMemCoreGlobals();
			}
		}

		void DestroyOOMGlobals()
		{
			gp_VMemCoreGlobals->~VMemCoreGlobals();
			gp_VMemCoreGlobals = NULL;
		}

		int GetSimulateOOMCount()						{ return gp_VMemCoreGlobals->m_SimulateOOM; }
		void SetSimulateOOMCount(int value)				{ gp_VMemCoreGlobals->m_SimulateOOM = value; }
		int GetInternalHeapSimulateOOMCount()			{ return gp_VMemCoreGlobals->m_InternalHeapSimulateOOM; }
		void SetInternalHeapSimulateOOMCount(int value)	{ gp_VMemCoreGlobals->m_InternalHeapSimulateOOM = value; }
		int GetPhysicalOOMCount()						{ return gp_VMemCoreGlobals->m_PhysicalSimulateOOM; }
		void SetPhysicalOOMCount(int value)				{ gp_VMemCoreGlobals->m_PhysicalSimulateOOM = value; }
	#endif

	//------------------------------------------------------------------------
	void InitialiseCore()
	{
		#if VMEM_SIMULATE_OOM
			InitialiseOOMGlobals();
		#endif

		VMemSysCreate();
	}

	//------------------------------------------------------------------------
	void UninitialiseCore()
	{
		VMemSysDestroy();

		#if VMEM_SIMULATE_OOM
			DestroyOOMGlobals();
		#endif
	}

	//------------------------------------------------------------------------
	// return true if break handled
	bool Break(const _TCHAR* p_message)
	{
		if(g_MemProDebugBreakFn)
			g_MemProDebugBreakFn();

		if(g_DebugBreakFn)
		{
			g_DebugBreakFn(p_message);
			return true;
		}
		else
		{
			DebugWrite(p_message);
			return false;
		}
	}

	//------------------------------------------------------------------------
	void SetDebugBreakFunction(void (*DebugBreakFn)(const _TCHAR* p_message))
	{
		g_DebugBreakFn = DebugBreakFn;
	}

	//------------------------------------------------------------------------
	void WriteAlloc(const void* p_alloc, size_t size)
	{
		DebugWrite(_T("%6d bytes at 0x%08x "), size, p_alloc);

		DebugWrite(_T("  <"));

		const size_t max_byte_printout = 8;
		size_t byte_count = VMin(max_byte_printout, size);

		// write hex memory
		const byte* p = (byte*)p_alloc;
		for(size_t i=0; i<byte_count; ++i)
		{
			byte c = *p++;
			if(c < 32 || c> 127)
				c = '.';
			DebugWrite(_T("%c"), (char)c);
		}
		DebugWrite(_T(">  "));

		// write chars
		p = (byte*)p_alloc;
		for(size_t i=0; i<byte_count; ++i)
			DebugWrite(_T("%02x "), *p++);
		if(byte_count < size)
			DebugWrite(_T("..."));

		DebugWrite(_T("\n"));
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemHeap.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_X64
	#define VMEM_COALESCE1_HEAP_REGION_SIZE (768*1024*1024)
	#define VMEM_COALESCE2_HEAP_REGION_SIZE (768*1024*1024)
#else
	#define VMEM_COALESCE1_HEAP_REGION_SIZE (128*1024*1024)
	#define VMEM_COALESCE2_HEAP_REGION_SIZE (256*1024*1024)
#endif

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	__declspec(thread) VMemHeapTLS* gp_ThreadFASAHeap = NULL;
#endif

	//------------------------------------------------------------------------
	namespace HeapSettings
	{
		enum Enum
		{
			// on windows we can reserve pages without committing them, this allows us
			// to have larger region sizes which improves performance
#if defined(VMEM_PLATFORM_XBOXONE) || defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_WIN)
			FSAHeap1_PageSize = 4 * 1024,
			FSAHeap1_RegionSize = 256 * 1024 * 1024,
			FSAHeap1_MaxSize = 64,

			FSAHeap2_PageSize = 64 * 1024,
			FSAHeap2_RegionSize = 256 * 1024 * 1024,
			FSAHeap2_MaxSize = 2048,

			CoalesceHeap1_RegionSize = VMEM_COALESCE1_HEAP_REGION_SIZE,
			CoalesceHeap1_MinSize = FSAHeap2_MaxSize,
			CoalesceHeap1_MaxSize = 128 * 1024,

			CoalesceHeap2_RegionSize = VMEM_COALESCE2_HEAP_REGION_SIZE,
			CoalesceHeap2_MinSize = CoalesceHeap1_MaxSize,
			CoalesceHeap2_MaxSize = 1024 * 1024,

			AlignedCoalesceHeap_RegionSize = 16 * 1024 * 1024,
			AlignedCoalesceHeap_MinSize = 32,
			AlignedCoalesceHeap_MaxSize = 2*1024,
#else
			FSAHeap1_PageSize = 4 * 1024,
			FSAHeap1_RegionSize = 1024 * 1024,
			FSAHeap1_MaxSize = 64,

			FSAHeap2_PageSize = 32 * 1024,
			FSAHeap2_RegionSize = 4 * 1024 * 1024,
			FSAHeap2_MaxSize = 512,

			CoalesceHeap1_RegionSize = 4 * 1024 * 1024,
			CoalesceHeap1_MinSize = 512,
			CoalesceHeap1_MaxSize = 10 * 1024,

			CoalesceHeap2_RegionSize = 8 * 1024 * 1024,
			CoalesceHeap2_MinSize = CoalesceHeap1_MaxSize,
			CoalesceHeap2_MaxSize = 1024 * 1024,

			AlignedCoalesceHeap_RegionSize = 16 * 1024 * 1024,
			AlignedCoalesceHeap_MinSize = 32,
			AlignedCoalesceHeap_MaxSize = 2*1024,
#endif
		};
	}

	//------------------------------------------------------------------------
	VMemHeap::VMemHeap(int page_size, int reserve_flags, int commit_flags)
	:	m_VirtualMem(page_size),
		m_PageSize(page_size),
		mp_FSA1PageHeapRegion(NULL),
		mp_FSA2PageHeapRegion(NULL),
		mp_CoalesceHeap1Region(NULL),
		mp_CoalesceHeap2Region(NULL),
		m_InternalHeap(m_VirtualMem),
		m_HeapRegions(m_InternalHeap),
		m_FSA1PageHeap(ToInt(HeapSettings::FSAHeap1_PageSize), ToInt(HeapSettings::FSAHeap1_RegionSize), m_InternalHeap, m_HeapRegions, RegionType::PageHeap1, reserve_flags, commit_flags, m_VirtualMem),
		m_FSA2PageHeap(ToInt(HeapSettings::FSAHeap2_PageSize), ToInt(HeapSettings::FSAHeap2_RegionSize), m_InternalHeap, m_HeapRegions, RegionType::PageHeap2, reserve_flags, commit_flags, m_VirtualMem),
#ifdef VMEM_FSA_HEAP_PER_THREAD
		mp_VMemHeapTLSList(NULL),
#else
		m_FSAHeap1(&m_FSA1PageHeap, m_InternalHeap, page_size),
#endif
		m_FSAHeap2(&m_FSA2PageHeap, m_InternalHeap, page_size),
		m_CoalesceHeap1(ToInt(HeapSettings::CoalesceHeap1_RegionSize), ToInt(HeapSettings::CoalesceHeap1_MinSize), ToInt(HeapSettings::CoalesceHeap1_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::CoalesceHeap1, reserve_flags, commit_flags, m_VirtualMem, VMEM_COALESCE_HEAP1_CACHE, false),
		m_CoalesceHeap2(ToInt(HeapSettings::CoalesceHeap2_RegionSize), ToInt(HeapSettings::CoalesceHeap2_MinSize), ToInt(HeapSettings::CoalesceHeap2_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::CoalesceHeap2, reserve_flags, commit_flags, m_VirtualMem, VMEM_COALESCE_HEAP2_CACHE, false),
		m_LargeHeap(reserve_flags, commit_flags, m_VirtualMem),
		m_AlignedCoalesceHeap(ToInt(HeapSettings::AlignedCoalesceHeap_RegionSize), ToInt(HeapSettings::AlignedCoalesceHeap_MinSize), ToInt(HeapSettings::AlignedCoalesceHeap_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::AlignedCoalesceHeap, m_VirtualMem, reserve_flags, commit_flags),
		m_AlignedLargeHeap(reserve_flags, commit_flags, m_VirtualMem)
#ifdef VMEM_PROTECTED_HEAP
		,m_ProtectedHeap(page_size, reserve_flags, commit_flags)
#endif
	{
	}

	//------------------------------------------------------------------------
	bool VMemHeap::Initialise()
	{
		if(!m_HeapRegions.Initialise())
			return false;

		if(!m_FSA1PageHeap.Initialise())
			return false;

		if(!m_FSA2PageHeap.Initialise())
			return false;

		if(!m_FSAHeap1.Initialise())
			return false;

		if(!m_FSAHeap2.Initialise())
			return false;

		if(!m_CoalesceHeap1.Initialise())
			return false;

		if(!m_CoalesceHeap2.Initialise())
			return false;

		if(!m_LargeHeap.Initialise())
			return false;

		if(!m_AlignedCoalesceHeap.Initialise())
			return false;

		if(!m_AlignedLargeHeap.Initialise())
			return false;

#ifndef VMEM_FSA_HEAP_PER_THREAD
		if(!InitialiseFSAHeap1(m_FSAHeap1))
			return false;
#endif
		if(!InitialiseFSAHeap2(m_FSAHeap2))
			return false;

		InitialiseTrailGuards();

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		CreateCoalesceHeapForThisThread();
#endif

		mp_FSA1PageHeapRegion = m_FSA1PageHeap.GetFirstRegion();
		mp_FSA2PageHeapRegion = m_FSA2PageHeap.GetFirstRegion();
		mp_CoalesceHeap1Region = m_CoalesceHeap1.GetFirstRegion();
		mp_CoalesceHeap2Region = m_CoalesceHeap2.GetFirstRegion();

		return true;
	}

	//------------------------------------------------------------------------
	VMemHeap::~VMemHeap()
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		VMemHeapTLS* p_iter = mp_VMemHeapTLSList;
		while(p_iter)
		{
			VMemHeapTLS* p_next = p_iter->mp_Next;

			CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap;
			if(p_coalesce_heap)
				m_InternalHeap.Delete_WithFalseSharingBuffer(p_coalesce_heap);

			m_InternalHeap.Delete_WithFalseSharingBuffer(p_iter);

			p_iter = p_next;
		}
		gp_ThreadFASAHeap = NULL;
#endif
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	VMemHeapTLS* VMemHeap::CreateVMemHeapTLS()
	{
		VMemHeapTLS* p_fsa_heap = m_InternalHeap.New_WithFalseSharingBuffer<VMemHeapTLS>(&m_FSA1PageHeap, m_InternalHeap);

		InitialiseFSAHeap1(p_fsa_heap->m_FSAHeap1);

		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			p_fsa_heap->mp_Next = mp_VMemHeapTLSList;
			mp_VMemHeapTLSList = p_fsa_heap;
		}

		gp_ThreadFASAHeap = p_fsa_heap;

		return p_fsa_heap;
	}
#endif

	//------------------------------------------------------------------------
	bool VMemHeap::InitialiseFSAHeap1(FSAHeap& fsa_heap)
	{
#ifdef VMEM_X64
		const int alignment = VMEM_NATURAL_ALIGNMENT;
		const int step = VMEM_NATURAL_ALIGNMENT;

		for(int i=step; i<=HeapSettings::FSAHeap1_MaxSize; i+=step)
		{
			if(!fsa_heap.InitialiseFSA(i, alignment))
				return false;
		}
#else
		if(!fsa_heap.InitialiseFSA(4, 4)) return false;
		if(!fsa_heap.InitialiseFSA(8, 8)) return false;
		if(!fsa_heap.InitialiseFSA(12, 12)) return false;
		if(!fsa_heap.InitialiseFSA(16, 16)) return false;
		if(!fsa_heap.InitialiseFSA(20, 20)) return false;
		if(!fsa_heap.InitialiseFSA(24, 24)) return false;
		if(!fsa_heap.InitialiseFSA(28, 28)) return false;
		if(!fsa_heap.InitialiseFSA(32, 16)) return false;
		if(!fsa_heap.InitialiseFSA(36, 36)) return false;
		if(!fsa_heap.InitialiseFSA(40, 40)) return false;
		if(!fsa_heap.InitialiseFSA(44, 44)) return false;
		if(!fsa_heap.InitialiseFSA(48, 16)) return false;
		for(int i=64; i<=HeapSettings::FSAHeap1_MaxSize; i+=16)
		{
			if(!fsa_heap.InitialiseFSA(i, 16))
				return false;
		}
#endif
		return true;
	}

	//------------------------------------------------------------------------
	bool VMemHeap::InitialiseFSAHeap2(FSAHeap& fsa_heap)
	{
		const int alignment = VMEM_NATURAL_ALIGNMENT;
		const int step = VMax(16, VMEM_NATURAL_ALIGNMENT);

		size_t last_size = HeapSettings::FSAHeap1_MaxSize;

		for(size_t i=HeapSettings::FSAHeap1_MaxSize+step; i<=HeapSettings::FSAHeap2_MaxSize; i+=step)
		{
			if(i - last_size > (i * VMEM_FSA_WASTAGE) / 100 || i == HeapSettings::FSAHeap2_MaxSize)
			{
				if(!fsa_heap.InitialiseFSA(ToInt(i), alignment))
					return false;

				last_size = i;
			}
		}

		return true;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_FSA_HEAP_PER_THREAD
	VMEM_FORCE_INLINE VMemHeapTLS* VMemHeap::GetVMemHeapTLS()
	{
		VMemHeapTLS* p_fsa_heap = gp_ThreadFASAHeap;
		if(!p_fsa_heap)
			p_fsa_heap = CreateVMemHeapTLS();

		return p_fsa_heap;
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD
	void VMemHeap::Update()
	{
		m_VirtualMem.Update();

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.Update();
		}
#else
		#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
			m_FSAHeap1.Update();
		#endif
#endif
		#if defined(VMEM_FSA_PAGE_CACHE) || defined(VMEM_POPULATE_FSA_CACHE)
			m_FSAHeap2.Update();
		#endif

		m_CoalesceHeap1.Update();
		m_CoalesceHeap2.Update();

		m_AlignedCoalesceHeap.Update();
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
	void VMemHeap::CreateCoalesceHeapForThisThread()
	{
		VMEM_ASSERT(GetVMemHeapTLS()->mp_CoalesceHeap.load() == NULL, "This thread already has its own CoalesceHeap");
		CoalesceHeap* p_coalesce_heap = m_InternalHeap.New_WithFalseSharingBuffer<CoalesceHeap>(ToInt(HeapSettings::CoalesceHeap1_RegionSize), ToInt(HeapSettings::CoalesceHeap1_MinSize), ToInt(HeapSettings::CoalesceHeap1_MaxSize), m_InternalHeap, m_HeapRegions, RegionType::TLSCoalesceHeap);
		GetVMemHeapTLS()->mp_CoalesceHeap.store(p_coalesce_heap, std::memory_order_release);
	}
#endif

	//------------------------------------------------------------------------
	void* VMemHeap::Alloc(size_t size, size_t alignment)
	{
		VMEM_GLOBAL_LOCK

		VMEM_ASSERT(size >= 0, "can't allocate negative size");

#ifdef VMEM_INC_INTEG_CHECK
		IncIntegrityCheck();
#endif

		if(alignment != VMEM_NATURAL_ALIGNMENT)
			return AlignedAlloc(size, alignment);

		void* p = NULL;

#ifdef VMEM_PROTECTED_HEAP
		p = m_ProtectedHeap.Alloc(size);
		if(p)
			return p;
#endif

#ifdef VMEM_FSA_HEAP_PER_THREAD
		if(size <= HeapSettings::FSAHeap1_MaxSize)
			p = GetVMemHeapTLS()->m_FSAHeap1.Alloc(size);
#else
		if(size <= HeapSettings::FSAHeap1_MaxSize)
			p = m_FSAHeap1.Alloc(size);
#endif
		else if(size <= HeapSettings::FSAHeap2_MaxSize)
			p = m_FSAHeap2.Alloc(size);

		else if(size <= HeapSettings::CoalesceHeap1_MaxSize)
		{
#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			VMemHeapTLS* p_heap_tls = GetVMemHeapTLS();
			CoalesceHeap* p_coalesce_heap = p_heap_tls->mp_CoalesceHeap;
			if(p_coalesce_heap)
				p = p_coalesce_heap->Alloc(size);
			else
				p = m_CoalesceHeap1.Alloc(size);
#else
			p = m_CoalesceHeap1.Alloc(size);
#endif
		}

		else if(size <= HeapSettings::CoalesceHeap2_MaxSize)
			p = m_CoalesceHeap2.Alloc(size);

		else
			p = m_LargeHeap.Alloc(size);

		return p;
	}

	//------------------------------------------------------------------------
	void* VMemHeap::AlignedAlloc(size_t size, size_t alignment)
	{
		VMEM_ASSERT(alignment, "zero alignment not supported");

		void* p = NULL;

		size_t aligned_size = size + sizeof(AlignedHeader) + alignment;

#ifdef VMEM_CUSTOM_ALLOC_INFO
		bool can_use_non_aligned_heap = false;
#else
		bool can_use_non_aligned_heap = true;
#endif
		if(can_use_non_aligned_heap && alignment == (size_t)m_PageSize)
		{
			p = m_LargeHeap.Alloc(size ? size : m_PageSize);
		}
		else if(aligned_size <= HeapSettings::AlignedCoalesceHeap_MaxSize)
		{
			p = m_AlignedCoalesceHeap.Alloc(size, alignment);
		}
		else
		{
			p = m_AlignedLargeHeap.Alloc(size, alignment);
		}

#ifdef VMEM_CUSTOM_ALLOC_INFO
		VMEM_ASSERT(!p || (((uint64)p + sizeof(VMemCustomAllocInfo)) % (uint64)alignment) == 0, "alignment failed");
#else
		VMEM_ASSERT(!p || (((uint64)p) % (uint64)alignment) == 0, "alignment failed");
#endif

		return p;
	}

	//------------------------------------------------------------------------
	void VMemHeap::Free(void* p)
	{
		VMEM_GLOBAL_LOCK

#ifdef VMEM_PROTECTED_HEAP
			if(m_ProtectedHeap.Free(p))
				return;
#endif
		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
		{
			FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->Free(p, HeapSettings::FSAHeap1_PageSize);
			return;
		}

		if(p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
		{
			FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->Free(p, HeapSettings::FSAHeap2_PageSize);
			return;
		}

		if(p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize)
		{
			m_CoalesceHeap1.Free(p);
			return;
		}

		if(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize)
		{
			m_CoalesceHeap2.Free(p);
			return;
		}

		if(!p)
			return;

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		switch(region_type)
		{
			case RegionType::PageHeap1:
				FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->Free(p, HeapSettings::FSAHeap1_PageSize);
				return;

			case RegionType::PageHeap2:
				FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->Free(p, HeapSettings::FSAHeap2_PageSize);
				return;

			case RegionType::CoalesceHeap1:
				m_CoalesceHeap1.Free(p);
				return;

			case RegionType::CoalesceHeap2:
				m_CoalesceHeap2.Free(p);
				return;

			case RegionType::AlignedCoalesceHeap:
				m_AlignedCoalesceHeap.Free(p);
				return;

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			case RegionType::TLSCoalesceHeap:
				((CoalesceHeap*)region.mp_Heap)->Free(p);
				return;
#endif
			case RegionType::Invalid:
			{
				if(m_LargeHeap.Free(p))
					return;

				if(m_AlignedLargeHeap.Free(p))
					return;
			} break;
		}

		VMEM_BREAK("Trying to free allocation that is not owned by VMem");
	}

	//------------------------------------------------------------------------
	size_t VMemHeap::GetSize(void* p)
	{
		VMEM_GLOBAL_LOCK

		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
		{
			return FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->GetSize();
		}

		if (p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
		{
			return FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->GetSize();
		}

		if ((p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize) ||
			(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize))
		{
			return CoalesceHeap::GetSize(p);
		}

#ifdef VMEM_PROTECTED_HEAP
		size_t size = m_ProtectedHeap.GetSize(p);
		if(size != VMEM_INVALID_SIZE) return size;
#endif

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		switch(region_type)
		{
			case RegionType::PageHeap1:
				return FSA::GetFSA(p, HeapSettings::FSAHeap1_PageSize)->GetSize();

			case RegionType::PageHeap2:
				return FSA::GetFSA(p, HeapSettings::FSAHeap2_PageSize)->GetSize();

			case RegionType::CoalesceHeap1:
				return CoalesceHeap::GetSize(p);

			case RegionType::CoalesceHeap2:
				return CoalesceHeap::GetSize(p);

			case RegionType::AlignedCoalesceHeap:
				return AlignedCoalesceHeap::GetSize(p);

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
			case RegionType::TLSCoalesceHeap:
				return CoalesceHeap::GetSize(p);
#endif
			case RegionType::Invalid:
			{
				size_t large_size = m_LargeHeap.GetSize(p);
				if(large_size != VMEM_INVALID_SIZE) return large_size;

				size_t large_aligned_size = m_AlignedLargeHeap.GetSize(p);
				if(large_aligned_size != VMEM_INVALID_SIZE) return large_aligned_size;
			} break;
		}

		return VMEM_INVALID_SIZE;
	}

	//------------------------------------------------------------------------
	bool VMemHeap::Owns(void* p)
	{
		VMEM_GLOBAL_LOCK

		if(p >= mp_FSA1PageHeapRegion && p < (byte*)mp_FSA1PageHeapRegion + HeapSettings::FSAHeap1_RegionSize)
			return true;

		if (p >= mp_FSA2PageHeapRegion && p < (byte*)mp_FSA2PageHeapRegion + HeapSettings::FSAHeap2_RegionSize)
			return true;

		if ((p >= mp_CoalesceHeap1Region && p < (byte*)mp_CoalesceHeap1Region + HeapSettings::CoalesceHeap1_RegionSize) ||
			(p >= mp_CoalesceHeap2Region && p < (byte*)mp_CoalesceHeap2Region + HeapSettings::CoalesceHeap2_RegionSize))
		{
			return true;
		}

#ifdef VMEM_PROTECTED_HEAP
		if(m_ProtectedHeap.GetSize(p) != VMEM_INVALID_SIZE)
			return true;
#endif

#ifdef VMEM_COALESCE_HEAP_PER_THREAD
		HeapRegions::RegionAndHeap region = m_HeapRegions.GetRegionAndHeap(p);
		RegionType::Enum region_type = region.m_RegionType;
#else
		RegionType::Enum region_type = m_HeapRegions.GetRegion(p);
#endif
		if (region_type != RegionType::Invalid)
			return true;

		if(m_LargeHeap.Owns(p))
			return true;

		if(m_AlignedLargeHeap.Owns(p))
			return true;

		return false;
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void VMemHeap::SendStatsToMemPro(MemProSendFn send_fn, void* p_context)
	{
		VMEM_GLOBAL_LOCK
		m_InternalHeap.SendStatsToMemPro(send_fn, p_context);
		m_FSA1PageHeap.SendStatsToMemPro(send_fn, p_context);
		m_FSA2PageHeap.SendStatsToMemPro(send_fn, p_context);
#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.SendStatsToMemPro(send_fn, p_context);
		}
#else
		m_FSAHeap1.SendStatsToMemPro(send_fn, p_context);
#endif
		m_FSAHeap2.SendStatsToMemPro(send_fn, p_context);

		m_CoalesceHeap1.SendStatsToMemPro(send_fn, p_context);
		m_CoalesceHeap2.SendStatsToMemPro(send_fn, p_context);

		m_LargeHeap.SendStatsToMemPro(send_fn, p_context);
		m_AlignedCoalesceHeap.SendStatsToMemPro(send_fn, p_context);
		m_AlignedLargeHeap.SendStatsToMemPro(send_fn, p_context);

		SendEnumToMemPro(vmem_End, send_fn, p_context);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	VMemHeapStats VMemHeap::GetStatsNoLock() const
	{
		VMemHeapStats stats;

		Stats page_heap1 = m_FSA1PageHeap.GetStatsNoLock();
		page_heap1.m_Used = 0;												// this is tracked by the FSAHeap
#ifdef VMEM_FSA_HEAP_PER_THREAD
		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			stats.m_FSAHeap1 += p_iter->m_FSAHeap1.GetStatsNoLock();
			stats.m_Internal.m_Overhead += sizeof(VMemHeapTLS) + InternalHeap::GetFalseSharingBufferOverhead();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
				{
					stats.m_CoalesceHeap1 += p_coalesce_heap->GetStatsNoLock();
					stats.m_Internal.m_Overhead += sizeof(CoalesceHeap) + InternalHeap::GetFalseSharingBufferOverhead();
				}
			#endif
		}
		stats.m_FSAHeap1 += page_heap1;
#else
		stats.m_FSAHeap1 = m_FSAHeap1.GetStatsNoLock() + page_heap1;
#endif

		Stats page_heap2 = m_FSA2PageHeap.GetStatsNoLock();
		page_heap2.m_Used = 0;												// this is tracked by the FSAHeap
		stats.m_FSAHeap2 = m_FSAHeap2.GetStatsNoLock() + page_heap2;

		stats.m_CoalesceHeap1 += m_CoalesceHeap1.GetStatsNoLock();
		stats.m_CoalesceHeap2 = m_CoalesceHeap2.GetStatsNoLock();

		stats.m_LargeHeap = m_LargeHeap.GetStatsNoLock();

		stats.m_AlignedCoalesceHeap = m_AlignedCoalesceHeap.GetStatsNoLock();
		stats.m_AlignedLargeHeap = m_AlignedLargeHeap.GetStatsNoLock();

		stats.m_Internal += m_InternalHeap.GetStatsNoLock();
		stats.m_Internal.m_Used = 0;										// this is tracked by the allocator overheads

		stats.m_Internal.m_Overhead += m_HeapRegions.GetMemoryUsage_NoLock();

		stats.m_Total =
			stats.m_FSAHeap1 +
			stats.m_FSAHeap2 +
			stats.m_CoalesceHeap1 +
			stats.m_CoalesceHeap2 +
			stats.m_LargeHeap +
			stats.m_AlignedCoalesceHeap +
			stats.m_AlignedLargeHeap +
			stats.m_Internal;

#ifdef VMEM_PROTECTED_HEAP
		stats.m_Total += m_ProtectedHeap.GetStatsNoLock();
#endif

		Stats virtual_mem_stats = m_VirtualMem.GetStatsNoLock();
		stats.m_Internal += virtual_mem_stats;
		stats.m_Total += virtual_mem_stats;

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void VMemHeap::Flush()
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
			{
				p_iter->m_FSAHeap1.Flush();

				#ifdef VMEM_COALESCE_HEAP_PER_THREAD
					CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
					if(p_coalesce_heap)
						p_coalesce_heap->Flush();
				#endif
			}
		}
#else
		m_FSAHeap1.Flush();
#endif
		m_FSAHeap2.Flush();

		m_CoalesceHeap1.Flush();
		m_CoalesceHeap2.Flush();

		m_VirtualMem.Flush();
	}

	//------------------------------------------------------------------------
	void VMemHeap::CheckIntegrity()
	{
		VMEM_GLOBAL_LOCK

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				p_iter->m_FSAHeap1.CheckIntegrity();
		}
#else
		m_FSAHeap1.CheckIntegrity();
#endif
		m_FSAHeap2.CheckIntegrity();

		m_CoalesceHeap1.CheckIntegrity();
		m_CoalesceHeap2.CheckIntegrity();

#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.CheckIntegrity();
#endif

		m_VirtualMem.CheckIntegrity();
	}

	//------------------------------------------------------------------------
#ifdef VMEM_INC_INTEG_CHECK
	void VMemHeap::IncIntegrityCheck()
	{
		static volatile long i = 0;
		if(i == VMEM_INC_INTEG_CHECK)
		{
#ifdef VMEM_FSA_HEAP_PER_THREAD
			{
				CriticalSectionScope lock(m_VMemHeapTLSListLock);
				for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
					p_iter->m_FSAHeap1.IncIntegrityCheck();
			}
#else
			m_FSAHeap1.IncIntegrityCheck();
#endif
		}
		else if(i == 2*VMEM_INC_INTEG_CHECK)
		{
			m_FSAHeap2.IncIntegrityCheck();
		}
		else if(i == 3*VMEM_INC_INTEG_CHECK)
		{
			m_CoalesceHeap1.CheckIntegrity();
		}
		else if(i == 4*VMEM_INC_INTEG_CHECK)
		{
			m_CoalesceHeap2.CheckIntegrity();
		}
		else if(i == 6*VMEM_INC_INTEG_CHECK)
		{
			i = -1;
		}

		VMem_InterlockedIncrement(i);
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	size_t VMemHeap::WriteAllocs()
	{
		VMEM_GLOBAL_LOCK

		size_t allocated_bytes = 0;

#ifdef VMEM_FSA_HEAP_PER_THREAD
		{
			CriticalSectionScope lock(m_VMemHeapTLSListLock);
			for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
				allocated_bytes += p_iter->m_FSAHeap1.WriteAllocs();
		}
#else
		allocated_bytes += m_FSAHeap1.WriteAllocs();
#endif
		allocated_bytes += m_FSAHeap2.WriteAllocs();

		allocated_bytes += m_CoalesceHeap1.WriteAllocs();
		allocated_bytes += m_CoalesceHeap2.WriteAllocs();

		allocated_bytes += m_LargeHeap.WriteAllocs();

		allocated_bytes += m_AlignedCoalesceHeap.WriteAllocs();
		allocated_bytes += m_AlignedCoalesceHeap.WriteAllocs();

		return allocated_bytes;
	}
#endif

	//------------------------------------------------------------------------
	void VMemHeap::InitialiseTrailGuards()
	{
#ifdef VMEM_TRAIL_GUARDS
		int start_size = sizeof(void*);
		for(int i=start_size; i<=ToInt(HeapSettings::FSAHeap1_MaxSize); i+=4)
		{
#ifdef VMEM_FSA_HEAP_PER_THREAD
			{
				CriticalSectionScope lock(m_VMemHeapTLSListLock);
				for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
					p_iter->m_FSAHeap1.GetFSA(i)->InitialiseTrailGuard(100*1024, 1000);
			}
#else
			FSA* p_fsa = m_FSAHeap1.GetFSA(i);
			p_fsa->InitialiseTrailGuard(100*1024, 1000);
#endif
		}

		for(int i=ToInt(HeapSettings::FSAHeap1_MaxSize)+4; i<=ToInt(HeapSettings::FSAHeap2_MaxSize); i+=4)
		{
			FSA* p_fsa = m_FSAHeap2.GetFSA(i);
			p_fsa->InitialiseTrailGuard(100*1024, 1000);
		}

		m_CoalesceHeap1.InitialiseTrailGuard(1024*1024, 100);
		m_CoalesceHeap2.InitialiseTrailGuard(1024*1024, 100);
#endif
	}

	//------------------------------------------------------------------------
	void VMemHeap::AcquireLockShared() const
	{
#ifdef VMEM_FSA_HEAP_PER_THREAD
		m_VMemHeapTLSListLock.Enter();

		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			p_iter->m_FSAHeap1.Lock();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
					p_coalesce_heap->Lock();
			#endif
		}
#else
		m_FSAHeap1.Lock();
#endif
		m_FSAHeap2.Lock();

		m_CoalesceHeap1.Lock();
		m_CoalesceHeap2.Lock();

		m_LargeHeap.AcquireLockShared();
		m_AlignedCoalesceHeap.Lock();
		m_AlignedLargeHeap.AcquireLockShared();
#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.Lock();
#endif
		m_VirtualMem.Lock();
	}

	//------------------------------------------------------------------------
	void VMemHeap::ReleaseLockShared() const
	{
		m_VirtualMem.Release();

		m_AlignedLargeHeap.ReleaseLockShared();
		m_AlignedCoalesceHeap.Release();
		m_LargeHeap.ReleaseLockShared();

		m_CoalesceHeap2.Release();
		m_CoalesceHeap1.Release();

		m_FSAHeap2.Release();
#ifdef VMEM_FSA_HEAP_PER_THREAD
		for(VMemHeapTLS* p_iter=mp_VMemHeapTLSList; p_iter; p_iter=p_iter->mp_Next)
		{
			p_iter->m_FSAHeap1.Release();

			#ifdef VMEM_COALESCE_HEAP_PER_THREAD
				CoalesceHeap* p_coalesce_heap = p_iter->mp_CoalesceHeap.load();
				if(p_coalesce_heap)
					p_coalesce_heap->Release();
			#endif
		}
#else
		m_FSAHeap1.Release();
#endif

#ifdef VMEM_PROTECTED_HEAP
		m_ProtectedHeap.Release();
#endif

	#ifdef VMEM_FSA_HEAP_PER_THREAD
		m_VMemHeapTLSListLock.Leave();
	#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemSys.cpp
//
#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#if defined(VMEM_PLATFORM_WIN) || defined(VMEM_PLATFORM_LINUX)
	#include <malloc.h>
#endif

#if defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE)
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

#if defined(VMEM_PLATFORM_ANDROID)
	#include <malloc.h>
#endif

#if defined(VMEM_PLATFORM_LINUX) || defined(VMEM_PLATFORM_APPLE) || defined(VMEM_PLATFORM_ANDROID)
	#include <sys/mman.h>
#endif

#ifdef VMEM_PLATFORM_XBOXONE
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

#ifdef VMEM_PLATFORM_SWITCH
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

//-----------------------------------------------------------------------------
#ifdef VMEM_OS_WIN
	#pragma warning(disable:4100)
#endif

//-----------------------------------------------------------------------------
// Using malloc as the base allocator is not recommended. It is only provided here
// as a quick way to get VMem working on any platform. Malloc does not support
// committing and decommitting of memory which VMem needs to perform optimally.
// If you do use malloc please ensure you set the region sizes carefully in VMemHeap
// in order to reduce the memory wastage.
//#define VMEM_USE_MALLOC

//-----------------------------------------------------------------------------
#ifdef VMEM_SIMULATE_PS4
	#ifdef off_t
		#undef off_t
	#endif
	namespace PS4Simulator { typedef long long off_t; }
	#define off_t PS4Simulator::off_t
	int sceKernelReserveVirtualRange(void**, size_t, int, int);
	int sceKernelMunmap(void*, size_t);
	int sceKernelMapDirectMemory(void**, size_t, int, int, off_t, size_t);
	int sceKernelAllocateMainDirectMemory(size_t, size_t, int, off_t*);
	int sceKernelCheckedReleaseDirectMemory(off_t, size_t);
	struct SceKernelVirtualQueryInfo { void *start; void *end; off_t offset; };
	int sceKernelVirtualQuery(const void *, int, SceKernelVirtualQueryInfo*, size_t);
	#define SCE_OK						0
	#define SCE_KERNEL_WB_ONION			0
	#define SCE_KERNEL_WC_GARLIC		1
	#define SCE_KERNEL_MAP_FIXED		(1<<1)
	#define SCE_KERNEL_MAP_NO_COALESCE	(1<<2)
	#define SCE_KERNEL_MAP_NO_OVERWRITE	(1<<3)
	#define SCE_KERNEL_PROT_CPU_RW		(1<<4)
	#define SCE_KERNEL_PROT_GPU_RW		(1<<5)
#endif

//-----------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	size_t g_ReservedBytes = 0;
	size_t g_CommittedBytes = 0;

	void (*g_LogFn)(const _TCHAR* p_message) = NULL;

	//------------------------------------------------------------------------
	struct VMemSysData
	{
		CriticalSection m_CriticalSection;

#ifdef VMEM_USE_PAGE_MAPPER
		PhysicalPageMapper m_PhysicalPageMapper;
#endif
	};
	byte gp_VMemSysDataMem[sizeof(VMemSysData)] VMEM_ALIGN_8;
	VMemSysData* gp_VMemSysData = NULL;

	//------------------------------------------------------------------------
	void Log(const _TCHAR* p_message)
	{
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

		if(g_LogFn)
		{
			g_LogFn(p_message);
		}
		else
		{
			#if defined(VMEM_OS_WIN)
				OutputDebugString(p_message);
			#else
				printf("%s", p_message);
			#endif
		}
	}

	//------------------------------------------------------------------------
	void PrintLastError()
	{
#ifdef VMEM_PLATFORM_WIN
		_TCHAR* p_buffer = NULL;
		va_list args = NULL;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			0,
			(_TCHAR*)&p_buffer,
			4 * 1024,
			&args);
		Log(p_buffer);
#endif
	}

	//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

	//------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#endif

	//------------------------------------------------------------------------
	void VMemSysCreate()
	{
		new(gp_VMemSysDataMem)VMemSysData();
		gp_VMemSysData = (VMemSysData*)gp_VMemSysDataMem;
	}

	//------------------------------------------------------------------------
	void VMemSysDestroy()
	{
		gp_VMemSysData->~VMemSysData();
		gp_VMemSysData = NULL;

		g_ReservedBytes = 0;
		g_CommittedBytes = 0;
	}

	//------------------------------------------------------------------------
	void* VirtualReserve(size_t size, int page_size, int reserve_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");
		VMEM_ASSERT((size & (page_size - 1)) == 0, "reserve size not aligned to page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC reserve
		// ====================================================
		VMEM_UNREFERENCED_PARAM(reserve_flags);
		void* p = _aligned_malloc(size, page_size);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					WIN reserve
		// ====================================================
		unsigned int flags = MEM_RESERVE | reserve_flags;

		// only zero memory if memset defined
		#if !defined(VMEM_ENABLE_MEMSET) && defined(VMEM_PLATFORM_XBOX360)
			#error Please contact slynch@puredevsoftware.com for this platform
		#endif

		// large pages more effecient on this platform
		#ifdef VMEM_PLATFORM_XBOX360
			#error Please contact slynch@puredevsoftware.com for this platform
		#endif

		#ifdef VMEM_PLATFORM_XBOXONE
			#error Please contact slynch@puredevsoftware.com for this platform
		#endif

		void* p = ::VirtualAlloc(NULL, size, flags, PAGE_NOACCESS);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_SWITCH)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
		// ====================================================
		//					LINUX reserve
		// ====================================================
		void* p = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON | reserve_flags, -1, 0);
		if(p == MAP_FAILED)
			p = NULL;

		msync(p, size, (MS_SYNC | MS_INVALIDATE));

#endif

		if(p)
			g_ReservedBytes += size;

		#if !(defined(VMEM_SIMULATE_PS4) && defined(VMEM_OS_LINUX))
			VMEM_ASSERT(((size_t)p & (page_size-1)) == 0, "Memory not aligned!");
		#endif

		return p;
	}

	//------------------------------------------------------------------------
	void VirtualRelease(void* p, size_t size)
	{
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC release
		// ====================================================
		_aligned_free(p);

#elif defined(VMEM_OS_WIN) && !defined(VMEM_SIMULATE_PS4)

		// ====================================================
		//					WIN release
		// ====================================================
		BOOL b = ::VirtualFree(p, 0, MEM_RELEASE);
		if(!b)
			PrintLastError();
		VMEM_ASSERT(b, "VirtualFree failed");

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_SWITCH)
	#error Please contact slynch@puredevsoftware.com for this platform
#else

		// ====================================================
		//					LINUX release
		// ====================================================
		msync(p, size, MS_SYNC);
		#ifdef VMEM_ASSERTS
			int ret = munmap(p, size);
			VMEM_ASSERT(ret == 0, "munmap failed");
		#else
			munmap(p, size);
		#endif

#endif

		g_ReservedBytes -= size;
	}

	//------------------------------------------------------------------------
	bool VirtualCommit(void* p, size_t size, int page_size, int commit_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if VMEM_SIMULATE_OOM
		int simluate_oom_count = GetSimulateOOMCount();
		if(simluate_oom_count > 0)
		{
			if(simluate_oom_count == 1)
  				return false;
			else
				SetSimulateOOMCount(simluate_oom_count - 1);
		}
#endif
		bool success = false;

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC commit
		// ====================================================
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_UNREFERENCED_PARAM(commit_flags);
		success = true;

#elif defined(VMEM_USE_PAGE_MAPPER)

		// ====================================================
		//			   PageMapper commit (PS4)
		// ====================================================

		success = gp_VMemSysData->m_PhysicalPageMapper.MapPhysicalPages(p, size, commit_flags);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_SWITCH)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_WIN)

		// ====================================================
		//					WIN commit
		// ====================================================

		unsigned int va_flags = MEM_COMMIT | commit_flags;

		// only zero memory if memset defined
		#if !defined(VMEM_ENABLE_MEMSET) && defined(VMEM_PLATFORM_XBOX360)
			#error Please contact slynch@puredevsoftware.com for this platform
		#endif

		// large pages more effecient on this platform
		#if defined(VMEM_PLATFORM_XBOX360) || defined(VMEM_PLATFORM_XBOXONE)
			#error Please contact slynch@puredevsoftware.com for this platform
		#endif

		success = ::VirtualAlloc(p, size, va_flags, PAGE_READWRITE) != NULL;

#else

		// ====================================================
		//					LINUX commit
		// ====================================================
		
		mmap(p, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON | commit_flags, -1, 0);
		msync(p, size, MS_SYNC | MS_INVALIDATE);
		success = true;

#endif

		if(success)
		{
			g_CommittedBytes += size;
			VMEM_MEMSET(p, VMEM_COMMITTED_MEM, size);
		}

		return success;
	}

	//------------------------------------------------------------------------
	void VirtualDecommit(void* p, size_t size, int page_size, int commit_flags)
	{
		VMEM_ASSERT(page_size >= VMEM_SYS_PAGE_SIZE && (page_size & (VMEM_SYS_PAGE_SIZE - 1)) == 0, "invalid page size");

		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

#if defined(VMEM_USE_MALLOC)

		// ====================================================
		//					MALLOC decommit
		// ====================================================
		// do nothing
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_UNREFERENCED_PARAM(commit_flags);

#elif defined(VMEM_USE_PAGE_MAPPER)

		// ====================================================
		//			   PageMapper decommit (PS4)
		// ====================================================

		gp_VMemSysData->m_PhysicalPageMapper.UnMapPhysicalPages(p, size, commit_flags);

#elif defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_SWITCH)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_WIN)

		// ====================================================
		//					WIN decommit
		// ====================================================

		#ifdef VMEM_OS_WIN
			#pragma warning(push)
			#pragma warning(disable : 6250)		// disable decommit but not release warning
		#endif

		VMEM_UNREFERENCED_PARAM(commit_flags);

		BOOL b = ::VirtualFree(p, size, MEM_DECOMMIT);

		#ifdef VMEM_OS_WIN
			#pragma warning(pop)
		#endif

		if(!b)
			PrintLastError();
		VMEM_ASSERT(b, "VirtualFree failed");

#else

		// ====================================================
		//					LINUX decommit
		// ====================================================

		VMEM_UNREFERENCED_PARAM(commit_flags);

		mmap(p, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
		msync(p, size, MS_SYNC | MS_INVALIDATE);

#endif
		g_CommittedBytes -= size;
	}

	//------------------------------------------------------------------------
	size_t GetReservedBytes()
	{
		return g_ReservedBytes;
	}

	//------------------------------------------------------------------------
	size_t GetCommittedBytes()
	{
		return g_CommittedBytes;
	}

	//------------------------------------------------------------------------
	// this does not need to be implemented, it is only for debug purposes. You can just return true.
#ifdef VMEM_ASSERTS
	bool Committed(void* p, size_t size)
	{
#if defined(VMEM_OS_WIN)
		MEMORY_BASIC_INFORMATION mem_info;

		byte* p_check_p = (byte*)p;
		size_t check_size = size;
		while(check_size)
		{
			memset(&mem_info, 0, sizeof(mem_info));
			VirtualQuery(p_check_p, &mem_info, sizeof(mem_info));
			if(mem_info.State != MEM_COMMIT)
				return false;
			size_t reserve_size = VMin(check_size, (size_t)mem_info.RegionSize);
			check_size -= reserve_size;
			p_check_p += reserve_size;
		}

		return true;
#elif defined(VMEM_PLATFORM_PS4)
	#error Please contact slynch@puredevsoftware.com for this platform
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		return true;
#endif
	}
#endif

	//------------------------------------------------------------------------
	// this does not need to be implemented, it is only for debug purposes. You can just return true.
#ifdef VMEM_ASSERTS
	bool Reserved(void* p, size_t size)
	{
#ifdef VMEM_OS_WIN
		MEMORY_BASIC_INFORMATION mem_info;

		byte* p_check_p = (byte*)p;
		size_t check_size = size;
		while(check_size)
		{
			memset(&mem_info, 0, sizeof(mem_info));
			VirtualQuery(p_check_p, &mem_info, sizeof(mem_info));
			if(mem_info.State != MEM_RESERVE)
				return false;
			size_t reserve_size = VMin(check_size, (size_t)mem_info.RegionSize);
			check_size -= reserve_size;
			p_check_p += reserve_size;
		}

		return true;
#else
		VMEM_UNREFERENCED_PARAM(p);
		VMEM_UNREFERENCED_PARAM(size);
		return true;
#endif
	}
#endif

	//------------------------------------------------------------------------
	void DebugWrite(const _TCHAR* p_str, ...)
	{
#ifdef VMEM_ENABLE_STATS
		va_list args;
		va_start(args, p_str);

		const int max_string_length = 1024;
		static _TCHAR gString[max_string_length];
		_vstprintf_s(gString, max_string_length, p_str, args);
		Log(gString);

		va_end(args);
#endif
	}

	//------------------------------------------------------------------------
	void Break()
	{
#ifdef VMEM_OS_WIN
		DebugBreak();
#else
		assert(false);
#endif
	}

	//------------------------------------------------------------------------
	void DumpSystemMemory()
	{
#if defined(VMEM_PLATFORM_WIN)
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);

		DebugWrite(_T("There is  %7ld percent of memory in use.\n"), status.dwMemoryLoad);
		DebugWrite(_T("There are %7I64d total Kbytes of physical memory.\n"), status.ullTotalPhys/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of physical memory.\n"), status.ullAvailPhys/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of paging file.\n"), status.ullTotalPageFile/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of paging file.\n"), status.ullAvailPageFile/1024);
		DebugWrite(_T("There are %7I64d total Kbytes of virtual memory.\n"), status.ullTotalVirtual/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of virtual memory.\n"), status.ullAvailVirtual/1024);
		DebugWrite(_T("There are %7I64d free Kbytes of extended memory.\n"), status.ullAvailExtendedVirtual/1024);

#elif defined(VMEM_PLATFORM_XBOX360)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_PLATFORM_XBOXONE)
	#error Please contact slynch@puredevsoftware.com for this platform
#elif defined(VMEM_OS_LINUX)
		// not possible on LINUX

#else

		#error platform not defined

#endif
	}

	//------------------------------------------------------------------------
	void VMemSysSetLogFunction(void (*LogFn)(const _TCHAR* p_message))
	{
		g_LogFn = LogFn;
	}

	//------------------------------------------------------------------------
	void VMemSysCheckIntegrity()
	{
#ifdef VMEM_USE_PAGE_MAPPER
		CriticalSectionScope lock(gp_VMemSysData->m_CriticalSection);

		gp_VMemSysData->m_PhysicalPageMapper.CheckIntegrity();
#endif
	}

	//------------------------------------------------------------------------
	void VMemSysWriteStats()
	{
#if defined(VMEM_USE_PAGE_MAPPER) && defined(VMEM_ENABLE_STATS)
		Stats stats = gp_VMemSysData->m_PhysicalPageMapper.GetStats();
		DebugWrite(_T("\n"));
		DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		DebugWrite(_T("Phys Pages: "));		  VMem::WriteStats(stats);
#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
//
// VMemThread.cpp
//
//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	struct ThreadGlobalData
	{
		CriticalSection m_CriticalSection;
		ThreadMain mp_ThreadMain;
		void* mp_Context;
	};
	char gp_ThreadGlobalDataMem[sizeof(ThreadGlobalData)] VMEM_ALIGN_8 = {0};
	ThreadGlobalData* gp_ThreadGlobalData = NULL;

	//------------------------------------------------------------------------
	void Thread::Initialise()
	{
		VMEM_ASSERT(!gp_ThreadGlobalData, "VMem not shut down correctly or gp_ThreadGlobalData has been corrupted");
		gp_ThreadGlobalData = (ThreadGlobalData*)gp_ThreadGlobalDataMem;
		new (gp_ThreadGlobalData)ThreadGlobalData();
	}

	//------------------------------------------------------------------------
	void Thread::Destroy()
	{
		gp_ThreadGlobalData->~ThreadGlobalData();
		gp_ThreadGlobalData = NULL;
	}

	//------------------------------------------------------------------------
#if defined(VMEM_OS_WIN)
	unsigned long WINAPI PlatformThreadMain(void*)
	{
		ThreadMain p_thread_main = gp_ThreadGlobalData->mp_ThreadMain;
		gp_ThreadGlobalData->mp_ThreadMain = NULL;

		void* p_context = gp_ThreadGlobalData->mp_Context;
		gp_ThreadGlobalData->mp_Context = NULL;

		gp_ThreadGlobalData->m_CriticalSection.Leave();

		return p_thread_main(p_context);
	}
#elif defined(VMEM_OS_LINUX)
	void* PlatformThreadMain(void*)
	{
		ThreadMain p_thread_main = gp_ThreadGlobalData->mp_ThreadMain;
		gp_ThreadGlobalData->mp_ThreadMain = NULL;

		void* p_context = gp_ThreadGlobalData->mp_Context;
		gp_ThreadGlobalData->mp_Context = NULL;

		gp_ThreadGlobalData->m_CriticalSection.Leave();

		p_thread_main(p_context);

		return NULL;
	}
#else
	#error	// OS not defined
#endif

	//------------------------------------------------------------------------
	ThreadHandle Thread::CreateThread(ThreadMain p_thread_main, void* p_context)
	{
		gp_ThreadGlobalData->m_CriticalSection.Enter();
		gp_ThreadGlobalData->mp_ThreadMain = p_thread_main;
		gp_ThreadGlobalData->mp_Context = p_context;

	#if defined(VMEM_OS_WIN)
		return ::CreateThread(NULL, 0, PlatformThreadMain, NULL, 0, NULL);
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#elif defined(VMEM_OS_LINUX)
		pthread_t thread;
		pthread_create(&thread, NULL, PlatformThreadMain, NULL);
		return thread;
	#else
		#error	// OS not defined
	#endif
	}

	//------------------------------------------------------------------------
	// only used in dire circumstances when VMem is trying to shut down after a crash (in the VMem tests)
	void Thread::TerminateThread(ThreadHandle thread)
	{
	#if defined(VMEM_PLATFORM_WIN)
		::TerminateThread(thread, 0);
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#elif defined(VMEM_OS_LINUX) && !defined(VMEM_PLATFORM_ANDROID)
		pthread_cancel(thread);
	#else
		Break(_T("pthread_cancel not implemented on this platform"));
	#endif

		WaitForThreadToTerminate(thread);
	}

	//------------------------------------------------------------------------
	void Thread::WaitForThreadToTerminate(ThreadHandle thread)
	{
	#if defined(VMEM_OS_WIN)
		WaitForSingleObject(thread, INFINITE);
	#elif defined(VMEM_PLATFORM_PS4)
		#error Please contact slynch@puredevsoftware.com for this platform
	#elif defined(VMEM_OS_LINUX)
		pthread_join(thread, NULL);
	#else
		#error	// OS not defined
	#endif
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_SERVICE_THREAD

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE
