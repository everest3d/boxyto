#include "OSMemory.h"

using namespace Everest;


void* OSMemory::memory = nullptr;

uint32 OSMemory::pageCount = 0;

std::vector<OSMemory::PageDesc> OSMemory::pages;
