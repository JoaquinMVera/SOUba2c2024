#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

typedef char*** matrix;

/*
	Explicacion del argumento progs:

	Corresponde con una matriz donde cada fila representa el iesimo programa pasado
	dividido por el pipe. En cada fila esta el nombre del programa 
	en su primera posicion y los parametros en las posiciones restantes.

	Ejemplo: 'du -h /home | head -n 10 | sort -rh'
	
	Representacion:
	[
		['du', '-h', '/home'],
		['head', '-n', '10'],
		['sort', '-rh']
	]

*/

static int run(matrix progs, size_t count)
{	
	int r, status;

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);
	int pipe_salida[2];
	int pipe_entrada[2];


	for (int i = 0; i < count ; i++) {

		//creo el pipe si no soy el ultimo
		if (i < (count - 1) && pipe(pipe_salida) < 0) {
			printf("Hola, falle en la creacion del pipe : %d", i);
			exit(EXIT_FAILURE);
		}

		//creo el hijo
		children[i] = fork();

		//me fijo que no haya error
		if (children[i] < 0) {
			printf("HOla, falle en la creacion del hijo i-esimo: %d", i);
			exit(EXIT_FAILURE);
		}

		if (children[i] == 0) {
			//soy un hijo
		
			if ( i < count-1) {
				//NO SOY EL ULTIMO
				close(pipe_salida[PIPE_READ]);
				//tiene que hacer esto, para esto lo cree
				// pongo mi salida a la escritura del pipe
				dup2(pipe_salida[PIPE_WRITE], STDOUT_FILENO);
			}

			if (i > 0) {
				//NO SOY EL PRIMERO
				close(pipe_entrada[PIPE_WRITE]);
				//lo que hago aca es leer de mi pipe_entrada
				dup2(pipe_entrada[PIPE_READ], STDIN_FILENO);
			}

			//ejecuto, este execvp toma un array
			execvp(progs[i][0], progs[i]);

			//algo salio mal
			exit(EXIT_FAILURE);
		}

		//yo soy el papa
		if (i > 0) {
			//cierro esto para que el hijo actual pueda leer del pipe_entrada
			//sies el 1, nos da igual, porque pipe_entrada no existe
			close(pipe_entrada[PIPE_WRITE]);
			close(pipe_entrada[PIPE_READ]);
		}
		
		if (i < count - 1) {
			//"muevo" los pipes al entrada para la proxima iteracion
			pipe_entrada[PIPE_READ] = pipe_salida[PIPE_READ];
			pipe_entrada[PIPE_WRITE] = pipe_salida[PIPE_WRITE];
		}

	}

	//Espero a los hijos y verifico el estado que terminaron
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
	matrix programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
