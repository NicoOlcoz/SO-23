#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

void hijo(int pipes[][2], int num_proc, int n){   //num_proc es el numero del proceso actual(empieza desde 0)
	int anterior = mod(num_proc-1, n);
	//Cerrar las conexiones a pipes que no va a usar
	//Va a hacer write con pipe num_proc+1 y read pipe num_proc
	close(pipes[n][PIPE_READ]);
	close(pipes[n][PIPE_WRITE]);
	
	for (int i = 0; i < n; i++){       //i recorre pipes
		if( num_proc == i ){    //Es el pipe al que escribe
			close(pipes[i][PIPE_READ]);    
		} else if( anterior == i ){     //Es el pipe del que lee
        	close(pipes[i][PIPE_WRITE]);
		} else{                         //Es pipe que no utiliza 
			close(pipes[i][PIPE_READ]);           
			close(pipes[i][PIPE_WRITE]);               
		}
	}
	
	//read devuelve numero de bytes leidos. Si no hay nada para leer, devuelve 0.
	int msg;
	while (1){
		ssize_t leido = read(pipes[anterior][PIPE_READ], &msg, sizeof(msg));
		if( leido == 0 ){                //Si no hay nada para leer, termina la operacion.
			//close(pipes[num_proc][PIPE_READ]);
			//close(pipes[num_proc][PIPE_WRITE]);
			exit(EXIT_SUCCESS);
		}else{
		    msg++;
		    write(pipes[num_proc][PIPE_WRITE], &msg, sizeof(msg));
		}
	}
}

void hijo_s(int pipes[][2], int num_proc, int buffer, int n){
    int anterior = mod(num_proc-1, n);

	for (int i = 0; i < n; i++){       //i recorre pipes
		if( num_proc == i ){    //Es el pipe al que escribe
			close(pipes[i][PIPE_READ]);    
		} else if( anterior == i ){     //Es el pipe del que lee
        	close(pipes[i][PIPE_WRITE]);
		} else{                         //Es pipe que no utiliza 
			close(pipes[i][PIPE_READ]);           
			close(pipes[i][PIPE_WRITE]);               
		}
	}

	int msg;
	int num_ale = generate_random_number();
	while (num_ale < buffer){
		srand(time(NULL));
		num_ale = generate_random_number();
	}
	printf("Numero al azar es: %i \n", num_ale);
	//Se lee el numero que da el padre, se incrementa y pasa al otro proceso del anillo
    read(pipes[n][PIPE_READ], &msg, sizeof(msg));

    //ciclo de anillo
	while (1){
		if( msg >= num_ale ){      //Se escribe a padre
			write(pipes[n][PIPE_WRITE], &msg, sizeof(msg));
			exit(EXIT_SUCCESS);
		}else{                     //Continua ciclo anillo
		    msg++;
		    write(pipes[num_proc][PIPE_WRITE], &msg, sizeof(msg));
			read(pipes[anterior][PIPE_READ], &msg, sizeof(msg));
		}
	}
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
    /* COMPLETAR */
	//Crear n+1 pipes (n para el anillo, n+1 para hijo_s y padre)
	//pipes[0] se reserva para hijo_s y padre
	int pipes[n+1][2];
	for (int i = 0; i < n+1; i++){
		pipe(pipes[i]);
	}

	//Realizar los fork. Chequeo de que si es hijo_s, se le da proceso hijo_s
	pid_t pid_s;
	for (int i = 0; i < n; i++){
		pid_t pid = fork();
		if( pid == -1){exit(EXIT_FAILURE);}
		if( pid == 0){
			if( i == start ){           //Es el proceso S
                hijo_s(pipes, i, buffer, n);   
			}else{
				hijo(pipes, i, n);
			}
		}
		if( i == start ){
			pid_s = pid;
		}
		
	}
	
    //Cerrar las referencias a hijos que no sean del pipe n+1
	for (int i = 0; i < n; i++){        //i recorre pipes
			close(pipes[i][PIPE_READ]);           
			close(pipes[i][PIPE_WRITE]);               
	}

	//Padre va a hacer un write a pipe de hijo_s
	int resultado;
    write(pipes[n][PIPE_WRITE], &buffer, sizeof(buffer));

    int valorFinal;
	if (waitpid(pid_s, &status, 0) == pid_s) { // Una vez que termina el hijo principal
		read(pipes[n][PIPE_READ], &valorFinal, sizeof(valorFinal)); // Recibe el valor
	}
    printf("El valor final es %i\n", valorFinal);
}
