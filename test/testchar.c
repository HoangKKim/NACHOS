#include "syscall.h"
#include "copyright.h"

int main()
{
    char c;
    c = ReadChar(); // đọc ký tự từ màn hình console
    PrintChar(c); // in ký tự vừa đọc được
    Halt();
}