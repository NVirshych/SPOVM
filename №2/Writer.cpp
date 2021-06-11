#include <Windows.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	int len = strlen(argv[argc - 1]);										//����� ������ ��� ������������� ������

	wchar_t eventName[4096] = { 0 };

	MultiByteToWideChar(0, 0, argv[argc - 1], len, eventName, len);

	HANDLE print = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);		//�������, ����������� ������ �����
	HANDLE complete = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"complete");		//�������, ���������� ����� ������

	while (1) {
	
		WaitForSingleObject(print, INFINITE);								//�������� ���������� �� �����

		//������������ ����� ������
		for (int i = 0; i < len; i++) {
			cout << argv[argc - 1][i];
			Sleep(35);
		}
		cout << endl;

		Sleep(1000);
						
		ResetEvent(print);													//������� �������, ������������ ����� � ������������ ���������
		SetEvent(complete);													//������� �������, ����� ������ � ���������� ���������
	}

	return 0;
}