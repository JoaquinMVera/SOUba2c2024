#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"
using namespace std;

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    float promedioParalelo(unsigned int cantThreads);
    float promedio();



 private:
     void calculoThread(atomic<int> &listaActual, mutex *mutexes,  ListaAtomica<hashMapPair> **tabla_thread, vector<pair<int,int>> *resultados_thread);
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    
    //Los mutex para cada bucket!
    mutex mutex_por_bucket[HashMapConcurrente::cantLetras];
    static unsigned int hashIndex(std::string clave);
};

#endif  /* HMC_HPP */
