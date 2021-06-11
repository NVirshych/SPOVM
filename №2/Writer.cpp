#include <Windows.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	int len = strlen(argv[argc - 1]);										//Длина строки для посимвольного вывода

	wchar_t eventName[4096] = { 0 };

	MultiByteToWideChar(0, 0, argv[argc - 1], len, eventName, len);

	HANDLE print = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);		//Событие, разрешающее начать вывод
	HANDLE complete = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"complete");		//Событие, означающее конец вывода

	while (1) {
	
		WaitForSingleObject(print, INFINITE);								//Ожидание разрешения на вывод

		//Посимвольный вывод строки
		for (int i = 0; i < len; i++) {
			cout << argv[argc - 1][i];
			Sleep(35);
		}
		cout << endl;

		Sleep(1000);
						
		ResetEvent(print);													//Перевод событая, разрешающего вывод в несигнальное состояние
		SetEvent(complete);													//Перевод событая, конца вывода в сигнальное состояние
	}

	return 0;
}