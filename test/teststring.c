
#include "syscall.h"
#include "copyright.h"

#define MaxLen 1024

int main()
{
    char s[MaxLen];
    ReadString(s, MaxLen);
    PrintString(s);
    Halt();
}