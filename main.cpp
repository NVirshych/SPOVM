#include <iostream>
#include <windows.h>
#include <heapapi.h>
#include "controlBlock.h"

#define MEM_SIZE 80													//Объем пула памяти

void* memory;														//Пул памяти

void* Malloc(size_t);												//Выделение памяти
void* Realloc(void*, size_t);										//Изменение размеры выделенной памяти
void Free(void*);													//Освобождение выделенной памяти
void visualize();													//Визуализация памяти

int main(){

	memory = malloc(MEM_SIZE);										//Выделение памяти
	controlBlock start(MEM_SIZE - sizeof(controlBlock));			//Первый controlBlock
	*((controlBlock*)memory) = start;

	//Вывод визуализации
	visualize();
	std::cout << std::endl;

	//Выделение некорректного объема
	void* ptr = Malloc(80);

	//Вывод визуализации
	visualize();
	std::cout << std::endl;

	//Освобождение по nullptr
	Free(ptr);

	//Вывод визуализации
	visualize();
	std::cout << std::endl;

	//Выделение памяти
	void* ptr2 = Malloc(16);

	//Вывод визуализации
	visualize();
	std::cout << std::endl;

	//Перевыделение
	ptr2 = Realloc(ptr2, 24);
	
	//Вывод визуализации
	visualize();
	std::cout << std::endl;

	//Освобождение
	Free(ptr2);

	//Вывод визуализации
	visualize();

	free(memory);

	return 0;
}

void* Malloc(size_t size) {

	//Приведение размера к числу, кратному размеру controlBlock
	if (size % sizeof(controlBlock))
		size += sizeof(controlBlock) - size % sizeof(controlBlock);

	//Запрашиваемый размер больше максимально доступного
	if (size > MEM_SIZE - sizeof(controlBlock))
		return nullptr;

	controlBlock* tmp;
	tmp = (controlBlock*)memory;											//Начало памяти (первый controlBlock)

	//Поиск первого подходящего блока
	while (tmp->size < size || tmp->used == true){

		tmp += tmp->size + sizeof(controlBlock);							//Следующий блок
		if (tmp - memory >= MEM_SIZE)										//Подходящий блок не найден
			return nullptr;
	}

	//Блок подходит по размеру
	if (tmp->size == size) {
		tmp->used = true;													//Пометить блок как занятый
		return tmp + sizeof(controlBlock);									//Вернуть указатель на начало блока памяти
	}
	//Блок нужно разделить
	else {
		
		controlBlock newBlock(tmp->size - size - sizeof(controlBlock));		//controlBlock нового блока памяти
		tmp->size = size;													//Блок необходимого размера
		tmp->used = true;													//Пометить блок как занятый

		if(tmp + sizeof(controlBlock) + tmp->size - memory < MEM_SIZE)		//Новый controlBlock не выходит за пределы памяти
			*(tmp + sizeof(controlBlock) + tmp->size) = newBlock;

		return tmp + sizeof(controlBlock);									//Вернуть указатель на начало блока памяти
	}
}

void* Realloc(void* ptr, size_t size) {

	//Приведение размера к числу, кратному размеру controlBlock
	if (size % sizeof(controlBlock))
		size += sizeof(controlBlock) - size % sizeof(controlBlock);

	//Запрашиваемый размер больше максимально доступного
	if (size > MEM_SIZE - sizeof(controlBlock))
		return nullptr;

	controlBlock* old = (controlBlock*)ptr - sizeof(controlBlock);			//controlBlock старого участка памяти

	if (old->size == size)													//Запрашиваемый размер равен старому размеру
		return ptr;

	controlBlock* tmp;
	tmp = (controlBlock*)memory;											//Начало памяти (первый controlBlock)

	//Поиск первого подходящего блока
	while (tmp->size < size || tmp->used == true) {

		tmp += tmp->size + sizeof(controlBlock);							//Следующий блок
		if (tmp - memory >= MEM_SIZE)										//Подходящий блок не найден
			return nullptr;
	}

	Free(ptr);																//Освобождение старой памяти

	//Блок подходит по размеру
	if (tmp->size == size) {
		tmp->used = true;													//Пометить блок как занятый
		memcpy(tmp + sizeof(controlBlock), ptr, old->size);					//Копирование данных
		return tmp + sizeof(controlBlock);									//Вернуть указатель на начало блока памяти
	}
	//Блок нужно разделить
	else {

		controlBlock newBlock(tmp->size - size - sizeof(controlBlock));		//controlBlock нового блока памяти
		tmp->size = size;													//Блок необходимого размера
		tmp->used = true;													//Пометить блок как занятый
		memcpy(tmp + sizeof(controlBlock), ptr, old->size);					//Копирование данных
		if (tmp + sizeof(controlBlock) + tmp->size - memory < MEM_SIZE)	//Новый controlBlock не выходит за пределы памяти
			* (tmp + sizeof(controlBlock) + tmp->size) = newBlock;

		return tmp + sizeof(controlBlock);									//Вернуть указатель на начало блока памяти
	}	
}

void Free(void* ptr) {

	if(ptr)
		((controlBlock*)ptr - sizeof(controlBlock))->used = false;			//Пометить блок как свободный

}

void visualize() {

	controlBlock* tmp = (controlBlock*)memory;								//Начало памяти (первый controlBlock)
	char a;																	//Символ для вывода

	while (1) {
	
		if (tmp->used)														//Блок занят
			a = '1';
		else																//Блок свободен
			a = '0';

		for (int i = 1; i > 0; i--) {										//controlBlock

			std::cout << 'C';

		}

		for (int i = tmp->size/sizeof(controlBlock); i > 0; i--) {			//Блок памяти
		
			std::cout << a;
		
		}

		tmp += sizeof(controlBlock) + tmp->size;							//Следующий controlBlock
		if (tmp - memory >= MEM_SIZE)										//Конец памяти
			return;
	}
}