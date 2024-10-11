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
    //fixed from original template
    return (unsigned int)(tolower(clave[0]) - 'a');
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
        tabla[index]->insertar(parNuevo); //ATOMICO

    } else {
        (*iteradorPrincipio).second++; // ATOMICO
    }

    mutex_por_letras[index].unlock();

}

std::vector<std::string> HashMapConcurrente::claves() {
    //todo preguntar como es el tema de la inanicion
    vector<string> result;


    //bloqueo todo
    //voy leyendo y mientras voy leyendo, voy desmuteando a medida que leo

    //TODO: Esto bloquea todo en un paso, pero despues vamos dejando
    //Ir bloqueando a medida que lees? -> No seria snapshot, pero es menos bloqueante
    //Tener una lista de claves, y manejar las concurrencias
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_por_letras[i].lock();
    }

    for (int i = 0; i < HashMapConcurrente::cantLetras; ++i) {

        for (const auto& nodo: *tabla[i]) {
            result.push_back(nodo.first);
        }
        mutex_por_letras[i].unlock();
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

    //bloqueo toda la tabla para la snapshot
    //TODO: Preguntar sobre la posiblidad de tener la cantidad como un num atomico y un vector de claves
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++){
        mutex_por_letras[i].lock();
    }

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        //para cada lista, sumo sus p's
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
        mutex_por_letras[index].unlock();
    }
    if (count > 0) {
        return sum / count;
    }
    return 0;        
}


void HashMapConcurrente::calculoThread(atomic<int> &listaActual, mutex *mutexes,  ListaAtomica<hashMapPair> **tabla_thread, vector<pair<int,int>> *resultados_thread) {

    for (int index = listaActual.fetch_add(1); index < HashMapConcurrente::cantLetras; index = listaActual.fetch_add(1)) {
        pair<int,int> par = make_pair(0,0);
        //aca estoy en una tabla
        for (auto &producto: *tabla_thread[index]) {
            //cantidad de tipos de productos
            par.first++;
            //cantidad de productos
            par.second+= producto.second;
        }
        mutexes[index].unlock();
        resultados_thread->at(index) = par;
    }
    
}

float HashMapConcurrente::promedioParalelo(unsigned int cantThreads){
    vector<thread> threads;
    atomic<int> lista_que_reviso(0);
    vector<pair<int,int>> resultados(cantLetras);

    for (int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        mutex_por_letras[i].lock();
    }

    for (int i = 0; i < cantThreads;i++) {
         threads.emplace_back(
        [this, &lista_que_reviso, &resultados]() {
            this->calculoThread(lista_que_reviso, mutex_por_letras, tabla, &resultados);
        }
    );
    }

    for (auto &t: threads) {
        t.join();
    }

    float sum = 0.0;
    unsigned int count = 0;

    for(auto res: resultados) {
        //CANTIDAD DE PRODUCTOS
        sum += res.second;
        //CANTIDAD DE TIPOS DE PRODUCTO
        count += res.first;
    }

    return (sum/count);

    // -> pasarles un indice de inicio, un indice de final
    // -> y que hagan sus cosas
}




#endif
