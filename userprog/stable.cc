#include "stable.h"

// Constructor
STable::STable()
{	
	this->bm = new BitMap(MAX_SEMAPHORE);
	
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if(this->bm)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for(int i=0; i < MAX_SEMAPHORE; i++)
	{
		if(this->semTab[i])
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
	
}

//Kiểm tra và tạo semaphore
int STable::Create(char *name, int init)
{

	// kiểm tra có tồn tại semaphore hay chưa
	for(int i=0; i<MAX_SEMAPHORE; i++)
	{
		if(bm->Test(i))
		{
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				return -1;
			}
		}
		
	}
	// tìm vị trí trống trong bảng semTab
	int id = this->FindFreeSlot();
	
	// trả về -1 khi không có vị trí trống
	if(id < 0)
	{
		return -1;
	}

	// nạp semaphore vào semTab 
	this->semTab[id] = new Sem(name, init);
	return 0;
}


int STable::Wait(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiểm tra vị trí index có tồn tại semaphore hay không
		if(bm->Test(i))
		{
			// so sánh khi tồn tại
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// nếu tồn tại thì down semaphore
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore"); //không tồn tại
	return -1;
}


int STable::Signal(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiểm tra vị trí index có tồn tại semaphore hay không
		if(bm->Test(i))
		{
			// so sánh khi tồn tại
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// nếu tồn tại thì up semaphore
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore");//không tồn tại
	return -1;
}

//tìm vị trí trống
int STable::FindFreeSlot()
{
	return this->bm->Find();
}
