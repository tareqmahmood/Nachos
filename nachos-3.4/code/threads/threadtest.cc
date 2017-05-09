// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "buffer.h"
#include "consumer.h"
#include "producer.h"


// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

// void
// SimpleThread(int which)
// {
//     int num;
    
//     for (num = 0; num < 5; num++) {
// 	printf("*** thread %d looped %d times\n", which, num);
//         currentThread->Yield();
//     }
// }

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

// void
// ThreadTest1()
// {
//     DEBUG('t', "Entering ThreadTest1");

//     Thread *t = new Thread("forked thread");

//     t->Fork(SimpleThread, 1);
//     SimpleThread(0);
// }

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------



void ThreadTest()
{
	Lock* lock = new Lock("lock");
    Condition* conditionPut = new Condition("condition put");
    Condition* conditionGet = new Condition("condition get");
    Buffer* buffer = new Buffer(10);

    Producer::setup(buffer, lock, conditionPut, conditionGet);
    Consumer::setup(buffer, lock, conditionPut, conditionGet);


    for(int i = 0; i < 20; i++)
    {
    	new Producer(i);
        new Consumer(i);
    }

    //printf("\n----------------------------------\n\n\n\n");
}

