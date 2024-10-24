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

const string ARCHIVO_LECTURA = "../../data/data";
const string EXTENSION_ARCHIVO_LECTURA= ".txt";
const string ARCHIVO_TESTER = "./stressTest.txt";
const string ARCHIVO_SALIDA = "../resultados/resultadoExperimento2.txt";
const int cantArchivos = 3;

//La funcion para agarrar testear el hashmap y el archivo tester y ver que coincidan
bool testearHashMap(const string& archivoTester, HashMapConcurrente& hashmap) {
    ifstream inFile(archivoTester);
    string linea;

    while (getline(inFile, linea)) {
        // Usamos este string stream para agarrar la linea y parsearla
        stringstream ss(linea);

        string clave;
        int valor;

        // Leemos la linea hasta la proxima coma, asi tenemos la calve
        if (!getline(ss, clave, ',')) {
            cerr << "Error al parsear la clave en la línea: " << linea << endl;
            return false;
        }

        //guardamos en valor lo que quedaba despues!
        ss >> valor;

        //agarramos el valor del hashmap
        int valorEnHashMap = hashmap.valor(clave);

        //comparamos, si es verdad esto, es que hay algo mal
        if (valorEnHashMap != valor) {
            cout << "Discrepancia encontrada: " << clave << " (Esperado: " << valor << ", Actual: " << valorEnHashMap << ")" << endl;
            return false;
        }
    }
    cout << "Verificación completada con éxito. Todos los valores coinciden." << endl;
    return true;
}

double realizarExperimentosConCantThreads(int threads) {
   
    vector<string> filePath = {};

    for(int i = 1; i <= cantArchivos; i++) {
        string archivoParaAbrir = ARCHIVO_LECTURA + to_string(i)+ EXTENSION_ARCHIVO_LECTURA;
        filePath.push_back(archivoParaAbrir);
    }

        int repeticiones = 10;
        double contador = 0;
        for (int j = 0; j < repeticiones ; j++)
        {
            HashMapConcurrente hashmap;
            auto t1 = Clock::now();
            cargarMultiplesArchivos(hashmap, threads, filePath);
            auto t2 = Clock::now();
            contador += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            testearHashMap(ARCHIVO_TESTER, hashmap);

        }
        //lo mismo que el experimento 1, hacmeos esto para tener el tiempo promedio
        double duracion = contador / repeticiones;
        return duracion;
}



int main(int argc, char* argv[]) {

    // Creamos el archivo de salida
    ofstream outFile(ARCHIVO_SALIDA);

    
    for (int i = 1; i < 50; i++)
    {
        double duracion = realizarExperimentosConCantThreads(i); 
        string nombreExperimento = "Experiento 2. Cantidad de Threads: " + to_string(i) + "," + to_string(duracion);
        outFile << nombreExperimento << endl;
        cout << nombreExperimento << endl;
    }

   outFile.close();
   return 0;
}