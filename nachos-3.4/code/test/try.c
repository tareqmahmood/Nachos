#include "syscall.h"

int a[1024], i;

int
main()
{
	//Write("try..\n", 6, 0);
	for(i = 0; i < 1024; i++)
	{
		a[i] = i;
	}
    Exit(0);
    /* not reached */
}
