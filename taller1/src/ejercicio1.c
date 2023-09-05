#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int idActual;

int n;
int numero_maldito;
int generate_random_number(){
	return (rand() % n);
}

struct IntPair {
    int first;
    int second;
};
struct IntPair tupleArray[10]; // first es PID y second si está vivo (1).


void sigterm_handler(int sig) {
    srand(getpid() + time(NULL));
    int azar = generate_random_number();
    printf("Azar = %d \n", azar);
    if(azar == numero_maldito){
        printf("Estas son mis últimas palabras.\n");
        exit(EXIT_SUCCESS);
    }
}

void hijo() {
    signal(SIGTERM, sigterm_handler);
    while(1){};
}

void sigchild_handler(int sig) {
    pid_t pidHijo = wait(NULL);
    // pidHijo es el pid del hijo que perdió
    for (size_t i = 0; i < n; i++)
    {
        if (tupleArray[i].first == pidHijo) {
            tupleArray[i].second = 0;
        }
    }
}

int main(int argc, char const *argv[]){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(getpid() + time(NULL));
    
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);

    // Completar
    signal(SIGCHLD, sigchild_handler);

    pid_t pid; 
    for (size_t i = 0; i < n; i++) {
        pid = fork();
        if (pid == -1) {
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            hijo();
            break;
        }
        tupleArray[i].first = pid;
        tupleArray[i].second = 1;
    }

    
    if (pid != 0) {
        for (size_t k = 0; k < rondas; k++) {
            for (size_t i = 0; i < n; i++) {
                idActual = i;
                if(tupleArray[i].second == 1){ //Si el hijo i esta vivo
                    printf("Envio SIGTERM a proceso %d \n", tupleArray[i].first);
                    kill(tupleArray[i].first, SIGTERM);
                    sleep(1);
                }
            }
        } 
    }
    
    for (size_t i = 0; i < n; i++)
    {
        if (tupleArray[i].second == 1) {
            printf("El hijo %d es ganador!\n", tupleArray[i].first);
            kill(tupleArray[i].first, SIGKILL);
        }
    }
    

    exit(0);
}