#ifndef BUFFER_H
#define BUFFER_H

#include "copyright.h"
#include "utility.h"

class Buffer
{
	int* buff;
	int front;
	int back;
	int size;
	int capacity;
public:
	Buffer(int n);
	void Push(int item);
	int Pop();
	bool isFull();
	bool isEmpty();
};


#endif	// BUFFER_H


