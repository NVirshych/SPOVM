#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main()
{
	wchar_t path[]= L"D:\\Visual Studio\\Controller\\Debug\\Controller.exe";			//���� ��� ��� ������ ��������
	char a;																				//������-�����
	int f = 1;																			//flag

	STARTUPINFO si;																		//��������� ������������ �������� ������ ��������	
	ZeroMemory(&si, sizeof(si));														//��������� ��������� si
	si.cb = sizeof(si);																	//������������� ���� cb ��������� si �������� ��������

	PROCESS_INFORMATION pi;																//�������� ��������� PROCESS_INFORMATION ��� ������ ��������
	ZeroMemory(&pi, sizeof(pi));														//��������� ��������� pi												//��������� ��������� �� ��������� � ������

	if (!CreateProcess(path,															//�������� ������ ��������
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

	HANDLE add = CreateEvent(NULL, TRUE, FALSE, L"addProc");							//������� ���������� ������ ��������
	HANDLE del = CreateEvent(NULL, TRUE, FALSE, L"delProc");							//������� �������� ��������
	HANDLE exit = CreateEvent(NULL, TRUE, FALSE, L"exit");								//������� ���������� ���������

	do {
		a = _getch();																	//���� ������������ �������

		switch (a)
		{
			//�������� �������
		case('+'): 
			SetEvent(add);
			break;

			//������� �������
		case('-'):
			SetEvent(del);
			break;

			//��������� ����������
		case('q'):
			f = 0;																		//���� ������ �� ���������
			break;																		
		}

	} while (f);

	SetEvent(exit);

	CloseHandle(add);																	//�������� handle �������
	CloseHandle(del);																	//�������� handle �������
	CloseHandle(exit);																	//�������� handle �������

	WaitForSingleObject(pi.hProcess, INFINITE);											//�������� ���������� ������ ���������� ��������

	return 0;
}