#include "syscall.h"

int a[1024], i;

int
main()
{
	for(i = 0; i < 10; i++)
	{
		a[i] = a[i - 1] + i;
	}
    Exit(a[i - 1]);
    /* not reached */
}
