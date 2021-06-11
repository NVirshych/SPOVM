#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main()
{
	string str = "D:\\Visual Studio\\Client\\Debug\\Client.exe ";								//Путь доченего процесса														
	char buf[64];																			//Буфер
	wchar_t wbuf[4096] = L"Synch";															//Название семафора

	HANDLE sem;																					
	sem = CreateSemaphore(NULL, 0, 1, wbuf);												//Создание семафора
	if (!sem) {
		cout << "Failed to create Semaphore!" << endl;
		return 0;
	}

	HANDLE hWrite, hRead, copy;
	if (!CreatePipe(&hRead, &hWrite, NULL, 0)) {											//Создание анонимного канала
		cout << "Failed to create pipe!" << endl;
		return 0;
	}

	if(!DuplicateHandle(GetCurrentProcess(), hRead, GetCurrentProcess(),					//Создание наследуемого
		&copy, NULL, TRUE, DUPLICATE_SAME_ACCESS)) {										//handle для чтения из канала
		cout << "Failed to duplicate handle!" << endl;																
		return 0;
	}
	CloseHandle(hRead);

	sprintf(buf, "%d %q", copy, "Synch");
	str += buf;

	ZeroMemory(wbuf, 4096 * sizeof(wchar_t));
	MultiByteToWideChar(0, 0, str.c_str(), str.length(), wbuf, str.length());

	STARTUPINFO si;																			//Структура определяющая свойства нового процесса	
	ZeroMemory(&si, sizeof(STARTUPINFO));													//Обнуление структуры si
	si.cb = sizeof(STARTUPINFO);															//Инициализация поля cb структуры si размером структры

	PROCESS_INFORMATION client;																//Создание структуры PROCESS_INFORMATION для нового процесса
	ZeroMemory(&client, sizeof(PROCESS_INFORMATION));										//Обнуление структуры pi

	if (!CreateProcess(NULL,																//Создание нового процесса
		wbuf,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&client)) {
		cout << "CreateProcess failed\n";
		return 0;
	}

	str.clear();

	do {

		//Ввод строки для передачи
		cout << "Input message or press 'Enter' to exit" << endl;
		getline(cin, str);

		//Закрытие handle записи и выход из цикла при вводе пустой строки
		if (!str.length()) {
			if (!WriteFile(hWrite, "", 80, NULL, NULL))
				cout << "Failed to write" << endl;
			CloseHandle(hWrite);
			ReleaseSemaphore(sem, 1, NULL);
			break;
		}

		//Запись строки в канал
		if (!WriteFile(hWrite, str.c_str(), 80, NULL, NULL))
			cout << "Failed to write" << endl;

		ReleaseSemaphore(sem, 1, NULL);														//Перевод семафора в сигнальное состояние
		WaitForSingleObject(sem, INFINITE);													//Ожидание перефода семафора в клиенте

	} while (1);

	WaitForSingleObject(client.hProcess, INFINITE);											//Ожидание завершения работы клиента

	//Закрытие handl'ов
	CloseHandle(sem);
	CloseHandle(copy);
	CloseHandle(client.hProcess);
	CloseHandle(client.hThread);

	return 0;
}