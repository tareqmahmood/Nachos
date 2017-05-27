#include "copyright.h"
#include "memorymanager.h"


MemoryManager::MemoryManager(int n)
{
	numPages = n;
	map = new BitMap(n);
	memLock = new Lock("Manager Lock");
	processMap = new int[n];
	round = 0;
	for(int i = 0; i < n; i++) 
	{
		processMap[i] = -1;			// unallocated
	}
}


MemoryManager::~MemoryManager()
{
	delete memLock;
	delete map;
}


int MemoryManager::AllocPage()
{
	memLock->Acquire();
	int pagePos = map->Find();		// find a free page
	memLock->Release();
	return pagePos;
}


int MemoryManager::AllocPage(int processNo, TranslationEntry &entry)
{
	memLock->Acquire();
	int pagePos = map->Find();		// find a free page
	memLock->Release();
	return pagePos;
}


int MemoryManager::AllocByForce()
{
	memLock->Acquire();
	int pagePos = round;		// find a free page
	round = (round + 1) % NumPhysPages;
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
	bool ret = map->Test(physPageNum);
	memLock->Release();
	return ret;
}