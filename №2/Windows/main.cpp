#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main()
{
	wchar_t path[]= L"D:\\Visual Studio\\Controller\\Debug\\Controller.exe";			//Путь ехе для нового процесса
	char a;																				//Символ-буфер
	int f = 1;																			//flag

	STARTUPINFO si;																		//Структура определяющая свойства нового процесса	
	ZeroMemory(&si, sizeof(si));														//Обнуление структуры si
	si.cb = sizeof(si);																	//Инициализация поля cb структуры si размером структры

	PROCESS_INFORMATION pi;																//Создание структуры PROCESS_INFORMATION для нового процесса
	ZeroMemory(&pi, sizeof(pi));														//Обнуление структуры pi												//Занесение указателя на структуру в вектор

	if (!CreateProcess(path,															//Создание нового процесса
		NULL,											
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&si,
		&pi)) {
		cout << "CreateProcess failed\n";
		return 0;
	}

	HANDLE add = CreateEvent(NULL, TRUE, FALSE, L"addProc");							//Событие добавления нового процесса
	HANDLE del = CreateEvent(NULL, TRUE, FALSE, L"delProc");							//Событие удаления процесса
	HANDLE exit = CreateEvent(NULL, TRUE, FALSE, L"exit");								//Событие завершения программы

	do {
		a = _getch();																	//Ввод управляющего символа

		switch (a)
		{
			//Добавить процесс
		case('+'): 
			SetEvent(add);
			break;

			//Удалить процесс
		case('-'):
			SetEvent(del);
			break;

			//Завершить выполнение
		case('q'):
			f = 0;																		//Флаг выхода из программы
			break;																		
		}

	} while (f);

	SetEvent(exit);

	CloseHandle(add);																	//Закрытие handle события
	CloseHandle(del);																	//Закрытие handle события
	CloseHandle(exit);																	//Закрытие handle события

	WaitForSingleObject(pi.hProcess, INFINITE);											//Ожидание завершения работы созданного процесса

	return 0;
}