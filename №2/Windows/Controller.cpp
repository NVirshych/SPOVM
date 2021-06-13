#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main(){

	wchar_t buf[4096];

	string line = "D:\\Visual Studio\\Writer\\Debug\\Writer.exe ";							//Аргументы командной строки
	string num = "I_love_C";																//Строка, выводимая в дочернем процессе
	int i = 0;
	

	STARTUPINFO si;																			//Структура определяющая свойства нового процесса	
	ZeroMemory(&si, sizeof(si));															//Обнуление структуры si
	si.cb = sizeof(si);																		//Инициализация поля cb структуры si размером структры

	vector<PROCESS_INFORMATION*> proc_vector;												//Вектор указателей на PROCESS_INFORMATION созданных процессов
	vector<HANDLE> proc_print;																//Вектор указателей на HANDLE созданных событий

	HANDLE add = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"addProc");							//Событие добавления процесса
	HANDLE del = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"delProc");							//Событие удаления процесса
	HANDLE exit = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"exit");								//Событие завершения работы
	HANDLE complete = CreateEvent(NULL, TRUE, TRUE, L"complete");							//Событие завершения посимвольного вывода процессом

	do {

		//Завершение работы
		if (WaitForSingleObject(exit, 0) == WAIT_OBJECT_0)
			break;

		//Добавление процесса
		if (WaitForSingleObject(add, 0) == WAIT_OBJECT_0) {	
			PROCESS_INFORMATION* pi = new PROCESS_INFORMATION;								//Создание структуры PROCESS_INFORMATION для нового процесса
			ZeroMemory(pi, sizeof(PROCESS_INFORMATION));									//Обнуление структуры
			proc_vector.push_back(pi);														//Занесение указателя на структуру в вектор

			string arg = line + num;

			ZeroMemory(buf, 4096 * sizeof(wchar_t));
			MultiByteToWideChar(0, 0, arg.c_str(), arg.length(), buf, arg.length());

			if (!CreateProcess(NULL,														//Создание нового процесса
				buf,																		//Аргументы командной строки
				NULL,
				NULL,
				FALSE,
				NULL,
				NULL,
				NULL,
				&si,
				proc_vector[proc_vector.size() - 1]))
				cout << "CreateProcess failed\n";

			ZeroMemory(buf, 4096 * sizeof(wchar_t));
			MultiByteToWideChar(0, 0, num.c_str(), num.length(), buf, num.length());

			HANDLE print = CreateEvent(NULL, TRUE, FALSE, buf);								//Создание события для синхронизации созданного процесса
			proc_print.push_back(print);													//Занесение HANDLE события в вектор
			num += "++";																	//Изменение выводимой следующим процессом строки	

			ResetEvent(add);																//Перевод события add в несигнальное состояние
		}

		//Удаление процесса
		if (WaitForSingleObject(del, 0) == WAIT_OBJECT_0 && proc_print.size()) {
			if (WaitForSingleObject(proc_print[proc_print.size() - 1], 0) == WAIT_OBJECT_0)
				WaitForSingleObject(complete, INFINITE);
			TerminateProcess(proc_vector[proc_vector.size() - 1]->hProcess, 1);				//Принудительное завершение дочернего процесса
			delete proc_vector[proc_vector.size() - 1];										//Освобождение выделенной памяти
			proc_vector.pop_back();															//Удаление элемента вектора
			
			CloseHandle(proc_print[proc_print.size() - 1]);									//Закрытие handle события
			proc_print.pop_back();															//Удаление handle события из вектора
		}
		ResetEvent(del);																	//Перевод события del в несигнальное состояние


		//Подача сигнала для начала вывода в дочернем процессе
		if (WaitForSingleObject(complete, 0) == WAIT_OBJECT_0 && proc_print.size()) {
			ResetEvent(complete);															//Перевод события complete в несигнальное состояние

			i++;
			if (i >= proc_print.size()) i = 0;

			SetEvent(proc_print[i]);														//Перевод события, разрешающего вывод в дочернем процессе
																							//в сигнальное состояние
		}
	
	} while (1);

	//Последовательное завершение и удаление данных всех оставшихся процессов
	while (proc_vector.size()) {
		TerminateProcess(proc_vector[proc_vector.size() - 1]->hProcess, 1);					//Принудительное завершение дочернего процесса
		delete proc_vector[proc_vector.size() - 1];											//Освобождение выделенной памяти
		proc_vector.pop_back();																//Удаление элемента вектора

		CloseHandle(proc_print[proc_print.size() - 1]);										//Закрытие handle события
		proc_print.pop_back();																//Удаление handle события из вектора
	}

	CloseHandle(add);																		//Закрытие handle события add
	CloseHandle(del);																		//Закрытие handle события del
	CloseHandle(exit);																		//Закрытие handle события exit

	return 0;
}