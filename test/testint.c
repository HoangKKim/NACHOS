#include "syscall.h"
int main() {
    int n = ReadInt();
    PrintInt(n);
    Halt();
}