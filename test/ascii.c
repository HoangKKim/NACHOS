#include "syscall.h"
#include "copyright.h"

int main()
{
    int i;
    char c;
    char space = ' ';
    char line = '\n';
    for(i = 32; i < 127; i++)
    {
        PrintInt(i); // in so decimal
        PrintChar('.');
        PrintChar(space);
        c = (char)(i);
        PrintChar(c); // in ki tu
        PrintChar('\t');
        if(i % 10 == 0)
        {
            PrintChar(line);
        }
    }
    Halt();
}