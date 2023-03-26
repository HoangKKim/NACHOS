#include "syscall.h"
#include "copyright.h"

int main()
{
    char c = ReadChar();
    PrintChar(c);
    Halt();
    // return 0;
}