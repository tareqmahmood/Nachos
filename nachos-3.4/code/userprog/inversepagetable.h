#ifndef INVERSEPAGETABLE_H
#define INVERSEPAGETABLE_H

#include "copyright.h"

class InversePageTable
{
public:
	int virtualPage;
	int processID;
	unsigned int lastUsed;
};

#endif // INVERSEPAGETABLE_H