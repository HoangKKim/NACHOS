// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2 -- đọc tại thanh ghi 2
//		arg1 -- r4 -- tham số thứ nhất sẽ bắt đầu lưu tại thanh ghi 4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever! -- phải tăng program counter lên
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

#define MaxFileLength 32
#define MaxBuffer 256 

void increasePC() {
    // Compute next pc
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    // Advance program counters.
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}


char* User2System(int virtAddr,int limit)
{
    int i;// index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit +1];//need for terminal string
    if (kernelBuf == NULL)
    return kernelBuf;
    memset(kernelBuf,0,limit+1);
    //printf("\n Filename u2s:");
    for (i = 0 ; i < limit ;i++)
    {
        machine->ReadMem(virtAddr+i,1,&oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
        break;
    }
return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{   
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0 ;
    do{
        oneChar= (int) buffer[i];
        machine->WriteMem(virtAddr+i,1,oneChar);
        i ++;
    }while(i < len && oneChar != 0);
    return i;
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2); // đọc tại thanh ghi 2

    switch(which) {
        case NoException:
            return;
        case PageFaultException:
        {
            DEBUG('a', "\n No valid translation found. \n");
            printf("\n \n No valid translation found.\n");
            interrupt->Halt();
            // break;
            return;
        }
        case ReadOnlyException:
        {
            DEBUG('a', "\n Write attempted to page marked read-only.\n");
            printf("\n\n Write attempted to page marked read-only.\n");
            interrupt->Halt();
            return;
        }
        case BusErrorException:
        {
        DEBUG('a', "\n Translation resulted in an invalid physical address.\n");
        printf("\n\n Translation resulted in an invalid physical address.\n");
        interrupt->Halt();
        return;
    }
        case AddressErrorException:
        {
            DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space.\n");
            printf("\n\n Unaligned reference or one that was beyond the end of the address space.\n");
            interrupt->Halt();
            return;
        }
        case OverflowException:
        {
            DEBUG('a', "\n Integer overflow in add or sub.\n");
            printf("\n\n Integer overflow in add or sub.\n");
            interrupt->Halt();
            return;
        }
        case IllegalInstrException:
        {
            DEBUG('a', "\n Unimplemented or reserved instr.\n");
            printf("\n\n Unimplemented or reserved instr.\n");
            interrupt->Halt();
            return;
        }
        case NumExceptionTypes:
        {
            DEBUG('a', "\n Num Exception Types.\n");
            printf("\n\n Num Exception Types.\n");
            interrupt->Halt();
            return;
        }
        case SyscallException:
        {
            switch(type) {
                case SC_Halt:
                    {   
                        DEBUG('a', "\n Shutdown, initiated by user program.");
                        printf ("\n\n Shutdown, initiated by user program.");
                        interrupt->Halt();
                        break;
                    }
                case SC_Create:
                {
                    int virtAddr;
                    char *filename;
                    DEBUG('a',"\n SC_Create call ...");
                    DEBUG('a', "\n Reading virtual addres of filename");
                    // lấy tham số(tên tập tin) từ thanh ghi r4
                    virtAddr = machine->ReadRegister(4);
                    DEBUG('a',"\n Reading filename.");
                    // MaxFileLength = 32
                    // increasePC();
                    filename = User2System(virtAddr,MaxFileLength + 1);
                    if(filename == NULL)
                    {
                        printf("\n Not enough memory in system");
                        DEBUG('a',"\n Not enough memory in system");
                        machine->WriteRegister(2,-1); // trả về lỗi cho chương trình người dùng
                        delete filename;
                        increasePC();
                        return;
                    }
                    DEBUG('a',"\n Finish reading filename.");
                    // DEBUG('a',"\n File name: "<<filename<<"");
                    
                    // Create file with size = 0
                    if(!fileSystem->Create(filename,0))
                    {
                        printf("\n Error create file '%s'", filename);
                        machine->WriteRegister(2,-1);
                        delete filename;
                        increasePC();
                        return;
                    }
                    machine->WriteRegister(2,0); // trả về cho chương trình người dùng thành công
                    delete filename;
                    // increasePC();
                    return;
                }                
                case SC_ReadInt:
                {
                    char* buff = new char[MaxBuffer + 1];
                    int len = gSynchConsole->Read(buff, MaxBuffer);
                    buff[len] = '\0';
                    // kiem tra so am
                    bool isNeg = false;
                    if(buff[0] == '-') {
                        buff[0] = '0';
                        isNeg = true;
                    }
                    // kiem tra co phai la so
                    for(int i = 0; i < len; i++) {
                        if(buff[i] < '0' || buff[i] > '9') { // neu khong phai la so
                            machine->WriteRegister(2, 0); // tra ve 0
                            printf("\n Invalid input");
                            DEBUG('a', "\n Invalid input");
                            delete buff;
                            increasePC();
                            return;
                        }
                    }
                    // chuyen ki tu thanh so
                    int res = 0;
                    for(int i = 0; i < len; i++)
                        res = res * 10 + (buff[i] - '0');

                    // neu la so am, * -1
                    if(isNeg)
                        res *= -1;

                    machine->WriteRegister(2, res); // tra so nguyen doc duoc
                    delete buff;
                    increasePC();
                    return;
                }
                case SC_PrintInt:
                {
                    int number = machine->ReadRegister(4);
                    if(number == 0) {
                        gSynchConsole->Write("0", 1);
                        increasePC();
                        return;
                    }
        
                    bool isNeg = false;
                    int firstIndex = 0;
                    if(number < 0) {
                        isNeg = true;
                        firstIndex = 1;
                        number = number * (-1);
                    }
        
                    int temp = number;
                    int count = 0;
                    while(temp) {
                        count++;
                        temp /= 10;
                    }
        
                    char* buff = new char[MaxBuffer];
                    for(int i = firstIndex + count - 1; i >= firstIndex; i--)
                    {
                        buff[i] = (char)((number % 10) + '0');
                        number /= 10;
                    }
                    if(isNeg == true)
                    {
                        buff[0] = '-';
                        buff[count + 1] = '\0';
                        gSynchConsole->Write(buff, count + 1);
                    }
                    else
                    {
                        buff[count] = '\0';
                        gSynchConsole->Write(buff, count);
                    }
                    machine->WriteRegister(2, 0); // in so nguyen thanh cong
        
                    delete buff;
                    increasePC();
                    return;
                }
                case SC_ReadChar:
                {
                    int maxByte = 255;
                    char *buffer = new char [MaxBuffer+1];
                    int numByte = gSynchConsole->Read(buffer, maxByte);                    
                    if(numByte>1) // nhiều hơn một kí tự
                    {
                        printf("\nInvalid - Number of character is greater than 1");
                        DEBUG('a', "\nError: Number of character is greater than 1");
                        machine->WriteRegister(2,0);
                    } else
                    if(numByte==0) // kí tự rỗng - null
                    {
                        printf("\nInvalid - It is a null character") ;
                        DEBUG('a',"\nError: It is a null character");
                        machine->WriteRegister(2,0);
                    } else
                    // kí tự hợp lệ
                    {
                        // printf("\nSuccess");
                        char c = buffer[0];
                        machine->WriteRegister(2,c); // ghi giá trị biến c tại thanh ghi 2

                    }
                    delete buffer;
                    // return;
                    // increasePC();
                    break;
                }
                case SC_PrintChar:
                {
                    char c = (char)machine->ReadRegister(4);
                    gSynchConsole->Write(&c,1);
                    break;
                }               
                default:
                {
                    printf("\n Unexpected user mode exception (%d %d)", which, type);
                    interrupt->Halt(); 
                    return; 
                }
            }
        }
        increasePC();
    }
}
