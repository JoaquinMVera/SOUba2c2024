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
    //Arreglado del esqueleto original
    return (unsigned int)(tolower(clave[0]) - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    int index = hashIndex(clave);

    mutex_por_bucket[index].lock();

    auto iterador_principio = tabla[index]->begin();
    auto iterador_final = tabla[index]->end();

    //iterador_final = -> ya me fui del array, apunta mas alla

    while(iterador_principio != iterador_final && (*iterador_principio).first != clave ) {
        iterador_principio++;
    }

    if (iterador_principio == iterador_final) {
        //Si estoy aca, no encontre el elemento
        //hay que agregarlo
        hashMapPair parNuevo = hashMapPair(clave, 1);
        tabla[index]->insertar(parNuevo); //ATOMICO

    } else {
        //ya existia! lo aumento
        (*iterador_principio).second++; // ATOMICO
    }

    mutex_por_bucket[index].unlock();

}

std::vector<std::string> HashMapConcurrente::claves() {
    vector<string> resultado;


    //bloqueo todo    
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_por_bucket[i].lock();
    }

    //voy leyendo y mientras voy leyendo, voy desmuteando a medida que leo
    for (int i = 0; i < HashMapConcurrente::cantLetras; ++i) {

        for (const auto& nodo: *tabla[i]) {
            resultado.push_back(nodo.first);
        }
        mutex_por_bucket[i].unlock();
    } 

    return resultado;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int index = hashIndex(clave);

    //Misma idea que con incrementar, solo que comportamiento distinto al encontrar
    mutex_por_bucket[index].lock();

    auto iterador_principio = tabla[index]->begin();
    auto iterador_final = tabla[index]->end();

    while(iterador_principio != iterador_final && (*iterador_principio).first != clave ) {
        iterador_principio++;
    }

    int result;

    if (iterador_principio == iterador_final) {
        //aca no lo encontre
        result = 0;

    } else {
       result = (*iterador_principio).second;
    }

    mutex_por_bucket[index].unlock();

    return result;
    
}

float HashMapConcurrente::promedio() {

    float suma = 0.0;
    unsigned int contador = 0;

    //bloqueo toda la tabla para la snapshot
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_por_bucket[i].lock();
    }

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        //para cada lista, sumo sus producto.second -> su cantidad, su valor
        for (const auto& producto : *tabla[index]) {
            suma += producto.second;
            contador++;
        }
        mutex_por_bucket[index].unlock();
    }
    if (contador > 0) {
        return suma / contador;
    }
    return 0;        
}


void HashMapConcurrente::calculoThread(atomic<int> &lista_actual, mutex *mutexes,  ListaAtomica<hashMapPair> **tabla_thread, vector<pair<int,int>> *resultados_thread) {

    for (int index = lista_actual.fetch_add(1); index < HashMapConcurrente::cantLetras; index = lista_actual.fetch_add(1)) {
        pair<int,int> par = make_pair(0,0);
        //aca estoy en una lista, vamos a procesarla
        for (auto &producto: *tabla_thread[index]) {
            //cantidad de productos
            par.second+= producto.second;
            //cantidad de tipos de productos
            par.first++;
        }
        mutexes[index].unlock();
        resultados_thread->at(index) = par;
    }
    
}

float HashMapConcurrente::promedioParalelo(unsigned int cantThreads){
    vector<thread> threads;
    atomic<int> lista_actual(0);
    vector<pair<int,int>> resultados(cantLetras);

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        mutex_por_bucket[i].lock();
    }

    for (int i = 0; i < cantThreads;i++) {
        //Esto de abajo es asi porque hay un problema al hacer un thread dentro de una clase
        //y es que el thread para crearse hace uso del operador *this
        //por lo que necesitas hacerlo de esta manera, como si fuese un lambda,
        //porque si no se confunde con el del HashMapConcurrente
         threads.emplace_back(
        [this, &lista_actual, &resultados]() {
            this->calculoThread(lista_actual, mutex_por_bucket, tabla, &resultados);
        }
    );
    }

    for (auto &t: threads) {
        t.join();
    }

    float suma = 0.0;
    unsigned int contador = 0;

    for(auto res: resultados) {
        //CANTIDAD DE PRODUCTOS
        suma += res.second;
        //CANTIDAD DE TIPOS DE PRODUCTO
        contador += res.first;            
    }

    return (suma/contador);
}




#endif
