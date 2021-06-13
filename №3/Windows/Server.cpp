#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main()
{
	string str = "D:\\Visual Studio\\Client\\Debug\\Client.exe ";								//���� �������� ��������														
	char buf[64];																			//�����
	wchar_t wbuf[4096] = L"Synch";															//�������� ��������

	HANDLE sem;																					
	sem = CreateSemaphore(NULL, 0, 1, wbuf);												//�������� ��������
	if (!sem) {
		cout << "Failed to create Semaphore!" << endl;
		return 0;
	}

	HANDLE hWrite, hRead, copy;
	if (!CreatePipe(&hRead, &hWrite, NULL, 0)) {											//�������� ���������� ������
		cout << "Failed to create pipe!" << endl;
		return 0;
	}

	if(!DuplicateHandle(GetCurrentProcess(), hRead, GetCurrentProcess(),					//�������� ������������
		&copy, NULL, TRUE, DUPLICATE_SAME_ACCESS)) {										//handle ��� ������ �� ������
		cout << "Failed to duplicate handle!" << endl;																
		return 0;
	}
	CloseHandle(hRead);

	sprintf(buf, "%d %q", copy, "Synch");
	str += buf;

	ZeroMemory(wbuf, 4096 * sizeof(wchar_t));
	MultiByteToWideChar(0, 0, str.c_str(), str.length(), wbuf, str.length());

	STARTUPINFO si;																			//��������� ������������ �������� ������ ��������	
	ZeroMemory(&si, sizeof(STARTUPINFO));													//��������� ��������� si
	si.cb = sizeof(STARTUPINFO);															//������������� ���� cb ��������� si �������� ��������

	PROCESS_INFORMATION client;																//�������� ��������� PROCESS_INFORMATION ��� ������ ��������
	ZeroMemory(&client, sizeof(PROCESS_INFORMATION));										//��������� ��������� pi

	if (!CreateProcess(NULL,																//�������� ������ ��������
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

		//���� ������ ��� ��������
		cout << "Input message or press 'Enter' to exit" << endl;
		getline(cin, str);

		//�������� handle ������ � ����� �� ����� ��� ����� ������ ������
		if (!str.length()) {
			if (!WriteFile(hWrite, "", 80, NULL, NULL))
				cout << "Failed to write" << endl;
			CloseHandle(hWrite);
			ReleaseSemaphore(sem, 1, NULL);
			break;
		}

		//������ ������ � �����
		if (!WriteFile(hWrite, str.c_str(), 80, NULL, NULL))
			cout << "Failed to write" << endl;

		ReleaseSemaphore(sem, 1, NULL);														//������� �������� � ���������� ���������
		WaitForSingleObject(sem, INFINITE);													//�������� �������� �������� � �������

	} while (1);

	WaitForSingleObject(client.hProcess, INFINITE);											//�������� ���������� ������ �������

	//�������� handl'��
	CloseHandle(sem);
	CloseHandle(copy);
	CloseHandle(client.hProcess);
	CloseHandle(client.hThread);

	return 0;
}