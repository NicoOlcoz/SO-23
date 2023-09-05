#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int reps = 0;

void sigurg_handler(int sig){
	printf("Ya va!\n");
	reps++;
}


void hijo(char* argv[]){
	signal(SIGURG, sigurg_handler);
	while (1){
        if( reps == 5){
			//Cuando se hace el ultimo "Ya va" se hace esta parte. Ver como se hace 
            pid_t pid_padre = getppid();
	        kill(pid_padre, SIGINT); 

            //Hacer el nuevo array de argv para los comandos dados
	        char* argv2[sizeof(*argv)-1];
	        for(int i = 1; i < sizeof(*argv); i++){
		        argv2[i-1] = argv2[i];
	        }
	        execvp(argv2[0], argv2);
		}

	}
	
}

int main(int argc, char* argv[]) {
    //Hacer el fork
	pid_t pid = fork();
	if( pid == 0 ){
		hijo(argv);
	}

    sleep(1);
	//Ciclo de "sup"
	for (int i = 0; i < 5; i++){
		printf("sup!\n");

		//Manda señal para que hijo escriba "Ya va!"
		kill(pid, SIGURG);
		sleep(1);

		//Nanosleep?
	}

	wait(NULL);

	return 0;
}
