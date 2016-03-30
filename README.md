### Introduction
Boxyto is a C++ library for real time and high performance applications that includes more of the standard library re-implemented, with many enhancements and new features.
On the other hand, Boxyto showed much performance improvements in benchmarks regarding to standard library.

### Version
1.0.12

### Features
- Cross platform (Windows - Mac OS X - IOS - Android - Linux)
- 32-bit & 64-bit support 
- Rich Templete support
- Complete memory management system
- Memory defragmentation support
- Different container classes
- Containers iterators
- Easy to use and extend
- Memory Alignment support
- Smart Pointers
- Standard library interchange support
- OS detection
- Compiter detection (MSVC - CLANG - GNU)

### Classes & Modules
#### Memory Manager 
- [MemoryOps]: Useful template functions for many pre-defined memory operations
- [OSMemory]: Operating system memory initializer and terminator
- [StaticSegment]: Segment manager for static allocations
- [DynamicSegment]: Segment manager for dynamic allocations, support defragmentation and other enhancments
- [LinearAllocator]: Allocator can be used with linear containers, such as Arrays
- [PoolNodeAllocator]: Allocator can be used with Node based containers, such as Linked Lists, with the idea of per-allocate capacity of nodes and recycling them
- [FastNodeAllocator]: Allocator used with StaticSegment, and can work with Node based containers
- [SimpleNodeAllocator]: Allocator used by Node based containers, this allocator allocate nodes on request
- [Pointer]: Different Pointer types to use with containers
- [ShadredPointer]: std::shadred_pointer like class
- [UniquePointer]: std::unique_pointer like class
- [StdAllocator]: Allocator to use with all std containers to consume boxyto memory manager
- [PlatformMemory]: Headers to use by system for memory on OS

#### Containers
Each container have iterators, search/find, and can input/output to other types of container
- [Array]
- [DoubleLinkedList]
- [Map]: Red-Black tree based map
- [HashMap]: key-value hashed map
- [HashList]
- [CircularArray]
- [Pair]
- [Sets]: Not a container, but a set of functions that apply Set opreations on any type of container
- [ForEach]: Simplified for each that iterate over a container elements, and execute a function on them

#### Templates
- [Common]: Some template based support for other systems

### Usage


[OSMemory]: </boxyto/memory/OSMemory.h>
[StaticSegment]: </boxyto/memory/StaticSegment.h>
[DynamicSegment]: </boxyto/memory/DynamicSegment.h>
[LinearAllocator]: </boxyto/memory/LinearAllocator.h>
[PoolNodeAllocator]: </boxyto/memory/PoolNodeAllocator.h>
[FastNodeAllocator]: </boxyto/memory/FastNodeAllocator.h>
[SimpleNodeAllocator]: </boxyto/memory/SimpleNodeAllocator.h>
[Pointer]: </boxyto/memory/Pointer.h>
[ShadredPointer]: </boxyto/memory/SmartPointers.h>
[UniquePointer]: </boxyto/memory/SmartPointers.h>
[StdAllocator]: </boxyto/memory/StdAllocator.h>
[MemoryOps]: </boxyto/memory/MemoryOps.h>
[PlatformMemory]: </boxyto/memory/PlatformMemory.h>
[Array]: </boxyto/containers/Array.h>
[DoubleLinkedList]: </boxyto/containers/list.h>
[Map]: </boxyto/containers/Map.h>
[HashMap]: </boxyto/containers/Map.h>
[HashList]: </boxyto/containers/list.h>
[CircularArray]: </boxyto/containers/CircularArray.h>
[Pair]: </boxyto/containers/Pair.h>
[ForEach]: </boxyto/containers/ForEach.h>
[Common]: </boxyto/template/common.h>
