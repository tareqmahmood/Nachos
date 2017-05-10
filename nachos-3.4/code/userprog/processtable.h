#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "copyright.h"
#include "synch.h"
#include "bitmap.h"

#define MAX_PROCESS_NUMBER 10

/* Create a process table to keep track of processes */

class ProcessTable
{
	int size;
	Lock* procLock;
	BitMap* procMap;
	void** table;
	int processCount;
public:
	/* Create a table to hold at most "size" entries. */
	ProcessTable(int n);
	~ProcessTable();

	/* Allocate a table slot for "object", returning the "index" of the
	allocated entry; otherwise, return -1 if no free slots are available. */
	int Alloc(void *object);

	/* Retrieve the object from table slot at "index", or NULL if that
	slot has not been allocated. */
	void* Get(int index);

	/* Free the table slot at index. */
	void Free(int index);

	/* Returns number of active processes */
	int getProcessCount();
};

#endif // PROCESSTABLE_H