// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6 
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

#define REG_V0 2
#define REG_R2 2
#define REG_A0 4
#define REG_A1 5

#define MAX_FILEPATH_LENGTH 50

void SafeRead();

void pcUp()
{
        int pc = machine->ReadRegister(PCReg);
        machine->WriteRegister(PrevPCReg, pc);
        pc = machine->ReadRegister(NextPCReg);
        machine->WriteRegister(PCReg, pc);
        pc += 4;
        machine->WriteRegister(NextPCReg, pc);
}

void ThreadFunction(int arg)
{
    if(arg == 0)
    {
        currentThread->RestoreUserState();
    }
    else if(arg == 1)
    {
        if(currentThread->space != NULL)
        {
            currentThread->space->InitRegisters();
            currentThread->space->RestoreState();
        }
    }

    // currentThread->space->InitRegisters();
    // currentThread->space->RestoreState();
    
    machine->Run();
    ASSERT(FALSE);
}

void Syscall_Exit();
void Syscall_Exec();
void Syscall_Read();
void Syscall_Write();
void PageFaultExceptionHandler();


void
ExceptionHandler(ExceptionType which)
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    int type = machine->ReadRegister(REG_R2);

    if ((which == SyscallException) && (type == SC_Halt)) 
    {
		DEBUG('a', "Shutdown, initiated by user program.\n");
	   	interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exec))
    {
        DEBUG('a', "Exec, initiated by user program.\n");
        Syscall_Exec();
    }
    else if((which == SyscallException) && (type == SC_Exit))
    {
        DEBUG('a', "Exit, initiated by user program.\n");
        Syscall_Exit();
    }
    else if((which == SyscallException) && (type == SC_Read))
    {
        DEBUG('a', "Read, initiated by user program.\n");
        Syscall_Read();
    }
    else if((which == SyscallException) && (type == SC_Write))
    {
        DEBUG('a', "Write, initiated by user program.\n");
        Syscall_Write();
    }
    else if(which == PageFaultException)
    {
        PageFaultExceptionHandler();
    }
    else if(which == ReadOnlyException)
    {
        printf("Read only exception\n");
        ASSERT(FALSE);
    }
    else if(which == BusErrorException)
    {
        printf("Bus error exception\n");
        ASSERT(FALSE);
    }
    else if(which == AddressErrorException)
    {
        printf("AddressErrorException\n");
        ASSERT(FALSE);
    }
    else if(which == OverflowException)
    {
        printf("OverflowException\n");
        ASSERT(FALSE);
    }
    else if(which == IllegalInstrException)
    {
        printf("Illegal instruction exception\n");
        ASSERT(FALSE);
    }
    else 
    {
		printf("Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
    }

    (void) interrupt->SetLevel(oldLevel);
}


void Syscall_Exit()
{
    int status = machine->ReadRegister(REG_A0);

    printf(">> Exiting %s. with value %d\n", currentThread->getName(), status);

    // if(!strcmp(currentThread->getName(), "main"))
    // {
    //     printf("main thread\n");
    //     currentThread->Yield();
    //     return;
    // }
    
    int PID = currentThread->PID;

    // freeing page table
    AddrSpace* space = currentThread->space;
    if(space != 0) delete space;

    // freeing process entry
    processTable->Free(PID);

    // if there are no more processes, Halt()
    if(processTable->getProcessCount() == 0)
    {
        //printf(">> Last thread %s.\n", currentThread->getName());
        interrupt->Halt();
        return;
    }

    //printf(">> Exited...\n");

    // otherwise, just stop this thread
    currentThread->Finish();
    machine->WriteRegister(2, status);

    pcUp();
}


void Syscall_Exec()
{
    // address in main memory where path is stored

    currentThread->space->savePageTable();

    int addr = machine->ReadRegister(REG_A0);

    // copying path to name[]
    char path[MAX_FILEPATH_LENGTH + 1];
    int i = 0, len = 0;
    
    while(1)
    {
        machine->ReadMem(addr + i, 1, (int *)&path[i]);
        if(path[i] == '\0') break;

        i++;

        // when file path is too long
        if(i > MAX_FILEPATH_LENGTH)
        {
            printf("Length of file name exceeds limit!");
            ASSERT(FALSE);
        }
    }
    currentThread->space->restorePageTable();

    if(path[0] != '.') path[0] = '.';
    
    len = i;

    printf("executable file %s\n", path);

    OpenFile* exeFile = fileSystem->Open(path);

    if(exeFile == NULL)
    {
        // unsuccessful file open
        printf("Cannot open executable file!\n");
        machine->WriteRegister(REG_V0, -1);
    }
    else
    {
        // successfully opened file
        AddrSpace* addrspace = new AddrSpace(exeFile);

        // extract filename from path, like extract "try" from "../test/try"
        int lastSlash = 0;
        for(i = 0; i < len ; i++) if(path[i] == '/') lastSlash = i + 1;
        char newName[50];
        strcpy(newName, path + lastSlash);
        printf("Executing %s...\n", newName);

        Thread* newThread =  new Thread("child");

        // allocate in process table
        newThread->PID = processTable->Alloc(newThread);
        newThread->space = addrspace;

        // start new process
        newThread->Fork(ThreadFunction, 1);

        // return PID to Kernel
        machine->WriteRegister(2, newThread->PID);
    }
    pcUp();
}



void Syscall_Read()
{
    consoleLock->Acquire();
    int addr = machine->ReadRegister(REG_A0);
    int size = machine->ReadRegister(REG_A1);

    //printf("Read %d %d\n", addr, size);

    int i = 0;
    char ch;
    currentThread->space->savePageTable();
    for(i = 0; i < size; i++)
    {
        semReadAvail->P();
        ch = userConsole->GetChar();
        if(ch == '\n') break;
        machine->WriteMem(addr + i, 1, ch);
    }
    machine->WriteMem(addr + i, 1, '\0');
    currentThread->space->restorePageTable();
    machine->WriteRegister(2, i);
    pcUp();
    consoleLock->Release();
}


void Syscall_Write()
{
    consoleLock->Acquire();
    int addr = machine->ReadRegister(REG_A0);
    int size = machine->ReadRegister(REG_A1);

    // addr = currentThread->space->pageTable[addr].physicalPage;

    //printf("Write %d %d\n", addr, size);

    int i = 0, value;
    currentThread->space->savePageTable();
    for(i = 0; i < size; i++)
    {
        machine->ReadMem(addr + i, 1, &value);
        userConsole->PutChar((char)value);
        semWriteDone->P();
    }
    currentThread->space->restorePageTable();
    machine->WriteRegister(2, i);
    pcUp();
    consoleLock->Release();
}


void PageFaultExceptionHandler()
{
    stats->numPageFaults++;

    int virtualAddr = machine->ReadRegister(BadVAddrReg);
    int vpn = virtualAddr / PageSize;

    DEBUG('p', "PageFaultException VA = %d\n", virtualAddr);

    int physicalPage = memorymanager->AllocPage(currentThread->PID, vpn);
    
    if(physicalPage == -1)
    {
        physicalPage = memorymanager->AllocByForce(currentThread->PID, vpn);
        //currentThread->space->evictPage(physicalPage);
        DEBUG('p', "By Force Allocation of PP = %d for VP = %d\n", physicalPage, vpn);
        //printf("Ran out of memory\n");
        //ASSERT(FALSE);
    }

    currentThread->space->loadIntoFreePage(vpn, physicalPage);


    DEBUG('p', "successfulAllocation of PP = %d for VP = %d\n\n", physicalPage, vpn);
}