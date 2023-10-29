#ifndef CHM_CPP
#define CHM_CPP

#include "CargarArchivos.hpp"

#include <atomic>
#include <fstream>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath) {
  std::fstream file;
  int cant = 0;
  std::string palabraActual;

  // Abro el archivo.
  file.open(filePath, file.in);
  if (!file.is_open()) {
    std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
    return -1;
  }
  while (file >> palabraActual) {
    // Completar (Ejercicio 4)
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

void cargarMultiplesArchivos(HashMapConcurrente &hashMap,
                             unsigned int cantThreads,
                             std::vector<std::string> filePaths) {
  std::vector<std::thread> threads;

  std::atomic<int> siguienteIndex;
  std::atomic_init(&siguienteIndex, -1);
  std::mutex mutex;

  // auto cargarArchivoThread = [&hashMap, &filePaths, &siguienteIndex]() {
  //   unsigned int index;
  //   while ((index = siguienteIndex.fetch_add(1)) < filePaths.size()) {
  //     cargarArchivo(hashMap, filePaths[index]);
  //   }
  // };

  for (unsigned int i = 0; i < cantThreads; i++) {
    threads.emplace_back([&hashMap, &filePaths, &siguienteIndex, &mutex]() {
      int index;
      while (true) {
        mutex.lock();
        index = siguienteIndex.fetch_add(1);
        mutex.unlock();
        if (index < filePaths.size()) {
          cargarArchivo(hashMap, filePaths[index]);
        } else {
          break;
        }
      }
    });
  }

  // std::queue<std::string> filesQueue;
  // for (const auto &filePath : filePaths) {
  //   filesQueue.push(filePath);
  // }
  // std::mutex filesQueueMutex;

  // for (unsigned int i = 0; i < cantThreads; i++) {
  //   threads.emplace_back([&hashMap, &filesQueue, &filesQueueMutex]() {
  //     while (!filesQueue.empty()) {
  //       filesQueueMutex.lock();
  //       if (filesQueue.empty()) break;
  //       auto filePath = filesQueue.front();
  //       filesQueue.pop();
  //       filesQueueMutex.unlock();
  //       cargarArchivo(hashMap, filePath);
  //     }
  //   });
  // }

  for (auto &t : threads) {
    t.join();
  }
}

#endif
