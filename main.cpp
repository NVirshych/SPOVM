#include <iostream>
#include <windows.h>
#include <heapapi.h>
#include "controlBlock.h"

#define MEM_SIZE 80													//����� ���� ������

void* memory;														//��� ������

void* Malloc(size_t);												//��������� ������
void* Realloc(void*, size_t);										//��������� ������� ���������� ������
void Free(void*);													//������������ ���������� ������
void visualize();													//������������ ������

int main(){

	memory = malloc(MEM_SIZE);										//��������� ������
	controlBlock start(MEM_SIZE - sizeof(controlBlock));			//������ controlBlock
	*((controlBlock*)memory) = start;

	//����� ������������
	visualize();
	std::cout << std::endl;

	//��������� ������������� ������
	void* ptr = Malloc(80);

	//����� ������������
	visualize();
	std::cout << std::endl;

	//������������ �� nullptr
	Free(ptr);

	//����� ������������
	visualize();
	std::cout << std::endl;

	//��������� ������
	void* ptr2 = Malloc(16);

	//����� ������������
	visualize();
	std::cout << std::endl;

	//�������������
	ptr2 = Realloc(ptr2, 24);
	
	//����� ������������
	visualize();
	std::cout << std::endl;

	//������������
	Free(ptr2);

	//����� ������������
	visualize();

	free(memory);

	return 0;
}

void* Malloc(size_t size) {

	//���������� ������� � �����, �������� ������� controlBlock
	if (size % sizeof(controlBlock))
		size += sizeof(controlBlock) - size % sizeof(controlBlock);

	//������������� ������ ������ ����������� ����������
	if (size > MEM_SIZE - sizeof(controlBlock))
		return nullptr;

	controlBlock* tmp;
	tmp = (controlBlock*)memory;											//������ ������ (������ controlBlock)

	//����� ������� ����������� �����
	while (tmp->size < size || tmp->used == true){

		tmp += tmp->size + sizeof(controlBlock);							//��������� ����
		if (tmp - memory >= MEM_SIZE)										//���������� ���� �� ������
			return nullptr;
	}

	//���� �������� �� �������
	if (tmp->size == size) {
		tmp->used = true;													//�������� ���� ��� �������
		return tmp + sizeof(controlBlock);									//������� ��������� �� ������ ����� ������
	}
	//���� ����� ���������
	else {
		
		controlBlock newBlock(tmp->size - size - sizeof(controlBlock));		//controlBlock ������ ����� ������
		tmp->size = size;													//���� ������������ �������
		tmp->used = true;													//�������� ���� ��� �������

		if(tmp + sizeof(controlBlock) + tmp->size - memory < MEM_SIZE)		//����� controlBlock �� ������� �� ������� ������
			*(tmp + sizeof(controlBlock) + tmp->size) = newBlock;

		return tmp + sizeof(controlBlock);									//������� ��������� �� ������ ����� ������
	}
}

void* Realloc(void* ptr, size_t size) {

	//���������� ������� � �����, �������� ������� controlBlock
	if (size % sizeof(controlBlock))
		size += sizeof(controlBlock) - size % sizeof(controlBlock);

	//������������� ������ ������ ����������� ����������
	if (size > MEM_SIZE - sizeof(controlBlock))
		return nullptr;

	controlBlock* old = (controlBlock*)ptr - sizeof(controlBlock);			//controlBlock ������� ������� ������

	if (old->size == size)													//������������� ������ ����� ������� �������
		return ptr;

	controlBlock* tmp;
	tmp = (controlBlock*)memory;											//������ ������ (������ controlBlock)

	//����� ������� ����������� �����
	while (tmp->size < size || tmp->used == true) {

		tmp += tmp->size + sizeof(controlBlock);							//��������� ����
		if (tmp - memory >= MEM_SIZE)										//���������� ���� �� ������
			return nullptr;
	}

	Free(ptr);																//������������ ������ ������

	//���� �������� �� �������
	if (tmp->size == size) {
		tmp->used = true;													//�������� ���� ��� �������
		memcpy(tmp + sizeof(controlBlock), ptr, old->size);					//����������� ������
		return tmp + sizeof(controlBlock);									//������� ��������� �� ������ ����� ������
	}
	//���� ����� ���������
	else {

		controlBlock newBlock(tmp->size - size - sizeof(controlBlock));		//controlBlock ������ ����� ������
		tmp->size = size;													//���� ������������ �������
		tmp->used = true;													//�������� ���� ��� �������
		memcpy(tmp + sizeof(controlBlock), ptr, old->size);					//����������� ������
		if (tmp + sizeof(controlBlock) + tmp->size - memory < MEM_SIZE)	//����� controlBlock �� ������� �� ������� ������
			* (tmp + sizeof(controlBlock) + tmp->size) = newBlock;

		return tmp + sizeof(controlBlock);									//������� ��������� �� ������ ����� ������
	}	
}

void Free(void* ptr) {

	if(ptr)
		((controlBlock*)ptr - sizeof(controlBlock))->used = false;			//�������� ���� ��� ���������

}

void visualize() {

	controlBlock* tmp = (controlBlock*)memory;								//������ ������ (������ controlBlock)
	char a;																	//������ ��� ������

	while (1) {
	
		if (tmp->used)														//���� �����
			a = '1';
		else																//���� ��������
			a = '0';

		for (int i = 1; i > 0; i--) {										//controlBlock

			std::cout << 'C';

		}

		for (int i = tmp->size/sizeof(controlBlock); i > 0; i--) {			//���� ������
		
			std::cout << a;
		
		}

		tmp += sizeof(controlBlock) + tmp->size;							//��������� controlBlock
		if (tmp - memory >= MEM_SIZE)										//����� ������
			return;
	}
}