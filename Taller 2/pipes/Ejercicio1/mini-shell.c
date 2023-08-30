#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

void hijo(int pipes[][2], int i, char ***progs, int count) {
	for (int j = 0; j < count - 1; j++) {
		// Con close eliminamos la referencia a pipes que no vamos a usar
		if (j != i && j != i - 1) {
			// printf("cierro read & write %d, del proceso %d\n", j, i);
			close(pipes[j][PIPE_READ]);
			close(pipes[j][PIPE_WRITE]);
		} else if (j == i){	// Si es el pipe con el que me comunico con el SIGUIENTE proceso
			// printf("cierro el read %d, del proceso %d\n", j, i);
			close(pipes[j][PIPE_READ]); 
		} else if (j == i - 1) { // Si es pipe con el que me comunico con el ANTERIOR proceso
			// printf("cierro el write %d, del proceso %d\n", j, i);
			close(pipes[j][PIPE_WRITE]); 
		}
	}

	if(i == 0){ // Si es el primer proceso (0), quiero redirigir mi stdoutput
		// printf("redirijo el write %d, del proceso %d\n", i, i);
		dup2(pipes[i][PIPE_WRITE], STD_OUTPUT);
	} else if (i == count - 1) { // Si es el ultimo proceso
		// printf("redirijo el read %d, del proceso %d\n", i - 1, i);
		dup2(pipes[i - 1][PIPE_READ], STD_INPUT);
	} else { // Si estamos en un proceso intermedio
		// printf("redirijo el read %d y el write %d del proceso %d\n", i - 1, i, i);
		dup2(pipes[i - 1][PIPE_READ], STD_INPUT);
		dup2(pipes[i][PIPE_WRITE],STD_OUTPUT);
	}
	
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
			hijo(pipes, i, progs, count);
		}
		children[i] = pid;
	}

	// Cierro descriptores de pipes en el proceso padre
    	for (int i = 0; i < count - 1; i++) {
        	close(pipes[i][0]);
        	close(pipes[i][1]);
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
