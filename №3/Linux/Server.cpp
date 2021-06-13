#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main(){

	system("clear");

	int sem = semget(ftok("./Parent.cpp", 1), 2, IPC_CREAT | 0666); // 0 - семафор чтения, 1 - семафор записи
	struct sembuf wait = {1, 0, SEM_UNDO}; //Ожидание семафора записи
	struct sembuf lock = {1, 1, SEM_UNDO}; //Блокировка семафора записи
	struct sembuf unlock = {0, -1, SEM_UNDO}; //Разблокировка семафора чтения

	int hndl[2]; //Дескрипторы канала
	char buf[80];

	//Открытие канала
	if(pipe(hndl)){
		cout « "Failed to create pipe!" « endl;
		return 0;
	}
	sprintf(buf, "%d", hndl[0]); //Дескриптор чтения в буфер

	pid_t pid = fork(); //pid нового процесса

	switch(pid){
	//Ошибка fork
	case(-1):
		cout « "Failed to fork!" « endl;
		return 0;

	case(0):
		close(hndl[1]); //Закрытие дескриптора записи в дочернем процессе
		execl("./Client", buf, NULL); //Исполнение новой программы в доч. проц
		break;
	}
	close(hndl[0]); //Закрытие дескриптора чтения в род. процессе

	while(1){

		semop(sem, &wait, 1);
		semop(sem, &lock, 1);

		//Ввод строки
		cout « "Input message: ";
		cin.getline(buf, 80, '\n');

		//Выход при вводе пустой строки
		if(!buf[0]){
			kill(pid, SIGTERM);
			break;
		}

		write(hndl[1], buf, 80); //Запись строки в канал

		semop(sem, &unlock, 1);
	}

	semctl(sem, 0, IPC_RMID); //Удаление семафора
	return 0;
}