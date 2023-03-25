#include "syscall.h"
#include "copyright.h"

int main()
{
    int i;
    char c;
    char space = ' ';
    char line = '\n';
    for(i=32; i<127; i++)
    {
        c= (char)(i);
        PrintChar(c);
        PrintChar(space);
        if(i>0 && i%10==0)
        {
            PrintChar(line);
        }
    }
    Halt();
}