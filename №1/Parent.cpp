#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
	SYSTEMTIME st;		//Структура содержащая системное время	
	
	STARTUPINFO si;		//Структура определяющая свойства нового процесса
	PROCESS_INFORMATION pi;		//Структура содержащая информацию о созданном процессе
	
	ZeroMemory(&si, sizeof(si));  //Обнуление структуры si
	si.cb = sizeof(si);		//Инициализация поля cb структуры si размером структры
	ZeroMemory(&pi, sizeof(pi));	//Обнуление структуры pi

	if (!CreateProcess(L"D:\\Visual Studio\\Child\\Debug\\main.exe",	//Вызов дочернего процесса со вторым проектом
		NULL,				//Аргументы командной строки
		NULL,				
		NULL,				
		FALSE,			
		CREATE_NEW_CONSOLE, //Создание новой консоли у нового процесса
		NULL,				
		NULL,
		&si,
		&pi))
		cout << "CreateProcess failed";

	//Вывод времени
	GetLocalTime(&st);
	cout << "Date: " << st.wYear << "." << st.wMonth << "." << st.wDay << endl
		 << "Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl << endl;

	cout << "Descr: " << pi.hProcess << endl;		//Вывод дескриптора дочернего процесса
	cout << "ID: " << pi.dwProcessId << endl;		//Вывод id дочернего процесса


	WaitForSingleObject(pi.hProcess, INFINITE);		//Ожидание дочернего процесса

	CloseHandle(pi.hProcess);		//Закрытие handle процесса
	CloseHandle(pi.hThread);		//Закрытие handle потока

	return 0;
}