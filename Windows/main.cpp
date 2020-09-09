#include <windows.h>
#include <iostream>

using namespace std;

int main() {

	HMODULE dll;

	void (*concat)(void);												//Указатель на функцию

	dll = LoadLibrary("concatFiles.dll");								//Загрузка динамической библиотеки
	if (!dll) {

		cout << "Failed to load DLL" << endl;
		return GetLastError();
	}

	concat = (void (*)(void))GetProcAddress(dll, "concat");				//Получение адреса функции
	if (!concat){

		cout << "Failed to get DLL function" << endl;
		return GetLastError();
	}

	concat();															//Вызов функции конкатенации файлов	

	//Выгрузка библиотеки
	if (!FreeLibrary(dll)){		

		cout << "Failed to free DLL" << endl;
		return GetLastError();
	}

	return 0;
}