// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    this->executable = executable;
    unsigned int i, size, j;
    int successfulAllocation = 1;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);



// first, set up the translation 
    
    pageTable = new TranslationEntry[numPages];

    for (i = 0; i < numPages; i++) 
    {
    	pageTable[i].virtualPage = i;

        pageTable[i].physicalPage = -1;

    	// pageTable[i].physicalPage = memorymanager->AllocPage();        // allock free page
     //    if(pageTable[i].physicalPage == -1)                     // check whether ran out of memory
     //    {
     //        successfulAllocation = 0;
     //        break;
     //    }

    	pageTable[i].valid = FALSE;
    	pageTable[i].use = FALSE;
    	pageTable[i].dirty = FALSE;
    	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
    					// a separate page, we could set its 
    					// pages to be read-only
    }

    if(successfulAllocation == 0)       // not all page is allocated, free the allocated pages and exit
    {
        for(j = 0; j < i; j++)
        {
            memorymanager->FreePage(pageTable[j].physicalPage);
        }
        printf("Unsuccesfull Memory Allocation. Not enough memory for %dth page.\n", i);
        ASSERT(FALSE);
    }
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    // bzero(machine->mainMemory, size);


// then, copy in the code and data segments into memory
   //  if (noffH.code.size > 0) {
   //      DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			// noffH.code.virtualAddr, noffH.code.size);
   //      executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			// noffH.code.size, noffH.code.inFileAddr);
   //  }
   //  if (noffH.initData.size > 0) {
   //      DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			// noffH.initData.virtualAddr, noffH.initData.size);
   //      executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			// noffH.initData.size, noffH.initData.inFileAddr);
   //  }





    /* **** NACHOS 3 ********


    memoryLock->Acquire();


    // zero out the pages allocated
    for(i = 0; i < numPages; i++)
    {
        bzero(&machine->mainMemory[pageTable[i].physicalPage * PageSize], PageSize);
    }


    // ReadAt writes data from disk to RAM
    // ReadAt(where to write on RAM, size of data, from where to copy data)

    unsigned int k = 0;     // keeps track of 

    // copying code segment
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",  noffH.code.virtualAddr, noffH.code.size);
        unsigned int codePage = divRoundUp(noffH.code.size, PageSize);
        for(i = 0; i < codePage; i++)
        {
            executable->ReadAt(&(machine->mainMemory[pageTable[k].physicalPage * PageSize]), 
                PageSize, noffH.code.inFileAddr + i * PageSize);
            k++;
        }
    }


    // copying data segment
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", noffH.initData.virtualAddr, noffH.initData.size);
        unsigned int dataPage = divRoundUp(noffH.initData.size, PageSize);
        for(i = 0; i < dataPage; i++)
        {
            executable->ReadAt(&(machine->mainMemory[pageTable[k].physicalPage * PageSize]), 
                PageSize, noffH.initData.inFileAddr +  i * PageSize);
            k++;
        }
    }


    memoryLock->Release();

    */

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    unsigned int i = 0;
    for(i = 0; i < numPages; i++)
    {
        if(pageTable[i].valid == TRUE)
            memorymanager->FreePage(pageTable[i].physicalPage);
    }
    delete pageTable;
    delete executable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void AddrSpace::loadIntoFreePage(int vpn, int physicalPage)
{
    memoryLock->Acquire();

    pageTable[vpn].physicalPage = physicalPage;
    pageTable[vpn].valid = TRUE;

    int virtualAddr = vpn * PageSize;               // starting vaddr of this page
    int physicalAddr = physicalPage * PageSize;

    // inside code segment
    if(noffH.code.size > 0 
        && virtualAddr >= noffH.code.virtualAddr 
        && virtualAddr < (noffH.code.virtualAddr + noffH.code.size))
    {
        // find codeoffset
        int codeoffset = (virtualAddr - noffH.code.virtualAddr);

        // for(int i = 0; i < PageSize; i++)
        // {
        //     executable->ReadAt(&(machine->mainMemory[physicalPage * PageSize + i]), 
        //         1, noffH.code.inFileAddr + codeoffset + i);
        // }


        int codesize = noffH.code.size - codeoffset;
        int size = min(codesize, PageSize);
        executable->ReadAt(&(machine->mainMemory[physicalAddr]), 
                size, noffH.code.inFileAddr + codeoffset);

        // overlap with initialized data segment
        if(size < PageSize)
        {
            
            int datasize = PageSize - size;
            datasize = min(datasize, noffH.initData.size);
            if(datasize > 0)
                executable->ReadAt(&(machine->mainMemory[physicalAddr + size]), 
                    datasize, noffH.initData.inFileAddr);
        }
    }


    // inside data segment
    else if(noffH.initData.size > 0 
        && virtualAddr >= noffH.initData.virtualAddr 
        && virtualAddr < (noffH.initData.virtualAddr + noffH.initData.size))
    {
        // find dataoffset
        int dataoffset = (virtualAddr - noffH.initData.virtualAddr);

        // for(int i = 0; i < PageSize; i++)
        // {
        //     executable->ReadAt(&(machine->mainMemory[physicalPage * PageSize + i]), 
        //         1, noffH.initData.inFileAddr + dataoffset + i);
        // }


        int datasize = noffH.initData.size - dataoffset;
        int size = min(datasize, PageSize);
        executable->ReadAt(&(machine->mainMemory[physicalAddr]), 
                size, noffH.initData.inFileAddr + dataoffset);

        // overlap with uninitialized data segment
        if(size < PageSize)
        {
            int udatasize = PageSize - size;
            udatasize = min(udatasize, noffH.uninitData.size);
            if(udatasize > 0)
                bzero(&(machine->mainMemory[physicalAddr + size]), udatasize);
        }
    }


    // inside uninitialised data segment
    else if(noffH.uninitData.size > 0 
        && virtualAddr >= noffH.uninitData.virtualAddr 
        && virtualAddr < (noffH.uninitData.virtualAddr + noffH.uninitData.size))
    {
        int size = min(noffH.uninitData.size + noffH.uninitData.virtualAddr - virtualAddr, PageSize);
        bzero(&(machine->mainMemory[physicalAddr]), size);
    }

    // zero'd out memory
    else
    {
        bzero(&(machine->mainMemory[physicalAddr]), PageSize);
    }


    memoryLock->Release();
}
