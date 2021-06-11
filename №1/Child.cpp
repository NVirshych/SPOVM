#include <Windows.h>
#include <iostream>

using namespace std;

int main() {
	
	SYSTEMTIME st;

	cout << "Child process <3" << endl << endl;

	GetLocalTime(&st);
	cout << "Date: " << st.wYear << "." << st.wMonth << "." << st.wDay << endl 
		 << "Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds << endl << endl;


	system("pause");
	return 0;
}