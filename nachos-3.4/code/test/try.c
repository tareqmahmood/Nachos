#include "syscall.h"

int a[1024], i;

int
main()
{
	//Write("try..\n", 6, 0);
	for(i = 0; i < 10; i++)
	{
		//Write("run..\n", 6, 0);
		a[i] = a[i - 1] + i;
	}
    Exit(a[i - 1]);
    /* not reached */
}
