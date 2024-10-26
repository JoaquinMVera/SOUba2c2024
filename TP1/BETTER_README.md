# Aclaraciones TP

- Entendemos como no bloqueantes aquellos metodos que no estan bloqueando TODO el tiempo la estructura. Nos permitimos un bloqueo total en un momento 0, para aplicar la politica de SNAPSHOT.
- Los tests que hemos realizado son de 3 tipos: Unit Tests, para comprobar el funcionamiento de los concurrentes (mas su correcto funcionamiento natural, no la concurrencia en si), y 2 experimentos para explorar la mejora de rendimiento que nos ofrece el threading, asi como hacer un stress test que, en el scope de la materia, nos sirven para demostrar, junto con el informe, que nuestro sistema funciona correctamente
- Hemos realizado cambios en el esqueleto, sobre todo siguiendo los errores que fueron surgiendo en la lista de mails y de consultas con los docentes. Estan debidamente marcados en el codigo.
- Tambien cambiamos el corpus y los scripts, para hacer un formato estilo csv, de manera tal que podamos llevar a cabo los experimentos

## Como reproducir los experimentos

Primero, generar los archivos "tester", que no son otra cosa que archivos con estilo csv tales que estamos "contando" la cantidad de veces que se repite una linea (que deberia ser el valor en nuestro hashmap).
Una vez parados sobre la carpeta scripts, correr:

- `awk -f contar-palabras-multifile.awk ../data/exp2/data1.txt ../data/exp2/data2.txt ../data/exp2/data3.txt > ../experimentos/exp2/stressTester.txt`
- `awk -f contar-palabras-multifile.awk ../data/exp1/data1_processed.txt ../data/exp1/data2_processed.txt ../data/exp1/data3_processed.txt > ../experimentos/exp1/promedioTester.txt`

Una vez hecho esto, ya tenemos toda la informacion para constrastar que los hashmap se hayan realizado correctamente!

Ahora podemos correr los experimentos!

Ahora vamos a compilarlos con los siguientes comandos, parandote en cada carpeta (primer comando adentro de exp1 y el segundo en exp2):

- `g++ ../../src/HashMapConcurrente.cpp ../../src/CargarArchivos.cpp experimento.cpp -o exp -std=c++17 -pthread`
- `g++ ../../src/HashMapConcurrente.cpp ../../src/CargarArchivos.cpp experimento2.cpp -o exp2 -std=c++17 -pthread`

y luego, parados dentro de las respectivas carpetas, ejecutarlos como

- `./exp`
- `./exp2`

Luego, en la carpeta:

`/experimentos/resultados`

Van a encontrar los txt con los resultados! Tienen un pequeño texto indicando el expeirmento, y la cantidad de threads, y luego separado por coma, el tiempo en milisegundos que se tardo en esa ejecucion!

Ahora solo nos queda pararnos sobre experimentacion y correr el comando

`python graficador.py`

Tener en cuenta que se requiere la dependencia de **matplotlib**.

Luego, tener en cuenta que por distintos equipos, y distintas maquinas, es posible ver picos (altos y bajos) en los distintos graficos, con distintos resultados y esto es algo correcto y una desviacion natural al usar una herramienta real, por limitaciones tanto de software como de hardware. Lo que si es importante notar es que hay un decenso significativo gracias al uso de threads!

Adicionalmente, porque nos dimos cuenta que correr los experimentos puede ser medio handy, dejamos un scripsito que hace todo lo anteriormente mencionado!

`sh run_experimentos.sh`
`sh clean_experimentos.sh`
