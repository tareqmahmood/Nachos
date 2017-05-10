#include "copyright.h"
#include "processtable.h"

/* Create a table to hold at most "size" entries. */
ProcessTable::ProcessTable(int n)
{
	size = n;
	processCount = 0;
	procLock = new Lock("ProcessTable Lock");
	table = new void*[size];
	procMap = new BitMap(size);
}


ProcessTable::~ProcessTable()
{
	delete procMap;
	delete table;
	delete procLock;
}

/* Allocate a table slot for "object", returning the "index" of the
allocated entry; otherwise, return -1 if no free slots are available. */
int ProcessTable::Alloc(void *object)
{
	procLock->Acquire();

	int index = procMap->Find();
	if(index == -1)
	{
		printf("No more space in process table.\n");
		ASSERT(FALSE);
	}

	table[index] = object;
	processCount++;

	procLock->Release();
	return index;
}


/* Retrieve the object from table slot at "index", or NULL if that
slot has not been allocated. */
void* ProcessTable::Get(int index)
{
	procLock->Acquire();

	bool allocated = procMap->Test(index);
	void* object = 0;
	if(allocated)
	{
		object = table[index];
	}

	procLock->Release();
	
	return object;
}


/* Free the table slot at index. */
void ProcessTable::Free(int index)
{
	procLock->Acquire();

	bool allocated = procMap->Test(index);
	if(allocated)
	{
		procMap->Clear(index);
		delete table[index];
		table[index] = 0;
		processCount--;
	}
	
	procLock->Release();
}


int ProcessTable::getProcessCount()
{
	procLock->Acquire();
	int cnt = processCount;
	procLock->Release();
	return cnt;
}