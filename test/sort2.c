// file test bubble sort
#include "syscall.h"
#include "copyright.h"
#define MAXSIZE 100

int main()
{
    int arr[MAXSIZE];
    int n, i, j, temp;

    PrintString("Nhap so so nguyen: ");
    n = ReadInt();
    if(n > MAXSIZE) {
        PrintString("Kich thuoc toi da la 100\n");
        Halt();
    }

    // nhap mang
    PrintString("Nhap gia tri cac phan tu:\n");
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
    PrintString("Day da sap xep: ");
    for(i = 0; i < n; i++) {
        PrintInt(arr[i]);
        PrintChar(' ');
    }

    Halt();
}