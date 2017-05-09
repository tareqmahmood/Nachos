#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "buffer.h"



class Consumer
{
	Thread* thr;
	int no;
public:
	Consumer(int _no);
	static void setup(Buffer* _buffer, Lock* _lock, Condition* _conditionPut, Condition* _conditionGet);
};
