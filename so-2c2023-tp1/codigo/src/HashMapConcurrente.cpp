#ifndef CHM_CPP
#define CHM_CPP

#include "HashMapConcurrente.hpp"

#include <fstream>
#include <iostream>
#include <thread>

HashMapConcurrente::HashMapConcurrente() {
  for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
    tabla[i] = new ListaAtomica<hashMapPair>();
  }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
  return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
  // Ej 2
  // Idea:
  //  Tenemos 26 semáforos mutex, uno por cada bucket
  //  Vemos a que bucket pertenece la clave
  //  Usamos el mutex para ese bucket
  //  Busqueda lineal:
  //      Si encontramos la clave, incrementamos su valor en 1.
  //      Si no, la insertamos en la lista.
  //  Liberamos el mutex correspondiente.
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

  if (!encontrado) {
    hashMapPair par(clave, 1);
    tabla[index]->insertar(par);
  }
}

std::vector<std::string> HashMapConcurrente::claves() {
  // Completar (Ejercicio 2)

  // Idea
  //  Bloqueamos toda la tabla usando el array de semaforos
  //  Una vez que terminamos de recorrer la iésima lista, liberamos el semáforo
  //  i y pasamos a la lista i+1
  //  Es una operación parcialmente bloqueante.

  // Bloqueamos la tabla
  for (int index = 0; index < HashMapConcurrente::cantLetras; index++) {
    mutexes[index].lock();
  }
  // Vector donde guardaremos las claves a devolver
  std::vector<std::string> claves;
  for (unsigned int index = 0; index < HashMapConcurrente::cantLetras;
       index++) {
    for (auto it = tabla[index]->begin(); it != tabla[index]->end(); ++it) {
      claves.push_back((*it).first);
    }
    // Liberamos el mutex de la lista actual y seguimos
    mutexes[index].unlock();
  }
  return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
  // Completar (Ejercicio 2)
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

  // Usamos la misma idea que en claves
  // Bloqueamos la tabla
  for (int index = 0; index < HashMapConcurrente::cantLetras; index++) {
    mutexes[index].lock();
  }

  for (unsigned int index = 0; index < HashMapConcurrente::cantLetras;
       index++) {
    for (auto &p : *tabla[index]) {
      if (p.second > max->second) {
        max->first = p.first;
        max->second = p.second;
      }
    }
    // Liberamos el mutex de la lista actual y seguimos
    mutexes[index].unlock();
  }

  return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
  // Completar (Ejercicio 3)
  std::vector<std::thread> threads;
  std::vector<hashMapPair> maximos(HashMapConcurrente::cantLetras,
                                   hashMapPair("", 0));

  std::atomic<int> siguienteIndex;
  std::atomic_init(&siguienteIndex, -1);

  // Función que procesa filas de la tabla en paralelo
  auto maximo_thread = [this, &maximos, &siguienteIndex]() {
    unsigned int index;
    while ((index = siguienteIndex.fetch_add(1)) <
           HashMapConcurrente::cantLetras) {
      for (auto &p : *tabla[index]) {
        if (p.second > maximos[index].second) {
          maximos[index] = p;
        }
      }
      // Liberamos el mutex de la lista actual
      this->mutexes[index].unlock();
    }
  };

  // Bloqueamos la tabla
  for (int index = 0; index < HashMapConcurrente::cantLetras; index++) {
    mutexes[index].lock();
  }

  for (size_t i = 0; i < cant_threads; i++) {
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
