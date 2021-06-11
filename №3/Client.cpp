#include <Windows.h>
#include <conio.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	char buf[80];																		//Буфер для записи строки

	int len = strlen(argv[argc - 1]);
	wchar_t semaphoreName[4096] = { 0 };
	MultiByteToWideChar(0, 0, argv[argc - 1], len, semaphoreName, len);

	HANDLE hRead  = (HANDLE)atoi(argv[argc-2]);											//Handle чтения из канала
	HANDLE sem = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, semaphoreName);			//Handle семафора

	do {

		WaitForSingleObject(sem, INFINITE);												//Ожидание семафора из сервера

		if (!ReadFile(hRead, &buf, 80, NULL, NULL)) {									//Чтение сообщения из канала в буфер
			cout << "Failed to read" << endl;
			break;																		//Окончание цикла при ошибке чтения
		}

		cout << "New message : " << buf << endl;										//Вывод сообщения

		ReleaseSemaphore(sem, 1, NULL);													//Перевод семафора в сигнальное состояние

	} while (1);

	//Закрытие handl'ов
	CloseHandle(sem);
	CloseHandle(hRead);

	return 0;
}