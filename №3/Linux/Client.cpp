#include <iostream>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>

using namespace std;

int main(int argc, char* argv[]){

	int sem = semget(ftok("./Parent.cpp", 1), 0, IPC_CREAT | 0666);

	struct sembuf wait = {0, 0, SEM_UNDO}; //Ожидание семафора чтения
	struct sembuf lock = {0, 1, SEM_UNDO}; //Блокировка семафора чтения
	struct sembuf unlock = {1, -1, SEM_UNDO}; //Разблокировка семафора записи
	semop(sem, &lock, 1);

	int hRead = atoi(argv[argc-1]); //Дескриптор чтения из канала
	char buf[80]; //Буфер для чтения

	while(1){
	
		semop(sem, &wait, 1);
		semop(sem, &lock, 1);

		read(hRead, buf, 80); //Чтение из канала в буфер
		cout « endl « "Message: " « buf « endl « endl; //Вывод сообщения на экран

		semop(sem, &unlock, 1);
	}

	return 0;
}