#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {

    unsigned int indice = hashIndex(clave);
    ListaAtomica<hashMapPair> lista_clave = tabla[indice];

    //Ver en que bucket pertence la clave, y esperar ese semaphore
    unsigned int index = hashIndex(clave);
    std::lock_guard<std::mutex> lock(mutexes[index]);

    bool encontrado = false; 
    for (auto it = tabla[index]->begin(); it != tabla[index]->end(); ++it) {
        if ((*it).first == clave) {
            // La clave existe, incrementamos el valor
            (*it).second++;
            encontrado = true;
            break;
        }
    }

    //Si no se encuentro, se añade a tabla
    if (!encontrado) {
        hashMapPair par(clave, 1);
        tabla[index]->insertar(par);
    }

}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2), (ver que cumpla concurrencia)
    std::vector<std::string> claves;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto it = tabla[index]->begin(); it != tabla[index]->end(); ++it) {
            claves.push_back((*it).first);
        }
    }
    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2). (Ver race condition)
    unsigned int index = hashIndex(clave);

    for (auto it = tabla[index]->begin(); it != tabla[index]->end(); ++it) {
        if ((*it).first == clave) {
        return (*it).second;
    }
}

  return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    unsigned int index = hashIndex(clave);
    for(int i = 0; i < cantLetras;)

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }

    return *max;
}



hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
  // Completar (Ejercicio 3)
  std::vector<std::thread> threads;
  std::vector<hashMapPair> maximos(HashMapConcurrente::cantLetras);
  // Inicializar maximos con pares de clave-valor vacíos
  for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
    maximos[i] = hashMapPair("", 0);
  }

  std::atomic<int> siguienteIndex(-1);

  // Función que procesa filas de la tabla en paralelo
  auto maximo_thread = [this, &maximos, &siguienteIndex]() {
    unsigned int index;
    while ((index = siguienteIndex.fetch_add(1)) < HashMapConcurrente::cantLetras) { //decide que lista atiende
      for (auto &p : *tabla[index]) { //recorre los pares de la lista asignada a el
        if (p.second > maximos[index].second) {
          maximos[index] = p;
        }
      }
    }
  };

  for (size_t i = 0; i < cantLetras; i++) {
    threads.emplace_back(maximo_thread);
  }

  for (auto &t : threads) {
    t.join();
  }

  // Encontrar el máximo entre todos los máximos
  hashMapPair max = maximos[0];
  for (unsigned int i = 1; i < cantLetras; i++) {
    if (maximos[i].second > max.second) {
      max = maximos[i];
    }
  }

  return max;
}

#endif
