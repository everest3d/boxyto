### Introduction
Boxyto is a C++ 11, 14 library for real time and high performance applications that includes more of the standard library re-implemented, with many enhancements and new features.
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
- Compiler support (MSVC - CLANG - GNU)

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
- [Array]: Dynamic array that can also simulate stack and queue containers
- [DoubleLinkedList]: Double Linked List of connected nodes, container also support stack and queue
- [Map]: Red-Black tree based map
- [HashMap]: key-value hashed map
- [HashList]: Hashed key list
- [CircularArray]: Static array the work like queue
- [Pair]: Key-Value pair
- [Sets]: Not a container, but a set of functions that apply Set opreations on any type of container
- [ForEach]: Simplified for each that iterate over a container elements, and execute a function on them

#### Templates
- [Common]: Some template based support for other systems

### Usage
#### Using Memory manager
First, we need to initialize the memory manager:
```
using namespace Everest;

// Init memory manager
OSMemory::Init(SIZE_T size);
```
size, is the required size by the application for it's life-cycle, it's important to notice that all boxyto memory system depend on this.

When the application goes to end, we need to shutdown and release our memory manager:
```
// Shutdown memory manager
OSMemory::Terminate();
```
and that's it!

To use the memory manager, we need to create something called Segment Manager, and a segment is simply a segment of memory contains one or more system pages, the default page size is the huge system page size (most of time 2 MiB), witch is a TLB hit, you can query the system page size size by call
```
OSMemory::GetPageSize();
```

Segments are very big, we can directly use segments for allocations, or we can use allocators to direct interact with segments, here is an example:
```
// Create a dynamic segment
// note that pageCount is how many pages we need in this segment, 
// if left empty, that means one page (default)
DynamicSegment dynamicSegment(pageCount);

// Direct allocate from segment
// Alloc takes the allocation size, and allocation alignment (power of 2)
UINTPTR handle = dynamicSegment.Alloc(1024, 16);

// We use a handle because our segment allows defragmentation, and this will change pointer location in memory
// To obtain a pointer at any time
void* ptr = DynamicSegment::PointerOf(handle);

// if you would like to obtain an updated pointer nomatter when it called, use Pointer<T> class
Pointer<T> ptr = DynamicSegment::PointerOf(handle);
ptr->DoSomething();
// OR...
ptr.Get()->DoSomething();

// To deallocate an allocation
dynamicSegment.Dealloc(handle);

// To defragment a dynamic segment
// remember that defragmentation is consuming time heavly, use it wisely,
// for example: use it when we have remaining time in our game loop system
dynamicSegment.Defragment();

// To reset/release a segment
dynamicSegment.Release();
```

#### Using Containers
We will assume that we have configured DynamicSegment, also we will use Everest::Array for example
```
// Define out allocator for simple
typedef Everest::LinearAllocator<std::string> Allocator;

// Create a segment manager to consume
std::shard_ptr<DynamicSegment> segment(new DynamicSegment);

// Create an allocator instance
// allocators should constructed with a segment manager instance
Allocator alloc(segment);

// Create the array
// We then pass a configured allocator to the container
Everest::Array<std::string> array(alloc);

// The following is wrong for all boxyto containers
// Everest::Array<std::string> array;
```
What if you would like to use boxyto memory manager with std containers?

No problem, use Everest::StdAllocator class:
```
// Define the allocator type
typedef Everest::Allocator<std::string> StdAllocType;

// Create the allocator
// Std allocators can consume static segments only
StdAllocType stdAlloc(staticSegment);

// Create a std vector container
std::vector<std::string> myVec(stdAlloc);

// ... 
// Use your container as usual
```

### Documentation
All source code is inline documented

### Questions
Contact me with your suggestions and questions at:
a.essam.sayed@gmail.com

### License
MIT

Free Software, Hell Yeah!

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
[sets]: </boxyto/containers/sets.h>
[Common]: </boxyto/template/common.h>
