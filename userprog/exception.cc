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

#define MaxBuffer 255

char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

void StartProcess_2(int virtAddr)
{
    int temp;
    char* filename;
    filename = User2System(virtAddr, MaxFileLength + 1);
    
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }

    space = new AddrSpace(executable);    
    currentThread->space = space;

    delete executable;			// close file

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(FALSE);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

// tang program counter
void increasePC() {
    // Compute next pc
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
    // Advance program counters.
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
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
            DEBUG('a', "\n No valid translation found.");
            printf("\n \n No valid translation found.");
            interrupt->Halt();
            return;
        }
        case ReadOnlyException:
        {
            DEBUG('a', "\n Write attempted to page marked read-only.");
            printf("\n\n Write attempted to page marked read-only.");
            interrupt->Halt();
            return;
        }
        case BusErrorException:
        {
        DEBUG('a', "\n Translation resulted in an invalid physical address.");
        printf("\n\n Translation resulted in an invalid physical address.");
        interrupt->Halt();
        return;
        }
        case AddressErrorException:
        {
            DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space.");
            printf("\n\n Unaligned reference or one that was beyond the end of the address space.");
            interrupt->Halt();
            return;
        }
        case OverflowException:
        {
            DEBUG('a', "\n Integer overflow in add or sub.");
            printf("\n\n Integer overflow in add or sub.");
            interrupt->Halt();
            return;
        }
        case IllegalInstrException:
        {
            DEBUG('a', "\n Unimplemented or reserved instr.");
            printf("\n\n Unimplemented or reserved instr.");
            interrupt->Halt();
            return;
        }
        case NumExceptionTypes:
        {
            DEBUG('a', "\n Num Exception Types.");
            printf("\n\n Num Exception Types.");
            interrupt->Halt();
            return;
        }
        case SyscallException:
        {
            switch(type) {
                case SC_Halt:
                {
                    DEBUG('a', "\nShutdown, initiated by user program.");
                    printf("\n\nShutdown, initiated by user program.");
                    interrupt->Halt();
                    return;
                }
                case SC_ReadInt:
                {
                    char* buff = new char[MaxBuffer + 1];
                    int len = gSynchConsole->Read(buff, MaxBuffer); // doc vao chuoi user nhap,
                                                                    // tra ve so byte doc duoc
                    buff[len] = '\0';

                    // kiem tra so am
                    int sign = 1; // so duong
                    if(buff[0] == '-') {
                        buff[0] = '0';
                        sign = -1; // so am
                    }

                    int res = 0; // ket qua so nguyen
                    for(int i = 0; i < len; i++) {
                        if(buff[i] < '0' || buff[i] > '9') { // neu khong phai la so
                            DEBUG('a', "\nInvalid input.");
                            printf("\n Invalid input.");
                            delete buff;
                            machine->WriteRegister(2, 0); // tra ve 0
                            increasePC();
                            return;
                        }
                        // neu la so
                        // chuyen char thanh int -> gan vao ket qua
                        res = res * 10 + sign * (buff[i] - '0');
                    }

                    machine->WriteRegister(2, res); // tra so nguyen doc duoc
                    delete buff;
                    break;
                }
                case SC_PrintInt:
                {
                    int number = machine->ReadRegister(4); // so nguyen duoc truyen vao
                    // neu la so 0
                    if(number == 0) {
                        gSynchConsole->Write("0", 1); // xuat so 0
                        increasePC();
                        return;
                    }

                    char* buff = new char[MaxBuffer + 1]; // chuoi ki tu dung de in so nguyen
                    
                    bool isNeg = false; // bien kiem tra co phai la so am
                    if(number < 0) { // neu la so am
                        isNeg = true;
                        number *= -1; // chuyen thanh so duong
                    }

                    int len = 0; // do dai chuoi ki tu so
                    // dem so chu so
                    int temp = number;
                    while(temp > 0) {
                        len++;
                        temp /= 10;
                    }

                    for(int i = len - 1; i >= 0; i--) {
                        buff[i] = (char)((number % 10) + '0'); // chuyen int thanh char
                        number /= 10;
                    }
                    buff[len] = '\0';

                    if(isNeg) // neu la so am
                        gSynchConsole->Write("-", 1); // xuat them dau -
                    gSynchConsole->Write(buff, len + 1); // xuat ket qua ra man hinh console

                    delete buff;
                    break;
                }
                case SC_ReadChar:
                {
                    char *buff = new char [MaxBuffer + 1];
                    int nBytes = gSynchConsole->Read(buff, MaxBuffer); // tra ve so luong byte ma nguoi dung su dung 

                    // So ki tu nhap vao nhieu hon 1 - nhap chuoi
                    if(nBytes > 1) 
                    {
                        printf("\nInvalid - Number of characters is greater than 1");
                        DEBUG('a', "\nError: Number of characters is greater than 1");
                        machine->WriteRegister(2, 0);  // tra ve 0 
                    } 
                    else if(nBytes == 0) // kí tự rỗng - null
                    {
                        printf("\nInvalid - Don't have any character") ;
                        DEBUG('a',"\nError: Don't have any character");
                        machine->WriteRegister(2, 0);  // tra ve 0
                    }
                    else // ki tu hop le
                    {
                        char c = *buff; // ki tu doc duoc luu tai vi tri dau tien cua mang buff
                        machine->WriteRegister(2, c); // tra gia tri ki tu doc duoc vao thanh ghi 2
                    }
                    delete buff;
                    break;
                }
                case SC_PrintChar:
                {
                    char c = (char)machine->ReadRegister(4); // lay gia tri tai thanh ghi 4
                    gSynchConsole->Write(&c, 1); // in ra man hinh console gia tri cua c, kich thuoc 1Byte
                    break;
                }
                case SC_ReadString:
                {
                    int buffAddr = machine->ReadRegister(4); // dia chi cua *buffer
                    int maxLen = machine->ReadRegister(5); // do dai toi da cua chuoi
                    
                    char *buffer = new char[maxLen + 1];
                    int len = gSynchConsole->Read(buffer, maxLen);   // doc chuoi user nhap,
                                                                        // tra ve do dai chuoi doc duoc
                    buffer[len] = '\0';

                    System2User(buffAddr, len + 1, buffer);  // chuyen du lieu tu kernelspcae qua userspace
                    delete buffer;
                    break;
                }
                case SC_PrintString:
                {
                    int buffAddr = machine->ReadRegister(4);    // dia chi cua *buffer
                    char* buffer = User2System(buffAddr, MaxBuffer);    // chuyen du lieu tu userspace qua kernelspcae 

                    int len = strlen(buffer);

                    gSynchConsole->Write(buffer, len + 1); // xuat chuoi ra console
                    delete buffer;
                    break;
                }
                case SC_CreateFile:
                {
                    //tao file rong, tra ve 0 neu thanh cong, -1 neu co loi
                    int virtAddr;
                    char* filename;
                    
                    virtAddr = machine->ReadRegister(4); 
                    filename = User2System(virtAddr, MaxFileLength + 1); // chuyen tu user qua kernel

                    if (strlen(filename) == 0)
                    {
                        printf("\nFile name is not valid");
                        machine->WriteRegister(2, -1); 
                        delete[] filename;
                        break;
                    }

                    if (filename == NULL)  //khong doc duoc file
                    {
                        printf("\n Not enough memory in system");
                        machine->WriteRegister(2, -1); 
                        delete[] filename;
                        break;
                    }

                    if (!fileSystem->Create(filename, 0)) //tao file bang filesystem
                    {
                        printf("\nError create file '%s'", filename);// tao file khong thanh cong
                        machine->WriteRegister(2, -1);
                        delete[] filename;
                        break;
                    }

                    printf("\nCreate file '%s' success", filename);//tao file thanh cong
                    machine->WriteRegister(2, 0);
                    delete[] filename;
                    break;
                }

                case SC_Open:
                {
                    int bufAddr = machine->ReadRegister(4); 
                    int type = machine->ReadRegister(5);
                    char *buf;

                    // neu da mo 10 files
                    if (fileSystem->index > 10)
                    {
                        machine->WriteRegister(2, -1);
                        delete[] buf;
                        break;
                    }
                        
                    // Khi mo stdin hay stdout, khong tang slg files
                    buf = User2System(bufAddr, MaxFileLength + 1);
                    if (strcmp(buf, "stdin") == 0)
                    {
                        //printf("Stdin mode\n");
                        machine->WriteRegister(2, 0);
                        delete[] buf;
                        break;
                    }
                    if (strcmp(buf, "stdout") == 0)
                    {
                        printf("Stdout mode\n");
                        machine->WriteRegister(2, 1);
                        delete[] buf;
                        break;
                    }

                    // Khong mo duoc file
                    if ((fileSystem->openf[fileSystem->index] = fileSystem->Open(buf, type)) != NULL)
                    {

                        printf("\nOpen file success '%s'\n", buf);
                        machine->WriteRegister(2, fileSystem->index - 1);
                    }
                    else 
                    {
                        printf("Can not open file '%s'", buf);
                        machine->WriteRegister(2, -1);
                    }
                    delete[] buf;
                    break;

                }

                case SC_Close:
                {
                    int no = machine->ReadRegister(4);
                    int i = fileSystem->index;

                    // mo file thu i va muon dong file thu no.[no] (no > i) --> loi
                    if (i < no)
                    {
                        printf("Close file failed \n");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    fileSystem->openf[no] == NULL;
                    delete fileSystem->openf[no];
                    machine->WriteRegister(2, 0);
                    printf("Close file success\n");
                    break;
                }

                case SC_Read:
                {
                    int virtAddr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
                    int openf_id = machine->ReadRegister(6);
                    int i = fileSystem->index;
                    
                    if (openf_id > i || openf_id < 0 || openf_id == 1) // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                    {						 	// or try to read stdout
                        printf("Try to open invalib file");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    if (fileSystem->openf[openf_id] == NULL)
                    {
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    char *buf = User2System(virtAddr, charcount);
                    
                    if (openf_id == 0) // read from stdin
                    {
                        int sz = gSynchConsole->Read(buf, charcount);
                        System2User(virtAddr, sz, buf);
                        machine->WriteRegister(2, sz);

                        delete[] buf;
                        break;
                    }
                    
                    int before = fileSystem->openf[openf_id]->GetCurrentPos();
                    if ((fileSystem->openf[openf_id]->Read(buf, charcount)) > 0)
                    {
                        // chuyen du lieu tu kernel sang user
                        int after = fileSystem->openf[openf_id]->GetCurrentPos();
                        System2User(virtAddr, charcount, buf);
                        machine->WriteRegister(2, after - before + 1);	// after & before just used for returning
                    } else {
                        machine->WriteRegister(2, -1);
                    }
                    delete[] buf;
                    break;
                }

                case SC_Write:
                {
                    int virtAddr = machine->ReadRegister(4);
                    int charcount = machine->ReadRegister(5);
                    int openf_id = machine->ReadRegister(6);
                    int i = fileSystem->index;


                    if (openf_id > i || openf_id < 0 || openf_id == 0) //  Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                    {
                        machine->WriteRegister(2, -1);
                        break;
                    }
                    
                    if (fileSystem->openf[openf_id] == NULL)
                    {
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    // file chi doc	
                    if (fileSystem->openf[openf_id]->type == 1)
                    {
                        printf("Try to modify read-only file");
                        machine->WriteRegister(2, -1);
                        break;
                    }

                    // ghi ra console
                    char *buf = User2System(virtAddr, charcount);
                    if (openf_id == 1)
                    {
                        int i = 0;
                        while (buf[i] != '\0' && buf[i] != '\n')
                        {
                            gSynchConsole->Write(buf + i, 1);
                            i++;
                        }
                        buf[i] = '\n';
                        gSynchConsole->Write(buf + i, 1); // ki tu cuoi

                        machine->WriteRegister(2, i - 1);
                        delete[] buf;
                        break;
                    }


                    // ghi vao file
                    int before = fileSystem->openf[openf_id]->GetCurrentPos();
                    if ((fileSystem->openf[openf_id]->Write(buf, charcount)) != 0)
                    {
                        int after = fileSystem->openf[openf_id]->GetCurrentPos();
                        System2User(virtAddr, after - before, buf);
                        machine->WriteRegister(2, after - before + 1);
                        delete[] buf;
                        break;
                    }
                }

                case SC_Seek:
                {
                    // Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
                    int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
                    int id = machine->ReadRegister(5); // Lay id cua file
                    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                    if (id < 0 || id > 14)
                    {
                        printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
                        machine->WriteRegister(2, -1);
                        //IncreasePC();
                        return;
                    }
                    // Kiem tra file co ton tai khong
                    if (fileSystem->openf[id] == NULL)
                    {
                        printf("\nKhong the seek vi file nay khong ton tai.");
                        machine->WriteRegister(2, -1);
                        //IncreasePC();
                        return;
                    }
                    // Kiem tra co goi Seek tren console khong
                    if (id == 0 || id == 1)
                    {
                        printf("\nKhong the seek tren file console.");
                        machine->WriteRegister(2, -1);
                        //IncreasePC();
                        return;
                    }
                    // Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
                    pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
                    if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
                    {
                        printf("\nKhong the seek file den vi tri nay.");
                        machine->WriteRegister(2, -1);
                    }
                    else
                    {
                        // Neu hop le thi tra ve vi tri di chuyen thuc su trong file
                        fileSystem->openf[id]->Seek(pos);
                        machine->WriteRegister(2, pos);
                    }
                    //IncreasePC();
                    return;
                }
                case SC_Exec:
                {
                    //ham chay file thuc thi, tra ve -1 neu loi
                    int virtAddr;
                    virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
                    char* name;
                    name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel space

                    if (name == NULL)
                    {
                        DEBUG('a', "\n Not enough memory in System");
                        printf("\n Not enough memory in System");
                        machine->WriteRegister(2, -1);// tra ve -1
                        //IncreasePC();
                        return;
                    }
                    OpenFile *file = fileSystem->Open(name);
                    if (file == NULL)
                    {
                        printf("\nExec:: Can't open this file.");
                        machine->WriteRegister(2, -1); // tra ve -1
                        //IncreasePC();
                        return;
                    }

                    delete file;

                    // Return child process id
                    int pid = pTab->ExecUpdate(name);
                    machine->WriteRegister(2, pid);

                    delete[] name;
                    //IncreasePC();
                    return;
                }


                default:
                {
                    printf("\n Unexpected user mode exception (%d %d)", which, type);
                    interrupt->Halt(); 
                    break;
                }
            }
        }
        increasePC(); // tang program counter
    }
}
