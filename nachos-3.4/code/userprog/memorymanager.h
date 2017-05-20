#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "copyright.h"
#include "bitmap.h"
#include "synch.h"

/* Create a manager to track the allocation of numPages of physical memory.
You will create one by calling the constructor with NumPhysPages as
the parameter. All physical pages start as free, unallocated pages. */

class MemoryManager
{
	int numPages;
	Lock* memLock;
	BitMap* map;
	int processMap;
	TranslationEntry *entries;
public:
	MemoryManager(int n);
	~MemoryManager();

	/* Allocate a free page, returning its physical page number or -1 */
	int AllocPage();

	/* Allocate with process no and translation entry */
	int AllocPage(int processNo, TranslationEntry *entry);

	/* Allocate by force, for swapping */
	int AllocByForce();

	/* Free the physical page and make it available for future allocation. */
	void FreePage(int physPageNum);

	/* True if the physical page is allocated, false otherwise. */
	bool IsPageAllocated(int physPageNum);
};

#endif // MEMORYMANAGER_H