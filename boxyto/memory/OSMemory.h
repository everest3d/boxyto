#pragma once

//TODO: Cross-Platform OS Memory, for now we only implement windows

#include "../system.h"
#include "../Template/Common.h"
#include "MemoryOps.h"

#include "PlatformMemory.h"

#include <math.h>
#include <vector>

namespace Everest
{
	class OSMemory
	{
	private:
		// Struct to store OS allocation data
		ALIGN_(sizeof(uint32)) struct PageDesc
		{
			PageDesc() : 
				count(0), flags(false)
			{}
			uint32 count; // Count of pages in this segment
			bool flags; // Free flag, true if allocated
		};

	public:

#ifdef OS_WINDOWS // Windows system memory allocation

		// Initialize the memory manager for all allocators
		// This is done by allocating memory in system large pages (2 MiBs per page)
		// We manage one or many pages in one segment using various types of allocators
		// Allocators use RequestSegment(SIZE_T) to allocate memory from a segment
		// Windows version
		//
		// @param: size - Configured size to allocate from system memory in huge pages,
		//                size MUST be a multiple of the large-page minimum
		static void Init(SIZE_T size)
		{ 
			// Calc how many system pages we need
			pageCount = CalcPages(size);
			
			// Try allocate Huge Pages
			memory = AllocHuge(size);

			// Huge allocate failed, try normal allocation instead
			if (!memory)
				memory = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

			//DWORD dw = GetLastError();

			// Create free list
			CreateMappingList();
		}

		// Allocate huge segmants from OS, segmant size is depending on the OS
		//
		// @param: size - Size of segmants to allocate
		// @return: Pointer to the allocated memory
		static void* AllocHuge(SIZE_T size)
		{
			// Try to allocate using large pages
			// Handle privilege
			HANDLE hToken;
			LONG Res;
			DWORD dw;
			TOKEN_PRIVILEGES tp;

			// Open process tocket an acquire handle
			Res = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
			if (!Res)
			{
				dw = GetLastError();
				// TODO: Log - Unable to get process token + dw
				return nullptr;
			}

			// Look up the SE_LOCK_MEMORY_NAME 
			Res = LookupPrivilegeValue(L"", SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid);
			if (!Res)
			{
				dw = GetLastError();
				// TODO: Log - Unable to get process token + dw
				return nullptr;
			}

			void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
			dw = GetLastError();
			if (dw != ERROR_SUCCESS || mem == NULL)
			{
				// TODO: Log - Unable to get process token + dw
				return nullptr;
			}

			return mem;
		}

		// Return OS huge page size in bytes
		static SIZE_T GetPageSize()
		{
			return GetLargePageMinimum();
		}

		// Release system allocated memory
		// This should only called by the engine shutdown routine
		static void Terminate()
		{
			if (memory != nullptr)
				VirtualFree(memory, 0, MEM_RELEASE);
		}

#elif OS_IOS || OS_MACOSX // IOS or OSX

		static void Init(SIZE_T size)
		{
			// Calc how many system pages we need
			pageCount = CalcPages(size);

			// Try allocate Huge Pages
			memory = valloc(pageCount * GetPageSize());

			if (memory == nullptr)
			{
				// LOG : No enought memory OR out of memory
			}

			// Create free list
			CreateMappingList();
		}

		// Return OS huge page size in bytes
		static SIZE_T GetPageSize()
		{
			// 2 MiG or more
			return getpagesize() >= 2 * 1024 * 1024? getpagesize() : 2 * 1024 * 1024;
		}

		// Release system allocated memory
		// This should only called by the engine shutdown routine
		static void Terminate()
		{
			if (memory != nullptr)
				free(memory);
		}

#else // Android & other linux OSs

		static void Init(SIZE_T size)
		{
			// Calc how many system pages we need
			pageCount = CalcPages(size);

			// Try allocate Huge Pages
			memory = memalign(sysconf(_SC_PAGESIZE), pageCount * GetPageSize());

			if (memory == nullptr)
			{
				// LOG : No enought memory OR out of memory
			}

			// Create free list
			CreateMappingList();
		}

		// Return OS huge page size in bytes
		static SIZE_T GetPageSize()
		{
			// 2 MiG or more
			return getpagesize() >= 2 * 1024 * 1024 ? getpagesize() : 2 * 1024 * 1024;
		}

		// Release system allocated memory
		// This should only called by the engine shutdown routine
		static void Terminate()
		{
			if (memory != nullptr)
				free(memory);
		}

#endif

		// Requesting a segment of page or more from the OS Memory manager
		// 
		// @param: size - The segment size in bytes
		// @return: Pointer to the reserved segment
		static void* RequestSegment(SIZE_T size)
		{
			// TODO: require lock
			// ...

			// Calc pages we need
			uint32 pcount = CalcPages(size);

			// Hold found contiguous pages
			uint32 found = 0;

			auto& it = pages.begin();
			for (; it != pages.end(); it++)
			{ 
				if (!it->flags)
				{
					if (++found == pcount)
					{
						// Found requested pages
						it -= (found -1);
						it->count = found; // Set count of pages
						it->flags = true; // Set allocated flag

						// Map memory pointer to this page
						SIZE_T index = ((SIZE_T)(UINTPTR)it._Ptr - (UINTPTR)pages.data()) / sizeof(PageDesc);
						return (Offset(memory, GetPageSize() * index));
					}
				}
				else
				{
					it += (it->count - 1);
					found = 0;
				}
			}

			// LOG: OUT OF MEMORY
			return nullptr;
		}

		// Release a segment to the OS Memory manager, previously allocated
		// 
		// @param: Pointer to the segment memory
		static void ReleaseSegment(void* ptr)
		{
			// Map the page where the segment starts
			SIZE_T index = ((UINTPTR)ptr - (UINTPTR)memory) / GetPageSize();
			auto& start = pages.begin() + index;
			auto& end = start + pages[index].count;
			
			// Reset segment's pages
			for (; start != end; ++start)
				*start = PageDesc();
		}

	private:

		// Init pages to segments mapping 
		static void CreateMappingList()
		{
			// Init segments descs
			pages.resize(pageCount);

			// Set first segment's count to all pages are free
			//pages[0].count = pageCount;
		}

		// Find how many pages from a given size in bytes
		static uint32 CalcPages(SIZE_T size)
		{
			return (uint32)(ceil((double)size / (double)GetPageSize()));
		}

	private:
		
		// The main memory allocated by system 
		static void* memory;

		// Allocation pages count based on requested size and system huge page size
		static uint32 pageCount;

		// All pages to segments mapping
		static std::vector<PageDesc> pages;

	};

	
}