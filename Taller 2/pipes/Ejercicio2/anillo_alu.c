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

void hijo(int pipes[][2], int num_proc, int n){   //num_proc es el numero del proceso actual(empieza desde 0)
	//Cerrar las conexiones a pipes que no va a usar
	//Va a hacer write con pipe  num_proc+1 y read pipe num_proc
	close(pipes[0][PIPE_READ]);
	close(pipes[0][PIPE_WRITE]);
	
	for (int i = 1; i <= n; i++){       //i recorre pipes
		if( (num_proc+1 % n) == i ){    //Es el pipe al que escribe
			close(pipes[i][PIPE_READ]);    

		} else if( num_proc == i ){     //Es el pipe del que lee
        	close(pipes[i][PIPE_WRITE]);
		} else{                         //Es pipe que no utiliza 
			close(pipes[i][PIPE_READ]);           
			close(pipes[i][PIPE_WRITE]);               
		}
	}

	//ya que read es bloqueante, vamos a hacer que haga read del proceso anterior
	int msg;
	while (1){
		read(pipes[num_proc][PIPE_READ], &msg, sizeof(msg));
		msg++;
		write(pipes[num_proc][PIPE_WRITE], &msg, sizeof(msg));
	}

}

void hijo_s(int pipes[][2], int num_proc, int buffer, int n){

	for (int i = 1; i <= n; i++){       //i recorre pipes, excepto la 0(conexion al padre)
		if( (num_proc+1 % n) == i ){    //Es el pipe al que escribe
			close(pipes[i][PIPE_READ]);    

		} else if( num_proc == i ){     //Es el pipe del que lee
        	close(pipes[i][PIPE_WRITE]);
		} else{                         //Es pipe que no utiliza 
			close(pipes[i][PIPE_READ]);           
			close(pipes[i][PIPE_WRITE]);               
		}
	}

	//ya que read es bloqueante, vamos a hacer que haga read del proceso anterior
	int msg;
	int num_ale = generate_random_number();
	while (num_ale < buffer){
		num_ale = generate_random_number();
	}
	while (1){
		read(pipes[num_proc][PIPE_READ], &msg, sizeof(msg));
		if( msg >= num_ale ){
			//Se envia a padre el numero

		}else{
		    msg++;
		    write(pipes[num_proc][PIPE_WRITE], &msg, sizeof(msg));
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
	//Crear n pipes (n-1 para el anillo, 1 para hijo_s y padre)
	//pipes[0] se reserva para hijo_s y padre
	int pipes[n][2];
	for (int i = 0; i < n; i++){
		pipe(pipes[i]);
	}

	//Realizar los fork. Chequeo de que si es hijo_s, se le da proceso hijo_s
	for (int i = 0; i < n; i++){
		pid_t pid = fork();
		if( pid == -1){exit(EXIT_FAILURE)}
		if( pid == 0){
			if( i+1 == start ){           //Es el proceso S
                hijo_s(pipes, i, buffer, n);    
			}else{
				hijo(pipes, i, n);
			}
		}
		
	}
	
	//Padre va a hacer un write a pipe de hijo_s
	int resultado;
    write(pipes[0][PIPE_WRITE], &buffer, sizeof(buffer));

	//Luego padre va a hacer un read de ese pipe, esperando a que hijo escriba  
	read(pipes[0][PIPE_READ], &resultado, sizeof(resultado));  

}
