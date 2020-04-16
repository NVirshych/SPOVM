#include <iostream>
#include <dlfcn.h>

using namespace std;

int main(){

    system("clear");

    void* lib;                                                              //���������� ������������ ����������
    void (*concat)(void);                                                   //��������� �� �������

    lib = dlopen("/home/nick/spo/Lib/libconcatFiles.so", RTLD_LAZY);        //�������� ���������� � ������

    *(void **) (&concat) = dlsym(lib, "concat");                            //��������� ������ �������

    concat();                                                               //����� �������

    dlclose(lib);                                                           //�������� ���������� � ������

    return 0;
}