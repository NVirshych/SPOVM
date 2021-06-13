#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

using namespace std;

pthread_mutex_t mutex;

//Реализация функции getch
int getch() {
struct termios oldt,
newt;
int ch;
tcgetattr( STDIN_FILENO, &oldt );
newt = oldt;
newt.c_lflag &= ~( ICANON | ECHO );
tcsetattr( STDIN_FILENO, TCSANOW, &newt );
ch = getchar();
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
return ch;
}

//Функция, выполняемая созданными потоками
void* print (void* ptr){

    int num = *(int*)ptr;                                                          //Переданный аргумент
    while(1){

        pthread_mutex_lock(&mutex);                                                //Блокировка мьютекса
        cout << "Thread №" << num << " says Hello!" << endl;
        pthread_mutex_unlock(&mutex);                                              //Разблокировка мьютекса
        sleep(1);       
    }
}

int main(){

    int f = 1;
    char a;
    int num = 0;

    pthread_mutex_init(&mutex, NULL);                                              //Инициализация мьютекса

    pthread_t newThread;                                                           //id нового потока       
    vector<pthread_t> threads;                                                     //Вектор id созданных потоков

    system("clear");

    while(f){

        a=getch();                                                        
 
        switch(a){

            //Добавление новго потока
            case('+'):

                pthread_create(&newThread, NULL, print, &num);                     //Создание нового потока
                threads.push_back(newThread);
                num++;
                break;

            //Завершение последнего созданного потока
            case('-'):

                if(!threads.size()) break;
                pthread_cancel(threads[threads.size() - 1]);                       //Сигнал о завершении потоку
                threads.pop_back();
                break;

            //Выход из программы
            case('q'):
                f=0;                                                               //Флаг выхода из цикла
                break;
        }
        a = 0;
    }   

    //Заврешение всех созданных потоков
    while(threads.size()){

        pthread_cancel(threads[threads.size() - 1]);
        threads.pop_back();
    }

    pthread_mutex_destroy(&mutex);                                                 //Уничтожение мьютекса

    return 0;
}