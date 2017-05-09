#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "buffer.h"



class Producer
{
	Thread* thr;
	int no;
public:
	Producer(int _no);
	static void setup(Buffer* _buffer, Lock* _lock, Condition* _conditionPut, Condition* _conditionGet);
};
