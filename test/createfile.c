#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int stdin;
	char fileName[MAX_LENGTH];
	PrintString("\n\t\t\tTao File\n\n");
	PrintString(" - Nhap ten file: ");
	
	stdin = Open("stdin", 2); // Goi ham Open mo file nhap vao ten file
	if (stdin != -1)
	{
		int len = Read(fileName, MAX_LENGTH, stdin); // Goi ham Read doc ten file vua nhap
		
		if(len <= 1)
			PrintString(" -> Ten file khong hop le!!!\n\n");
		else
		{
			if (CreateFile(fileName) == 0) // Goi ham CreateFile
			{
				PrintString(" -> Create file thanh cong.\n\n");
			}
		}
		Close(stdin); // Goi ham Close de dong stdin
	}
    return 0;
}