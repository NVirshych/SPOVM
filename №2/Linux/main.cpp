#include <iostream>
#include <vector>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <signal.h>

using namespace std;

int NUM = 0;
vector<pid_t> PID;

void print(int);

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

int main(){

	//Создание обработчика сигнала
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = print;
	act.sa_flags = 0;
	sigaction(2, &act, 0);

	int f = 1;
	char a = 1;

	system("clear");

	pid_t n_pid; //pid нового процесса

	while(f){

		a=getch();

		switch(a){

			//Добавление новго процесса
		case('+'):

			n_pid = fork(); //Создание дубликата текущего процесса

			switch(n_pid){
			//Не удалось создать дубликат
			case(-1):
				cout « "Failed to fork" « endl;
				break;

			//Инструкции дочернему процессу
			case(0):
				execv("./Child", NULL); //Запуск пругой программы в новом процессе
				break;

			//Инструкции родительскому процессу
			default:
				PID.push_back(n_pid); //Добавление pid нового процесса в вектор
			}

			sleep(1);

			if(PID.size() == 1)
				kill(PID[0], 1);
			break;

		//Удаление последнего созданного процесса
		case('-'):

			if(NUM == PID.size()-1)
			pause();
			kill(PID[PID.size()-1], SIGTERM); //Завершение процесса
			PID.pop_back(); //Удаление pid завершенного процесса из вектора
			break;

		//Выход из программы
		case('q'):

			f=0; //Флаг выхода из цикла
			break;
		}

		a = 0;
	}

	while(PID.size()){

		kill(PID[PID.size()-1], SIGTERM); //Завершение процесса
		PID.pop_back(); //Удаление pid завершенного процесса из вектора
	}

	return 0;
}

void print(int signum){

	NUM++;
	if(NUM >= PID.size()) NUM=0;
	kill(PID[NUM], 1);
}