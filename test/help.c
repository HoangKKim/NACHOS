#include "syscall.h"

int main()
{
	// team in4
	PrintString("\tTEAM INFORMATION\n");
	PrintString("21127012 - Tran Huy Ban\n");
	PrintString("21127090 - Vo Nguyen Hoang kim\n");
	PrintString("21127419 - Ngo Phuoc Tai\n");
	PrintString("21127478 - Tran Thi Thanh Van\n");

	//Instruction
	PrintString("\n\tINSTRUCTIONS\n");
	PrintString("ASCII Program:\n");
	PrintString("Print the ASCII table\n");
	PrintString("Run .userprog/nachos -rs 1023 -x ./test/ascii\n");

	PrintString("Bublle sort Program:\n");
	PrintString("Enter number of elements than enter number for each element\n");
	PrintString("Run .userprog/nachos -rs 1023 -x ./test/sort2\n");

	Halt();
}