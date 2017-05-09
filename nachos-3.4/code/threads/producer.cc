#include "producer.h"
#include <unistd.h>

int a;
Lock* lockP;
Buffer* bufferP;
Condition* conditionPutP;
Condition* conditionGetP;

int randomP(int seed)
{
	return ((seed + 227621) + 1) % 3456789 + 100000000;
}

void produce(int which)
{
	printf("Producer %d started\n", which);
	int loopCount = which, c;
	while(1) {
		lockP->Acquire();

		while(bufferP->isFull())
		{
			conditionPutP->Wait(lockP);
		}

		// delay 1
		c = loopCount = randomP(loopCount);
		//sleep(loopCount % 3 + 1);
		while(c > 0) c--;

		bufferP->Push(a);
		printf("<< Producer %d produced %d\n", which, a);
		a++;

		conditionGetP->Signal(lockP);
		lockP->Release();

		// delay 2
		c = loopCount = randomP(loopCount);
		//sleep(loopCount % 3 + 1);
		while(c > 0) c--;
	}
}



Producer::Producer(int _no)
{
	no = _no;
	thr = new Thread("Producer");
	thr->Fork(produce, no);
}

void Producer::setup(Buffer* _buffer, Lock* _lock, Condition* _conditionPut, Condition* _conditionGet)
{
	bufferP = _buffer;
	lockP = _lock;
	conditionPutP = _conditionPut;
	conditionGetP = _conditionGet;
	a = 0;
}

