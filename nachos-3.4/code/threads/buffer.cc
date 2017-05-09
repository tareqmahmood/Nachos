

#include "copyright.h"
#include "buffer.h"

Buffer::Buffer(int n)
{
	capacity = n;
	size = front = back = 0;
	buff = new int[capacity];
}

void Buffer::Push(int item)
{
	buff[front] = item;
	front = (front + 1) % capacity;
	size++;
}

int Buffer::Pop()
{
	int item = buff[back];
	back = (back + 1) % capacity;
	size--;
	return item;
}

bool Buffer::isFull()
{
	return size == capacity;
}


bool Buffer::isEmpty()
{
	return size == 0;
}
