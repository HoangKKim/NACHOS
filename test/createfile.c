
#include "syscall.h"
#include "copyright.h"

#define maxlen 32

int main()
{
    // int len;
    // char filename[maxlen+1];

    // // create file
    // if(Create("text.txt")==-1)
    // {
    //     // printf("Error create file");
    // } else
    // {
    //     // printf("Success create file");
    // }
    Create("text.txt");
    
    Halt();
}