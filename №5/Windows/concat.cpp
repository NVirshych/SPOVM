#include "pch.h"
#include "concat.h"

using namespace std;

DWORD WINAPI reader(LPVOID lpParam) {

	HANDLE hWrite = *(HANDLE*)lpParam;													//Дескриптор для записи в канал

	HANDLE hFind;																		//Дескриптор поиска файла
	HANDLE hFile;																		//Дескриптор файла
	WIN32_FIND_DATA FindFileData;

	int read;																			//Количество прочитанных символов
	const int bufSize = 100;															//Размер буфера
	char buf[bufSize];																	//Буфер
	char separator = '\n';																//Разделитель между файлами


	//Создание сигнала об окончании работы
	HANDLE finished;
	finished = CreateEvent(NULL, TRUE, FALSE, "finished");
	if (!finished) {
		cout << "Failed to create finished Event!" << endl;
		return 0;
	}

	//Создание синхронизируюшего сигнала
	HANDLE readyToRead;
	readyToRead = CreateEvent(NULL, TRUE, FALSE, "readyToRead");
	if (!readyToRead) {
		cout << "Failed to create readyToRead Event!" << endl;
		return 0;
	}

	//Overlapped для чтения из файла
	HANDLE fileReader;
	fileReader = CreateEvent(NULL, TRUE, FALSE, "fileReader");
	if (!fileReader) {
		cout << "Failed to create fileReader Event!" << endl;
		return 0;
	}

	OVERLAPPED file = { 0 };
	file.hEvent = fileReader;

	//Overlapped для записи в канал
	HANDLE pipeWriter;
	pipeWriter = CreateEvent(NULL, TRUE, FALSE, "pipeWriter");
	if (!pipeWriter) {
		cout << "Failed to create pipeWriter Event!" << endl;
		return 0;
	}

	OVERLAPPED pipe = { 0 };
	pipe.hEvent = pipeWriter;

	char searchDir[MAX_PATH];															//Путь директории, содержащей исходные файлы
	GetModuleFileName(NULL, searchDir, MAX_PATH);										//Получение пути запущенного exe

	int i = MAX_PATH - 1;
	int j = 0;

	//Преобразование пути exe к строке поиска txt файлов в данной директории
	while (i) {

		if (searchDir[i - 1] == '\\') {

			if (j) {
				char searchAttr[] = "*.txt";
				for (j = 0; j < 5; j++)
					searchDir[i + j] = searchAttr[j];

				searchDir[i + 5] = 0;
				break;
			}
			else j++;
		}
		i--;
	}

	Sleep(100);

	//Открытие синхро сигнала
	HANDLE readyToWrite = OpenEvent(EVENT_ALL_ACCESS, FALSE, "readyToWrite");
	if (!readyToWrite) {
		cout << "Failed to open readyToWrite Event!" << endl;
		return 0;
	}

	if ((hFind = FindFirstFile(searchDir, &FindFileData)) != INVALID_HANDLE_VALUE) {	//Поиск txt файлов в зад. директории

		do {

			if (!strcmp(FindFileData.cFileName, "output.txt")) continue;				//Пропуск файла для вывода

			//Открытие файла
			hFile = CreateFile(FindFileData.cFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (!hFile) {
				cout << "Failed to open a file!" << endl;
				return 0;
			}

			file.Offset = 0;															//Установка нулевого сдвига в OVERLAPPED файла

			do {

				ReadFile(hFile, &buf, bufSize, NULL, &file);							//Чтение сообщения из файла в буфер
				WaitForSingleObject(fileReader, INFINITE);								//Ожидание окончания чтения

				read = file.InternalHigh;												//Кол-во прочитанных символов
				file.Offset += read;													//Сдвиг отступа в файле

				WaitForSingleObject(readyToWrite, INFINITE);							//Ожидание разрешения на запись в канал
				ResetEvent(readyToWrite);

				if (!read) {

					WriteFile(hWrite, &separator, 1, NULL, &pipe);						//Запись в канал
					WaitForSingleObject(pipeWriter, INFINITE);							//Ожидание конца записи в канала

					SetEvent(readyToRead);
					break;
				}

				WriteFile(hWrite, buf, read, NULL, &pipe);								//Запись в канал
				WaitForSingleObject(pipeWriter, INFINITE);								//Ожидание конца записи в канала

				SetEvent(readyToRead);

			} while (1);

			CloseHandle(hFile);

		} while (FindNextFile(hFind, &FindFileData));									//Следующий файл

		FindClose(hFind);
	}
	else {
	
		cout << "No txt files found in exe directory." << endl;
	
	}

	WaitForSingleObject(readyToWrite, INFINITE);										//Ожидание завершения чтения из канала 

	SetEvent(finished);
	SetEvent(readyToRead);

	//Закрытие handl'ов
	CloseHandle(hWrite);
	CloseHandle(finished);
	CloseHandle(readyToRead);
	CloseHandle(fileReader);
	CloseHandle(pipeWriter);
	CloseHandle(readyToWrite);

	return 0;
}

DWORD WINAPI writer(LPVOID lpParam) {

	HANDLE hRead = *(HANDLE*)lpParam;													//Дескириптор чтения из канала

	int read;																			//Кол-во прочитанных символоы
	const int bufSize = 100;															//Размер буфера
	char buf[bufSize];																	//Буфер для записи строки																	

	//Создание синхронизирующего сигнала
	HANDLE readyToWrite;
	readyToWrite = CreateEvent(NULL, TRUE, FALSE, "readyToWrite");
	if (!readyToWrite) {
		cout << "Failed to create Event!" << endl;
		return 0;
	}

	//Overlapped для записи в файл
	HANDLE fileWriter;
	fileWriter = CreateEvent(NULL, TRUE, TRUE, "fileWriter");
	if (!fileWriter) {
		cout << "Failed to create Event!" << endl;
		return 0;
	}

	OVERLAPPED file = { 0 };
	file.hEvent = fileWriter;

	//Overlapped для чтения из канала
	HANDLE pipeReader;
	pipeReader = CreateEvent(NULL, TRUE, TRUE, "pipeReader");
	if (!pipeReader) {
		cout << "Failed to create Event!" << endl;
		return 0;
	}

	OVERLAPPED pipe = { 0 };
	pipe.hEvent = pipeReader;

	//Открытие файла для вывода результата
	HANDLE hFile;
	hFile = CreateFile("output.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	if (!hFile) {
		cout << "Failed to open a file!" << endl;
		return 0;
	}

	Sleep(100);

	//Открытие синхронизирующего сигнала
	HANDLE readyToRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, "readyToRead");
	if (!readyToRead) {
		cout << "Failed to open readyToRead Event!" << endl;
		return 0;
	}

	//Открытие сигнала об окончании работы
	HANDLE finished = OpenEvent(SYNCHRONIZE, FALSE, "finished");
	if (!finished) {
		cout << "Failed to open finished Event!" << endl;
		return 0;
	}

	do {

		SetEvent(readyToWrite);															//Разрешение записи в канал

		WaitForSingleObject(readyToRead, INFINITE);										//Ожидание окончания записи в канал

			if (WaitForSingleObject(finished, 0) == WAIT_OBJECT_0) break;				//Проверка сигнала об окончании работы

		ResetEvent(readyToRead);
		
		ReadFile(hRead, &buf, bufSize, NULL, &pipe);									//Чтение сообщения из канала в буфер

		WaitForSingleObject(pipeReader, INFINITE);										//Ожидание конца чтения из канала

		read = pipe.InternalHigh;														//Кол-во прочитанных символов

		WriteFile(hFile, buf, read, NULL, &file);										//Запись в файл

		WaitForSingleObject(fileWriter, INFINITE);										//Ожидание конца записи в файл

		file.Offset += read;

	} while (1);

	//Закрытие handl'ов
	CloseHandle(hFile);
	CloseHandle(hRead);
	CloseHandle(readyToWrite);
	CloseHandle(fileWriter);
	CloseHandle(pipeReader);
	CloseHandle(readyToRead);
	CloseHandle(finished);

	return 0;
}

//Внешняя функция
extern "C" __declspec(dllexport) void concat(void) {

	//Создание анонимного канала
	HANDLE hWrite, hRead;
	if (!CreatePipe(&hRead, &hWrite, NULL, 0)) {
		cout << "Failed to create pipe!" << endl;
		return;
	}

	//Создание потока-писателя
	HANDLE hWriter;
	hWriter = CreateThread(NULL, 0, &writer, &hRead, 0, NULL);

	if (!hWriter) {
		cout << "Failed to create writer thread" << endl;
		return;
	}

	//Создание потока-читателя
	HANDLE hReader;
	hReader = CreateThread(NULL, 0, &reader, &hWrite, 0, NULL);

	if (!hReader) {
		cout << "Failed to create reader thread" << endl;
		TerminateThread(hWriter, 0);
		return;
	}


	WaitForSingleObject(hWriter, INFINITE);												//Ожидание окочания работы потока-писателя

	CloseHandle(hWrite);
	CloseHandle(hRead);
	CloseHandle(hReader);
	CloseHandle(hWriter);

	return;
}
