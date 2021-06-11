#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
	SYSTEMTIME st;		//��������� ���������� ��������� �����	
	
	STARTUPINFO si;		//��������� ������������ �������� ������ ��������
	PROCESS_INFORMATION pi;		//��������� ���������� ���������� � ��������� ��������
	
	ZeroMemory(&si, sizeof(si));  //��������� ��������� si
	si.cb = sizeof(si);		//������������� ���� cb ��������� si �������� ��������
	ZeroMemory(&pi, sizeof(pi));	//��������� ��������� pi

	if (!CreateProcess(L"D:\\Visual Studio\\Child\\Debug\\main.exe",	//����� ��������� �������� �� ������ ��������
		NULL,				//��������� ��������� ������
		NULL,				
		NULL,				
		FALSE,			
		CREATE_NEW_CONSOLE, //�������� ����� ������� � ������ ��������
		NULL,				
		NULL,
		&si,
		&pi))
		cout << "CreateProcess failed";

	//����� �������
	GetLocalTime(&st);
	cout << "Date: " << st.wYear << "." << st.wMonth << "." << st.wDay << endl
		 << "Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl << endl;

	cout << "Descr: " << pi.hProcess << endl;		//����� ����������� ��������� ��������
	cout << "ID: " << pi.dwProcessId << endl;		//����� id ��������� ��������


	WaitForSingleObject(pi.hProcess, INFINITE);		//�������� ��������� ��������

	CloseHandle(pi.hProcess);		//�������� handle ��������
	CloseHandle(pi.hThread);		//�������� handle ������

	return 0;
}