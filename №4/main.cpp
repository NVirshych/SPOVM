#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;
CRITICAL_SECTION cs;																	//Критическая секция

DWORD WINAPI print(LPVOID lpParam) {

	wchar_t name[10];																	//Название сигнала для завершения работы
	wsprintf(name, L"%d", GetCurrentThreadId());										//Название сигнала = id потока

	HANDLE exit = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);								//Открытие сигнала

	//Строка для вывода
	string str = "Thread №";
	int num = *(int*)lpParam;
	int size = str.size();

	while (!WaitForSingleObject(exit, 0) == WAIT_OBJECT_0) {							//Проверка на получение сигнала о завершении работы

		EnterCriticalSection(&cs);														//Вход в критическую секцию

		//Посимвольный вывод строки
		for (int i = 0; i < size; i++) {
			cout << str[i];
			Sleep(35);
		}
		cout << num << endl;

		Sleep(1000);
		LeaveCriticalSection(&cs);														//Выход из критический секции
		Sleep(1000);
	}

	return 0;
}

int main()
{
	SetConsoleOutputCP(1251);

	char a;																				//Символ-буфер
	int f = 1;																			//flag
	int num = 0;																		//Номер процесса
	wchar_t buf[10];

	InitializeCriticalSection(&cs);														//Инициализация критической секции

	DWORD lastThreadId;																	//Id последнего созданного потока
	HANDLE lastThread;																	//Handle последнего созданного потока
	HANDLE exit;																		//Handle сигнала для последнего созданного потока
	vector<HANDLE> threads;																//Вектор Handle потоков
	vector<HANDLE> events;																//Вектор Handle сигналов

	do {
		a = _getch();																	//Ввод управляющего символа

		switch (a)
		{

		//Добавить процесс
		case('+'):

			//Создание нового потока
			lastThread = CreateThread(NULL, 0, &print, &num, 0, &lastThreadId);		
			threads.push_back(lastThread);									

			//Создание нового сигнала
			wsprintf(buf, L"%d", lastThreadId);
			exit = CreateEvent(NULL, TRUE, FALSE, buf);
			events.push_back(exit);

			num++;
			break;

		//Удалить процесс
		case('-'):

			if (!threads.size()) break;
			SetEvent(events[events.size() - 1]);										//Сигнал процессу
			CloseHandle(events[events.size() - 1]);
			events.pop_back();
			threads.pop_back();
			break;

		//Завершить выполнение
		case('q'):
			f = 0;																		//Флаг выхода из программы
			break;
		}

	} while (f);

	while (threads.size()) {
		SetEvent(events[events.size() - 1]);											//Сигнал процессу
		CloseHandle(events[events.size() - 1]);
		WaitForSingleObject(threads[threads.size() - 1], INFINITE);						//Ожидание завершения потока
		events.pop_back();
		threads.pop_back();
	}

	return 0;
}