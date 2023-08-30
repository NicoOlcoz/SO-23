#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

int primerPipe(int indice, int countPipes){
	return indice == 0;
}

int ultimoPipe(int indice, int countPipes){
	return indice == (countPipes - 2);
}

void hijo(int pipes[][2], int i, char ***progs, int count) {
	// for (size_t j = 0; j < count; j++) {
	// 	// Con close eliminamos la referencia a pipes que no vamos a usar
	// 	if (j != i || (j != (i-1) ) )
	// 	{
	// 		close(pipes[j][0]);// Cerramos READ y WRITE
	// 		close(pipes[j][1]);// Cerramos READ y WRITE

	// 	}

	// 	else if ( j == i){ // Situados en el pipe a la izquierda del hijo
	// 		close(pipes[j][PIPE_READ]); // Si es pipe siguiente cerramos READ

	// 	} else if (j != (i-1) ) {
	// 		close(pipes[j][PIPE_WRITE]); // Si es pipe anterior cerramos WRITE

	// 	}
	// }

	// Si soy el primer proceso (0), quiero redirigir mi stdoutput
	if(i==0){
		dup2(pipes[i][PIPE_WRITE], STD_OUTPUT);
	}else{
		// Soy  el último proceso
		dup2(pipes[0][PIPE_READ], STD_INPUT);
	}

	// if (primerPipe(i, count)) {
	// 	dup2(pipes[i][PIPE_WRITE], STD_OUTPUT);
		
	// } else if ( ultimoPipe(i, count) ) {
	// 	dup2(pipes[count - 2][PIPE_READ], STD_INPUT);
		
	// } else {
	// 	dup2(pipes[i][PIPE_READ], STD_INPUT);
	// 	dup2(pipes[i][PIPE_WRITE], STD_OUTPUT);
		
	// }
	
	// TODO: Ejecutar el comando del hijo
	// printf("Este es el hijo %s\n", (progs+sizeof(progs[0])));
	
	execvp(progs[i][0], progs[i]);
	

	
}




static int run(char ***progs, size_t count)
{	
	int r, status;

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);

	int pipes[count - 1][2];
	for (int i = 0; i < count - 1; i++) {
		pipe(pipes[i]);
	}

	
	for (size_t i = 0; i < count; i++) {
		int pid = fork();
		if (pid == 0) {
			hijo(pipes, i, progs, count); // completar
			// exit(EXIT_SUCCESS);
		}
		children[i] = pid;
	}
	
	//TODO: Pensar cuantos procesos necesito
	//TODO: Pensar cuantos pipes necesito.


	//TODO: Para cada proceso hijo:
			//1. Redireccionar los file descriptors adecuados al proceso
			//2. Ejecutar el programa correspondiente

	//Espero a los hijos y verifico el estado en que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char*** programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
