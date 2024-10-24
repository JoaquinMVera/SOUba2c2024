#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>


#include "CargarArchivos.hpp"

using namespace std;

//esto deberia ser cargarCompra
int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    //Este while fue modificado por problemas parseando
    while (getline(file, palabraActual)) {
        //lo meto en el hashmap y gg
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


void cargaThread(
    atomic<int> &archivoActual,vector<string> &filePaths,HashMapConcurrente &hashMap

) {
        for (int index = archivoActual.fetch_add(1); index < filePaths.size(); index = archivoActual.fetch_add(1)) {
            cargarArchivo(hashMap, filePaths[index]);
        }

}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    //tenemos los archivos
    //tenemos la cantidad de threads
    // lo mismo que promedio paralelo, pero en este caso no hay mutex porque el hasmap es concurrente

    vector<thread> threads;
    atomic<int> archivo_actual(0);

    for (int i = 0; i < cantThreads;i++) {
        threads.emplace_back(cargaThread, ref(archivo_actual), ref(filePaths),ref(hashMap));
    }

    for (auto &t:threads) {
        t.join();
    }   
}



#endif
