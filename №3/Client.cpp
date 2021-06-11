#include <Windows.h>
#include <conio.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	char buf[80];																		//����� ��� ������ ������

	int len = strlen(argv[argc - 1]);
	wchar_t semaphoreName[4096] = { 0 };
	MultiByteToWideChar(0, 0, argv[argc - 1], len, semaphoreName, len);

	HANDLE hRead  = (HANDLE)atoi(argv[argc-2]);											//Handle ������ �� ������
	HANDLE sem = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, semaphoreName);			//Handle ��������

	do {

		WaitForSingleObject(sem, INFINITE);												//�������� �������� �� �������

		if (!ReadFile(hRead, &buf, 80, NULL, NULL)) {									//������ ��������� �� ������ � �����
			cout << "Failed to read" << endl;
			break;																		//��������� ����� ��� ������ ������
		}

		cout << "New message : " << buf << endl;										//����� ���������

		ReleaseSemaphore(sem, 1, NULL);													//������� �������� � ���������� ���������

	} while (1);

	//�������� handl'��
	CloseHandle(sem);
	CloseHandle(hRead);

	return 0;
}