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


void proceso_hijo(int id, int pipes[][2], int soyElPrimero, int pipeAlPadre[2], int cantidadTotal) {
	int numero_secreto = -1;
	int numero;

	if(soyElPrimero && numero_secreto == -1) {
		numero_secreto = generate_random_number();
		printf("EL numero secreto es: %d \n", numero_secreto);
	}

	while(1) {

		//me quedo aca para siempre
		read(pipes[id][PIPE_READ], &numero, sizeof(numero));
		int siguientePipe = (id+1) % (cantidadTotal);

		//si me pasaron -1, me
		if (numero == -1) {
			write(pipes[siguientePipe][PIPE_WRITE], &numero, sizeof(numero));
			exit(EXIT_SUCCESS);
		}



		if (soyElPrimero && numero >= numero_secreto ) {
			printf("Ya pasaron mi numero secreto. Soy el primer hijo\n");
			write(pipeAlPadre[PIPE_WRITE], &numero, sizeof(numero));


			//ACA MATO A MIS AMIGOS
			numero = -1;
			write(pipes[siguientePipe][PIPE_WRITE], &numero, sizeof(numero));
			exit(EXIT_SUCCESS);
		}
		//soy un hijo cualquiera
				numero++;

		printf("Soy el hijo %d y estoy pasando el numero %d. Recibi %d \n", id, numero, numero - 1);

		write(pipes[siguientePipe][PIPE_WRITE], &numero, sizeof(numero));
	}
}


int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start_process, start_number;
	n = atoi(argv[1]);
	start_number = atoi(argv[2]);
	start_process = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}

	printf("Vamos a usar n: %d \n", n);
	printf("Vamos a usar el primer numero: %d \n", start_number);
	printf("Vamos a usar el primero proceso: %d \n", start_process);

	int pipes[n][2];
	int pipeParaAvisarAlPadre[2];
	pipe(pipeParaAvisarAlPadre);

	for(int i = 0; i < n; i ++)   {
		if (pipe(pipes[i]) < 0) {
			printf("Che, fallo la creacion de un pipe \n");
			exit(EXIT_FAILURE);
		}
	}


	printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, start_number, start_process);

	for(int i = 0; i < n; i++) {
		pid_t pidHijo = fork();
		if(pidHijo < 0){
			printf("Che, fallo la creacion de un fork \n");
			exit(EXIT_FAILURE);
		}

		if (pidHijo == 0) {
			//ya soy el hijo
			proceso_hijo(
				i,
				pipes,
				i == start_process,
				pipeParaAvisarAlPadre,
				n
			);
		}
	}

	write(pipes[start_process][PIPE_WRITE], &start_number, sizeof(start_number));

	int valorFinal;
	read(pipeParaAvisarAlPadre[PIPE_READ], &valorFinal, sizeof(valorFinal));
	printf("Llegue al final, el ultimo numero fue: %d \n", valorFinal);

	printf("Matando a todos los hijos \n");
	//POdria haber sido un sigkill, tenineod un array de los hijos y gg, era mas facil, hicimos como que maten

	return 0;
    
}
