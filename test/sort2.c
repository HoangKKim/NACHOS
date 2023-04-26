// file test bubble sort
#include "syscall.h"
#include "copyright.h"
#define MAXSIZE 100

int main()
{
    int arr[MAXSIZE];
    int n, i, j, temp;

    // nhap so luong phan tu
    PrintString("Enter number of elements: ");
    n = ReadInt();
    if(n > MAXSIZE) {
        PrintString("Max size is 100\n");
        Halt();
    }
    if(n < 0) {
        PrintString("Invalid\n");
        Halt();
    }

    // nhap mang
    PrintString("Enter each value:\n");
    for(i = 0; i < n; i++) {
        arr[i] = ReadInt();
    }

    // bubble sort
    for(i = 0; i < n - 1; i++) {
        for(j = 0; j < n - i - 1; j++) {
            if(arr[j] > arr[j + 1]) {
                // swap
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    // xuat mang
    PrintString("After sort: ");
    for(i = 0; i < n; i++) {
        PrintInt(arr[i]);
        PrintChar(' ');
    }

    Halt();
}