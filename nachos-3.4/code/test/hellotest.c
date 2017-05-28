#include "syscall.h"

int a[200], i;

int main()
{
	Exec("../test/try");
	for(i = 0; i < 100; i++)
	{
		//Write("run..\n", 6, 0);
		a[i] = a[i - 1] + i;
	}
    Exit(a[i - 1]);
}