#include "consumer.h"
#include <unistd.h>

Lock* lockC;
Buffer* bufferC;
Condition* conditionPutC;
Condition* conditionGetC;

int randomC(int seed)
{
	return ((seed + 229431) + 1) % 3456789 + 100000000;
}


void consume(int which)
{
	printf("Consumer %d started\n", which);
	int loopCount = which, c;
	while(1) {
		lockC->Acquire();

		while(bufferC->isEmpty())
		{
			conditionGetC->Wait(lockC);
		}

		// loop
		c = loopCount = randomC(loopCount);
		//sleep(loopCount % 3 + 1);
		while(c > 0) c--;

		int a = bufferC->Pop();
		printf(">> \tConsumer %d consumed %d\n", which, a);

		conditionPutC->Signal(lockC);
		lockC->Release();

		// loop
		c = loopCount = randomC(loopCount);
		//sleep(loopCount % 3 + 1);
		while(c > 0) c--;
	}
}

Consumer::Consumer(int _no)
{
	no = _no;
	thr = new Thread("Consumer");
	thr->Fork(consume, no);
}

void Consumer::setup(Buffer* _buffer, Lock* _lock, Condition* _conditionPut, Condition* _conditionGet)
{
	bufferC = _buffer;
	lockC = _lock;
	conditionPutC = _conditionPut;
	conditionGetC = _conditionGet;
}



