#include "syscall.h"
#include "copyright.h"

int main()
{
    for(int i=0; i<255; i++)
    PrintChar(i);
    Halt();
    // return 0;
}