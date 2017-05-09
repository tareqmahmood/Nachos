#include "copyright.h"
#include "memorymanager.h"


MemoryManager::MemoryManager(int n)
{
	numPages = n;
	map = new BitMap(n);
	memLock = new Lock("Memory Lock");
}

MemoryManager::~MemoryManager()
{
	delete map;
	delete memLock;
}

int MemoryManager::AllocPage()
{
	memLock->Acquire();
	int pagePos = map->Find();		// find a free page
	memLock->Release();
	return pagePos;
}

void MemoryManager::FreePage(int physPageNum)
{
	memLock->Acquire();
	map->Clear(physPageNum);
	memLock->Release();
}

bool MemoryManager::IsPageAllocated(int physPageNum)
{
	memLock->Acquire();
	return map->Test(physPageNum);
	memLock->Release();
}