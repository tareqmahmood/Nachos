#include "copyright.h"
#include "memorymanager.h"
#include "system.h"


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


int MemoryManager::AllocPage(int processID, int vpn)
{
	memLock->Acquire();
	int physicalPage = map->Find();		// find a free page
	if(physicalPage != -1)
	{
		inversePageTable[physicalPage].processID = processID;
		inversePageTable[physicalPage].virtualPage = vpn;
		inversePageTable[physicalPage].lastUsed = stats->totalTicks;
	}
	memLock->Release();
	return physicalPage;
}


int MemoryManager::AllocByForce(int processID, int vpn)
{
	memLock->Acquire();

	// // round robin algorithm
	// int physicalPage = round;		// find a free page
	// round = (round + 1) % NumPhysPages;

	

	// find least recently used physical page
	unsigned int smallestTime = inversePageTable[0].lastUsed;
	int physicalPage = 0;
	for(int i = 1; i < NumPhysPages; i++)
	{
		if(inversePageTable[i].lastUsed < smallestTime)
		{
			physicalPage = i;
		}
	}
	DEBUG('p', "LRU Physical Page = %d\n", physicalPage);



	
	// time to write back this page
	Thread *thread = (Thread *)processTable->Get(inversePageTable[physicalPage].processID);
	ASSERT(thread != NULL);
	thread->space->evictPage(inversePageTable[physicalPage].virtualPage);

	// time to alloc
	inversePageTable[physicalPage].processID = processID;
	inversePageTable[physicalPage].virtualPage = vpn;
	inversePageTable[physicalPage].lastUsed = stats->totalTicks;

	memLock->Release();
	return physicalPage;
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