#include <iostream>
#include <vector>
#include <ctime>
#include <filesystem> 
#include <fstream>
#include <fstream>
#include <sstream>
#include "../../src/CargarArchivos.hpp"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
using namespace std;

const string ARCHIVO_LECTURA = "../../data/exp1/data";
const string EXTENSION_ARCHIVO_LECTURA= "_processed.txt";
const string ARCHIVO_TESTER = "./promedioTester.txt";
const string ARCHIVO_SALIDA = "../resultados/resultadoExperimento1.txt";
const int cant_archivos = 5;

//La funcion para agarrar el promedio paralelo que calculamos y que coincida con el promedio real
void testearPromedio(const float& promedioParalelo, HashMapConcurrente& hashmap) {
    if(hashmap.promedio() == promedioParalelo) {
        return;
    }

    cout << "Promedio paralelo no calculo correctamente el promedio" << endl;
    exit(EXIT_FAILURE);
}

double realizarExperimentosConCantThreads(int threads) {
   
    vector<string> archivos = {};

    HashMapConcurrente hashmap;

    for(int i = 1; i <= cant_archivos; i++) {
        string archivoParaAbrir = ARCHIVO_LECTURA + to_string(i)+ EXTENSION_ARCHIVO_LECTURA;
        archivos.push_back(archivoParaAbrir);
    }

    for(auto &archivo:archivos) {
        cargarArchivo(hashmap,archivo);
    }

        int repeticiones = 10;
        double contador = 0;
        for (int j = 0; j < repeticiones ; j++)
        {
            auto t1 = Clock::now();
            float promedioParalelo = hashmap.promedioParalelo(threads);
            auto t2 = Clock::now();
            contador += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            testearPromedio(promedioParalelo, hashmap);
        }
        //Hacmeos esto para tener el tiempo promedio, un valor mas real
        double duracion = contador / repeticiones;
        return duracion;
}



int main(int argc, char* argv[]) {

    // Creamos el archivo de salida
    ofstream outFile(ARCHIVO_SALIDA);

    
    for (int i = 1; i < 11; i++)
    {
        double duracion = realizarExperimentosConCantThreads(i); 
        string nombreExperimento = "Experiento 1. Cantidad de Threads: " + to_string(i) + "," + to_string(duracion);
        outFile << nombreExperimento << endl;
        cout << nombreExperimento << endl;
    }

   outFile.close();
   return 0;
}