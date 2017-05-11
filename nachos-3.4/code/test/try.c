#include "syscall.h"

int
main()
{
	//Write("try..\n", 6, 0);
	char word[10];
	Read(word, 5, 0);
	Write("You entered :\n", 14, 0);
	Write(word, 5, 0);
	Write("\n", 1, 0);
	//Exec("../test/sort");
    Exit(0);
    /* not reached */
}
