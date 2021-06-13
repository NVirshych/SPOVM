#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;
CRITICAL_SECTION cs;																	//����������� ������

DWORD WINAPI print(LPVOID lpParam) {

	wchar_t name[10];																	//�������� ������� ��� ���������� ������
	wsprintf(name, L"%d", GetCurrentThreadId());										//�������� ������� = id ������

	HANDLE exit = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);								//�������� �������

	//������ ��� ������
	string str = "Thread �";
	int num = *(int*)lpParam;
	int size = str.size();

	while (!WaitForSingleObject(exit, 0) == WAIT_OBJECT_0) {							//�������� �� ��������� ������� � ���������� ������

		EnterCriticalSection(&cs);														//���� � ����������� ������

		//������������ ����� ������
		for (int i = 0; i < size; i++) {
			cout << str[i];
			Sleep(35);
		}
		cout << num << endl;

		Sleep(1000);
		LeaveCriticalSection(&cs);														//����� �� ����������� ������
		Sleep(1000);
	}

	return 0;
}

int main()
{
	SetConsoleOutputCP(1251);

	char a;																				//������-�����
	int f = 1;																			//flag
	int num = 0;																		//����� ��������
	wchar_t buf[10];

	InitializeCriticalSection(&cs);														//������������� ����������� ������

	DWORD lastThreadId;																	//Id ���������� ���������� ������
	HANDLE lastThread;																	//Handle ���������� ���������� ������
	HANDLE exit;																		//Handle ������� ��� ���������� ���������� ������
	vector<HANDLE> threads;																//������ Handle �������
	vector<HANDLE> events;																//������ Handle ��������

	do {
		a = _getch();																	//���� ������������ �������

		switch (a)
		{

		//�������� �������
		case('+'):

			//�������� ������ ������
			lastThread = CreateThread(NULL, 0, &print, &num, 0, &lastThreadId);		
			threads.push_back(lastThread);									

			//�������� ������ �������
			wsprintf(buf, L"%d", lastThreadId);
			exit = CreateEvent(NULL, TRUE, FALSE, buf);
			events.push_back(exit);

			num++;
			break;

		//������� �������
		case('-'):

			if (!threads.size()) break;
			SetEvent(events[events.size() - 1]);										//������ ��������
			CloseHandle(events[events.size() - 1]);
			events.pop_back();
			threads.pop_back();
			break;

		//��������� ����������
		case('q'):
			f = 0;																		//���� ������ �� ���������
			break;
		}

	} while (f);

	while (threads.size()) {
		SetEvent(events[events.size() - 1]);											//������ ��������
		CloseHandle(events[events.size() - 1]);
		WaitForSingleObject(threads[threads.size() - 1], INFINITE);						//�������� ���������� ������
		events.pop_back();
		threads.pop_back();
	}

	return 0;
}