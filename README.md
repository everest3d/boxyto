### Introduction
Boxyto is a C++ library for real time and high performance applications that includes more of the standard library re-implemented, with many enhancements and new features.
On the other hand, Boxyto showed much performance improvements in benchmarks regarding to standard library.

### Version
1.0.12

### Features
- Cross platform (Windows - Mac OS X - IOS - Android - Linux)
- Rich Templete support
- Complete memory management system
- Memory defragmentation support
- Different container classes
- Easy to use and extend
- Memory Alignment support
- Smart Pointers
- Standard library interchange support
- OS detection
- Compiter detection (MSVC - CLANG - GNU)

### Classes & Modules
#### Memory Manager 
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
- [MemoryOps]: Useful template functions for many pre-defined memory operations
- [PlatformMemory]: Headers to use by system for memory on OS

[OSMemory]: </boxyto/memory/OSMemory.h>
[OSMemory]: </boxyto/memory/OSMemory.h>
[OSMemory]: </boxyto/memory/OSMemory.h>
[OSMemory]: </boxyto/memory/OSMemory.h>
[OSMemory]: </boxyto/memory/OSMemory.h>
[OSMemory]: </boxyto/memory/OSMemory.h>
