// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"


// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics
extern Timer *timer;				// the hardware alarm clock

				
#ifdef USER_PROGRAM
#include "machine.h"
extern Machine* machine;	// user program memory and registers

    // **************** My Code Starts **************** //

#include "memorymanager.h"

extern MemoryManager *memorymanager;	// manage page allocation
extern Lock *memoryLock;				// lock on memory read and write
extern MemoryManager *swapmanager;	// tracks swap spaces
extern Lock *swapLock;

#include "processtable.h"
extern ProcessTable *processTable;		// keep track of process ids

#include "console.h"
extern Console *userConsole;
extern Semaphore *semReadAvail;
extern Semaphore *semWriteDone;

extern Lock *consoleLock;

#include "inversepagetable.h"
extern InversePageTable inversePageTable[NumPhysPages];

// swap space
extern char swapSpace[SwapPages * PageSize];

    // **************** My Code Ends **************** //

#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
