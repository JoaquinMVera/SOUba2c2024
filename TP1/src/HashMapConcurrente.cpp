#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>
#include <mutex>


#include "HashMapConcurrente.hpp"
using namespace std;


HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
    
}


unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    int index = hashIndex(clave);

    mutex_por_letras[index].lock();

    auto iteradorPrincipio = tabla[index]->begin();
    auto final = tabla[index]->end();

    //final = -> ] ya me fui del array
    //lockear desde aca

    while(iteradorPrincipio != final && (*iteradorPrincipio).first != clave ) {
        iteradorPrincipio++;
    }

    if (iteradorPrincipio == final) {
        //aca no lo encontre
        //hay que agregarlo nuevo
        hashMapPair parNuevo = hashMapPair(clave, 1);
        tabla[index]->insertar(parNuevo);

    } else {
        (*iteradorPrincipio).second++;
    }

    mutex_por_letras[index].unlock();

}

std::vector<std::string> HashMapConcurrente::claves() {
    //todo preguntar como es el tema de la inanicion
    vector<string> result;

    for (int i = 0; i < HashMapConcurrente::cantLetras; ++i) {

        for (const auto& nodo: *tabla[i]) {
            result.push_back(nodo.first);
        }

        //---> lei la 1
        //.. leyendo la n
        //actualizan la 1

        //podria bloquear 1 por 1, que pasa cuando nos actualizan 

        // -> tenrr una lista de cabezas[cantLetras]
        // while -> no haya cabezas que cambiaron
        // -> hacer todo lo otro : PROBLEMA: INANICION

    } 

    return result;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int index = hashIndex(clave);


    //aca entinedo entoces que podemos hacer mutex y listo?
    //preguntar
    mutex_por_letras[index].lock();

    auto iteradorPrincipio = tabla[index]->begin();
    auto final = tabla[index]->end();

    while(iteradorPrincipio != final && (*iteradorPrincipio).first != clave ) {
        iteradorPrincipio++;
    }

    int result;

    if (iteradorPrincipio == final) {
        //aca no lo encontre
        result = 0;

    } else {
       result = (*iteradorPrincipio).second;
    }

    mutex_por_letras[index].unlock();

    return result;
    
}

float HashMapConcurrente::promedio() {

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }
    if (count > 0) {
        return sum / count;
    }
    return 0;        
}



#endif
