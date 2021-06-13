#include <iostream>
#include <string.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

void print(int);

int main(){

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = print;
	act.sa_flags = 0;
	sigaction(1, &act, 0);

	while(1){
		pause();
	}

	return 0;
}

void print (int sig_num){

	cout « "Process " « getpid() « " says Hello!" « endl;

	sleep(1);
	kill(getppid(), 2);
}