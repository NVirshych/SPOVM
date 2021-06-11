#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;

int main(){

	wchar_t buf[4096];

	string line = "D:\\Visual Studio\\Writer\\Debug\\Writer.exe ";							//��������� ��������� ������
	string num = "I_love_C";																//������, ��������� � �������� ��������
	int i = 0;
	

	STARTUPINFO si;																			//��������� ������������ �������� ������ ��������	
	ZeroMemory(&si, sizeof(si));															//��������� ��������� si
	si.cb = sizeof(si);																		//������������� ���� cb ��������� si �������� ��������

	vector<PROCESS_INFORMATION*> proc_vector;												//������ ���������� �� PROCESS_INFORMATION ��������� ���������
	vector<HANDLE> proc_print;																//������ ���������� �� HANDLE ��������� �������

	HANDLE add = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"addProc");							//������� ���������� ��������
	HANDLE del = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"delProc");							//������� �������� ��������
	HANDLE exit = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"exit");								//������� ���������� ������
	HANDLE complete = CreateEvent(NULL, TRUE, TRUE, L"complete");							//������� ���������� ������������� ������ ���������

	do {

		//���������� ������
		if (WaitForSingleObject(exit, 0) == WAIT_OBJECT_0)
			break;

		//���������� ��������
		if (WaitForSingleObject(add, 0) == WAIT_OBJECT_0) {	
			PROCESS_INFORMATION* pi = new PROCESS_INFORMATION;								//�������� ��������� PROCESS_INFORMATION ��� ������ ��������
			ZeroMemory(pi, sizeof(PROCESS_INFORMATION));									//��������� ���������
			proc_vector.push_back(pi);														//��������� ��������� �� ��������� � ������

			string arg = line + num;

			ZeroMemory(buf, 4096 * sizeof(wchar_t));
			MultiByteToWideChar(0, 0, arg.c_str(), arg.length(), buf, arg.length());

			if (!CreateProcess(NULL,														//�������� ������ ��������
				buf,																		//��������� ��������� ������
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

			HANDLE print = CreateEvent(NULL, TRUE, FALSE, buf);								//�������� ������� ��� ������������� ���������� ��������
			proc_print.push_back(print);													//��������� HANDLE ������� � ������
			num += "++";																	//��������� ��������� ��������� ��������� ������	

			ResetEvent(add);																//������� ������� add � ������������ ���������
		}

		//�������� ��������
		if (WaitForSingleObject(del, 0) == WAIT_OBJECT_0 && proc_print.size()) {
			if (WaitForSingleObject(proc_print[proc_print.size() - 1], 0) == WAIT_OBJECT_0)
				WaitForSingleObject(complete, INFINITE);
			TerminateProcess(proc_vector[proc_vector.size() - 1]->hProcess, 1);				//�������������� ���������� ��������� ��������
			delete proc_vector[proc_vector.size() - 1];										//������������ ���������� ������
			proc_vector.pop_back();															//�������� �������� �������
			
			CloseHandle(proc_print[proc_print.size() - 1]);									//�������� handle �������
			proc_print.pop_back();															//�������� handle ������� �� �������
		}
		ResetEvent(del);																	//������� ������� del � ������������ ���������


		//������ ������� ��� ������ ������ � �������� ��������
		if (WaitForSingleObject(complete, 0) == WAIT_OBJECT_0 && proc_print.size()) {
			ResetEvent(complete);															//������� ������� complete � ������������ ���������

			i++;
			if (i >= proc_print.size()) i = 0;

			SetEvent(proc_print[i]);														//������� �������, ������������ ����� � �������� ��������
																							//� ���������� ���������
		}
	
	} while (1);

	//���������������� ���������� � �������� ������ ���� ���������� ���������
	while (proc_vector.size()) {
		TerminateProcess(proc_vector[proc_vector.size() - 1]->hProcess, 1);					//�������������� ���������� ��������� ��������
		delete proc_vector[proc_vector.size() - 1];											//������������ ���������� ������
		proc_vector.pop_back();																//�������� �������� �������

		CloseHandle(proc_print[proc_print.size() - 1]);										//�������� handle �������
		proc_print.pop_back();																//�������� handle ������� �� �������
	}

	CloseHandle(add);																		//�������� handle ������� add
	CloseHandle(del);																		//�������� handle ������� del
	CloseHandle(exit);																		//�������� handle ������� exit

	return 0;
}