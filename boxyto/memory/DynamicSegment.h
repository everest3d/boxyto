/*****************************************************************************
The MIT License(MIT)

Copyright(c) 2016 Amr Esam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*********************************************************************************/
#pragma once

#include "../system.h"
#include "OSMemory.h"
#include "../Template/Common.h"  

#include <math.h>
#include <iostream> /* For basic print log */

namespace Everest
{
#pragma warning( push )
#pragma warning( disable : 4244)
	class DynamicSegment
	{
	private:

		// Define packet size
		enum { PACKET_SIZE = 16, MIN_PACKETS = 64 }; // Default is 1 KB

		enum { FLAG_CHUNK_STATUS = 1 }; // Chunk status flag


		// Chunk desc for every allocation, 32 bytes
		ALIGN_(sizeof(void*)) struct ChunkDesc
		{
			// The actul pointer to map in the lookup table
			void* MappedPtr;

			// Packets count
			uint32 PacketCount;

			// FREE linked list indcies, used with free chunks only
			int32 PrevFreeIndex;
			int32 NextFreeIndex;

			// Table linked list indcies, used for ALL chunks in the table
			int32 PrevIndex;
			int32 NextIndex;

			// ArbitraryAllocator chunk flags
			int32 Flags;			

#ifdef SYS_32BITS
			// Padding to fit 32 bytes in 32-bit systems
			ubyte[4] padding;
#endif
		};

		enum { CHUNK_DESC_SIZE = sizeof(ChunkDesc) }; // We know that ChunkDesc struct always 16 bytes

		// Allocator table to map worest-case allocations
		// This is array of ChunkDesc allocated at the beginging of the Raw
		// It also helps reduce memory footprint for searching free &
		// make defragmentaion easier
		ChunkDesc* lookupTable;

		// Array of pointers that are released
		int32* recycledPointers;

		// Pointer to the allocator memory memory ready for allocation
		void* memory;

		// Raw memory, this is allocated by the OS for allocator memory & data
		void* raw;

		// Segment size in bytes
		SIZE_T rawSize;

		// Actual memory ready for allocation size
		SIZE_T memorySize;

		// Count of currently recycled pointers. 
		// if this is 0, then we try to get a new pointer using CurrentChunkIndex
		int32 RecycledPointersCount;

		// Index to the last used chunk from the lookup table.
		// We use this when no recycled pointers
		int32 CurrentChunkIndex;

		// Index to the recent freed chunk
		// typically, this is 0 and set to all memory when initialized
		int32 freeHead;


	public:

		// Default constructor that allocate segment to one huge system page as default size
		DynamicSegment() :
			lookupTable(nullptr),
			recycledPointers(nullptr),
			memory(nullptr),
			raw(nullptr),
			rawSize(OSMemory::GetPageSize()),
			RecycledPointersCount(0),
			CurrentChunkIndex(0),
			freeHead(INDEX_NONE)
		{
			// Reserve memory from OS to this allocator
			raw = OSMemory::RequestSegment(rawSize);

			// Initialize OR reset the segment
			Init();
		}

		// Construct segment with count of segments
		// size = count * system-page-size
		//
		// @param: segCount - Count of segments to allocate, one segment = default page size
		DynamicSegment(uint32 segCount) :
			lookupTable(nullptr),
			recycledPointers(nullptr),
			memory(nullptr),
			raw(nullptr),
			rawSize(segCount * OSMemory::GetPageSize()),
			RecycledPointersCount(0),
			CurrentChunkIndex(0),
			freeHead(INDEX_NONE)
		{
			// Reserve memory from OS to this allocator
			raw = OSMemory::RequestSegment(rawSize);

			// Init this allocator
			Init();
		}

		// Delete equality constructor
		DynamicSegment(const DynamicSegment&) = delete;
		DynamicSegment& operator= (const DynamicSegment&) = delete;

		// SegmentManager destructor
		~DynamicSegment()
		{
			Release();
		}

		// Release this segment to the os memory
		// The allocations in this segment is no longer valid
		void Release()
		{
			OSMemory::ReleaseSegment(raw);
		}

		// Initialize OR reset the segment
		void Init()
		{
			uint32 packetCount = 0;
			uint32 allocationCount = 0;

			// Calculate allocator packet count
			packetCount = ((double)rawSize / (double)PACKET_SIZE);
			packetCount -= ((CHUNK_DESC_SIZE * packetCount) / (PACKET_SIZE * MIN_PACKETS));

			// Real allocations count, for min packet count per allocation
			allocationCount = rawSize / (PACKET_SIZE * MIN_PACKETS);

			// Setting lookup table to the first of the raw memory
			// Aligned to 16
			lookupTable = (ChunkDesc*)(Align(raw, ALIGNMENT_OPTIMAL));

			// Setting recycled pointers to the offset of the end of lookupTable
			// Aligned to int32
			recycledPointers = (int32*)(Align
				(Offset(raw, (allocationCount * CHUNK_DESC_SIZE)), alignof(int32)));

			// Setting memory for data
			memory = Offset(recycledPointers, (allocationCount * sizeof(int32)));

			// Reset recycled count and chunk index
			RecycledPointersCount = 0;
			CurrentChunkIndex = 0;

			// Set all memory as one free chunk, at index 0
			freeHead = 0;
			MemWrite(freeHead, memory, 0); // White chunk index
			memory = Offset(memory, sizeof(freeHead)); // Advance memory by size of index

			// Set first element in lookup table array
			ChunkDesc& desc = lookupTable[0];
			desc = 
			{ 
				memory, 
				packetCount, 
				INDEX_NONE, 
				INDEX_NONE,
				INDEX_NONE,
				INDEX_NONE,
				ClearBit(desc.Flags, FLAG_CHUNK_STATUS)
			};
		}

		// Allocate memory using this segment's private memory
		// The allocation is aligned by alignment
		//
		// @param: size - The size in bytes to allocate
		// @param: alignment(16) - The alignment of the allocation
		// @return: Handle to the allocated memory
		UINTPTR Alloc(SIZE_T size, uint32 alignment = ALIGNMENT_OPTIMAL)
		{
			// LOG: check alignment is a power of 2 ?

			// Find how many packets we need
			// We add 4 bytes to store index + alignment
			SIZE_T packets = GetPacketCount(size + alignment + sizeof(uint32));

			int32 current = freeHead;

			// Check for free chunk of almost the same size
			ChunkDesc& freeDesc = lookupTable[current];
			while (current != INDEX_NONE)
			{
				if (packets >= freeDesc.PacketCount // Take the whole chunk without split
					|| (packets - freeDesc.PacketCount) < MIN_PACKETS)
				{
					// Unlink free chunk
					UnlinkFree(&freeDesc);

					// Update used flag
					freeDesc.Flags = SetBit(freeDesc.Flags, FLAG_CHUNK_STATUS);

					// Use current chunk for allocation
					freeDesc.MappedPtr = AlignW(freeDesc.MappedPtr, alignment); // Calc alignment
					return (UINTPTR(&freeDesc));
				}
				else if (packets < freeDesc.PacketCount) // Split packet and take what we need
				{
					// Retrive an index to store the splitted free chunk
					int32 validIndex = RecycledPointersCount ? 
						recycledPointers[RecycledPointersCount--] : ++CurrentChunkIndex;

					// Create the new allocation chunk
					ChunkDesc& newChunk		= lookupTable[validIndex];
					newChunk.MappedPtr		= freeDesc.MappedPtr;
					newChunk.PacketCount	= packets;
					newChunk.PrevFreeIndex	= INDEX_NONE;
					newChunk.NextFreeIndex	= INDEX_NONE;
					newChunk.NextIndex		= current;
					newChunk.PrevIndex		= freeDesc.PrevIndex;
					newChunk.Flags			= SetBit(newChunk.Flags, FLAG_CHUNK_STATUS);

					// Update the free chunk	
					freeDesc.MappedPtr		= Offset(freeDesc.MappedPtr, packets * PACKET_SIZE);
					freeDesc.PacketCount	= freeDesc.PacketCount - packets;
					if (freeDesc.PrevIndex != INDEX_NONE)
						lookupTable[freeDesc.PrevIndex].NextIndex = validIndex;
					freeDesc.PrevIndex = validIndex;

					// Updateing lookup indcies
					MemWrite(validIndex, newChunk.MappedPtr, -(int32)sizeof(int32));
					MemWrite(current, freeDesc.MappedPtr, -(int32)sizeof(int32));

					newChunk.MappedPtr = AlignW(newChunk.MappedPtr, alignment); // Calc alignment
					return (UINTPTR(&newChunk));
				}
				

				current = freeDesc.NextFreeIndex;
			}

			// LOG: Allocator is full
			return (UINTPTR)nullptr;
		}

		// Deallocate memory and release it back to the allocator
		//
		// @param: handle - Memory handle to release
		void Dealloc(const UINTPTR& handle)
		{
			if (!handle) return;

			ChunkDesc* current = (ChunkDesc*)handle;
			
			// Check if requested chunk is already free
			if (!CheckBit(current->Flags, FLAG_CHUNK_STATUS))
				return;

			// Unalign chunk pointer
			current->MappedPtr = UnalignW(current->MappedPtr);

			ChunkDesc* prev = (current->PrevIndex == INDEX_NONE ? nullptr : &lookupTable[current->PrevIndex]);
			ChunkDesc* next = (current->NextIndex == INDEX_NONE ? nullptr : &lookupTable[current->NextIndex]);

			if ((prev && !CheckBit(prev->Flags, FLAG_CHUNK_STATUS))
				&& (next && !CheckBit(next->Flags, FLAG_CHUNK_STATUS))) // Both prev & next are exsist & free
			{
				// Sum all packets in the prev chunk
				prev->PacketCount += current->PacketCount;
				prev->PacketCount += next->PacketCount;

				// Fix prev & next links
				Unlink(current);
				Unlink(next);
				UnlinkFree(next);

				// Recycle current & next indcies
				RecycleIndex((handle - (UINTPTR)lookupTable) / CHUNK_DESC_SIZE);
				RecycleIndex(((UINTPTR)next - (UINTPTR)lookupTable) / CHUNK_DESC_SIZE);
			}
			else if (prev && !CheckBit(prev->Flags, FLAG_CHUNK_STATUS)) // prev is exsist & free
			{
				prev->PacketCount += current->PacketCount;
				Unlink(current);
				RecycleIndex((handle - (UINTPTR)lookupTable) / CHUNK_DESC_SIZE);
			}
			else if (next && !CheckBit(next->Flags, FLAG_CHUNK_STATUS)) // next is exsist & free
			{
				current->PacketCount += next->PacketCount;
				
				Unlink(next);
				UnlinkFree(next);
				
				RecycleIndex(((UINTPTR)next - (UINTPTR)lookupTable) / CHUNK_DESC_SIZE);

				// Update current as free
				SetFree(current);
			}
			else // prev & next are not free
			{
				SetFree(current);
			}
		}

		// Deallocate a pointer allocated with this segment
		// IMPORTANT: Use this method only when ptr allocated after the last defragmentation
		//
		// @param: ptr - Pointer to memory to deallocate
		void Dealloc(void* ptr)
		{
			Dealloc((UINTPTR)&lookupTable[IndexOf(ptr)]);
		}

		// Reallocate a handle previously allocated by Alloc
		//
		// @param: oldHandle - The old handle to reallocate
		// @param: newSize - New size in bytes
		// @param: alignment(ALIGNMENT_OPTIMAL) - The alignment for the reallocated handle
		UINTPTR Realloc(UINTPTR oldHandle, SIZE_T newSize, uint32 alignment = ALIGNMENT_OPTIMAL)
		{
			ChunkDesc* oldChunk = (ChunkDesc*)oldHandle;
			if (!oldChunk)
				return (UINTPTR)nullptr;

			// Unalign chunk pointer
			//oldChunk->MappedPtr = UnalignW(oldChunk->MappedPtr);

			// Find how many packets we need
			SIZE_T packets = GetPacketCount(newSize + alignment + sizeof(uint32));

			if (packets == oldChunk->PacketCount)
			{
				// New allocation size is the same as old size OR they have the same number of packets
				// We just return the oldChunk pointer
				return oldHandle;
			}
			else if (packets > oldChunk->PacketCount)
			{
				// New size > old size,
				// Try to merge old chunk with next chunk (if free)
				ChunkDesc* next = (oldChunk->NextIndex == INDEX_NONE ? nullptr : &lookupTable[oldChunk->NextIndex]);
				if (next && !CheckBit(next->Flags, FLAG_CHUNK_STATUS)) // Next chunk is FREE
				{
					uint32 needPackets = packets - oldChunk->PacketCount;
					
					if ((needPackets == next->PacketCount) 
						|| ((next->PacketCount - needPackets) < MIN_PACKETS)) // Found free chunk that is equal to needed packets
					{
						// Unlink next chunk
						RecycleIndex(oldChunk->NextIndex);
						Unlink(next);
						UnlinkFree(next);

						// Update old chunk
						oldChunk->PacketCount += needPackets;

						// Return old chunk handle
						return oldHandle;
					}
					else if (needPackets < next->PacketCount) // Found enought packets in the next chunk
					{
						// Update next chunk by shrinking it
						next->MappedPtr = Offset(next->MappedPtr, needPackets * PACKET_SIZE);
						next->PacketCount -= needPackets;
						MemWrite(oldChunk->NextIndex, next->MappedPtr, -(int32)sizeof(int32));

						// Update old chunk by expanding it
						oldChunk->PacketCount += needPackets;

						// Return old chunk handle
						return oldHandle;
					}
				}
			}
			
			// Allocate new chunk with new size
			UINTPTR newChunk = Alloc(newSize, alignment);

			// Copy old contents to it
			memcpy(((ChunkDesc*)newChunk)->MappedPtr, oldChunk->MappedPtr, newSize);

			// free old chunk
			Dealloc(oldHandle);
			
			return newChunk;
		}

		void Defragment()
		{
			//TODO:...
		}

		// New function that allocate and construct empty an object
		// if you want to use the engine allocator
		//
		// @return: Pointer to the allocated type
		template<class Type>
		UINTPTR _NEW()
		{
			UINTPTR handle = Alloc(sizeof(Type), alignof(Type)); // TODO: FOR NOW!
			new(((ChunkDesc*)handle)->MappedPtr) Type();
			return handle;
		}

		// New function that allocate and construct an object
		// Constructing object version
		// if you want to use the engine allocator
		//
		// @param: args - The constructing data
		// @return: Pointer to the allocated type
		template<class Type, class ...Args>
		UINTPTR _NEW(Args && ...args)
		{
			UINTPTR handle = Alloc(sizeof(Type), alignof(Type));
			new(((ChunkDesc*)handle)->MappedPtr) Type(_EVEREST Forward<Args>(args)...);
			return handle;
		}

		void _DELETE(UINTPTR& handle)
		{
			Dealloc(handle);
		}

		// Generic parser, that parse a type pointer from a given handle
		// 
		// @param: handle - The allocation handle to parse
		// @return: The parsed handle
		static void* PointerOf(UINTPTR handle)
		{
			return (reinterpret_cast<ChunkDesc*>(handle))->MappedPtr;
		}

		

	private:

		void Unlink(ChunkDesc* chunk)
		{
			if (chunk->NextIndex != INDEX_NONE)
				lookupTable[chunk->NextIndex].PrevIndex = chunk->PrevIndex;
			if (chunk->PrevIndex != INDEX_NONE)
				lookupTable[chunk->PrevIndex].NextIndex = chunk->NextIndex;
		}

		void UnlinkFree(ChunkDesc* chunk)
		{
			if (chunk->NextFreeIndex != INDEX_NONE)
				lookupTable[chunk->NextFreeIndex].PrevFreeIndex = chunk->PrevFreeIndex;
			if (chunk->PrevFreeIndex != INDEX_NONE)
				lookupTable[chunk->PrevFreeIndex].NextFreeIndex = chunk->NextFreeIndex;
		}

		void RecycleIndex(int32 index)
		{
			*&recycledPointers[RecycledPointersCount++] = index;
		}

		void SetFree(ChunkDesc* chunk)
		{
			int32 newIndex = ((UINTPTR)chunk - (UINTPTR)lookupTable) / CHUNK_DESC_SIZE;
			lookupTable[freeHead].PrevFreeIndex = newIndex;
			chunk->NextFreeIndex = freeHead;
			chunk->Flags = ClearBit(chunk->Flags, FLAG_CHUNK_STATUS);
			freeHead = newIndex;
		}

		uint32 GetPacketCount(SIZE_T size)
		{
			// Find how many packets we need
			uint32 packets = ceil((double)(size) / (double)PACKET_SIZE);

			// Validate packets
			packets = packets < MIN_PACKETS ? MIN_PACKETS : packets;

			return packets;
		}

		// Return index of attached lookup desc element to an allocated memory
		uint32 IndexOf(const void* ptr)
		{
			return MemRead<uint32>(UnalignW(ptr), -(int32)sizeof(int32));
		}


		/*****************
		 *	DEBUG SUPPORT
		 *****************/

	public:
		// Simple development logging function that shows
		//  1) All allocations in order and their status
		//  2) Recycled pointers
		void Print()
		{
			// Logging allocations
			std::cout << "All Allocations" << std::endl;
			std::cout << "---------------" << std::endl;
			int32 index = MemRead<int32>(memory, -4);

			do
			{
				ChunkDesc& current = lookupTable[index];
				std::cout << "Index:      " << index << std::endl;
				std::cout << "Pointer:    " << current.MappedPtr << std::endl;
				std::cout << "Packets:    " << current.PacketCount << std::endl;
				std::cout << "Links:      " << "Prev(" << current.PrevIndex << "), Next(" << current.NextIndex << ")" << std::endl;
				std::cout << "Free Links: " << "Prev(" << current.PrevFreeIndex << "), Next(" << current.NextFreeIndex << ")" << std::endl;

				const char* status = CheckBit(current.Flags, FLAG_CHUNK_STATUS) ? "ALLOCATED" : "FREE";
				std::cout << "Status:     " << status  << std::endl;

				std::cout << "---------------------------------------" << std::endl;
				index = current.NextIndex;
			} while (index != INDEX_NONE);

			// Logging recycled pointers
			std::cout << std::endl;
			std::cout << "Recycled Pointers" << std::endl;
			std::cout << "-----------------" << std::endl;
			int32 recycleIndex = 0;
			while (recycleIndex != RecycledPointersCount)
				std::cout << recycledPointers[recycleIndex++] << ", ";
		}

	};

#pragma warning( pop ) 
}