#include <iostream>
#include <dlfcn.h>

using namespace std;

int main(){

    system("clear");

    void* lib;                                                              //Дескриптор динамической библиотеки
    void (*concat)(void);                                                   //Указатель на функцию

    lib = dlopen("/home/nick/spo/Lib/libconcatFiles.so", RTLD_LAZY);        //Загрузка библиотеки в память

    *(void **) (&concat) = dlsym(lib, "concat");                            //Получение адреса функции

    concat();                                                               //Вызов функции

    dlclose(lib);                                                           //Выгрузка библиотеки в памяти

    return 0;
}