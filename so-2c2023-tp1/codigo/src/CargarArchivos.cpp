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
  std::atomic_init(&siguienteIndex, 0);

  auto cargarArchivoThread = [&hashMap, &filePaths, &siguienteIndex]() {
    int index;
    while ((index = siguienteIndex.fetch_add(1)) < filePaths.size()) {
      cargarArchivo(hashMap, filePaths[index]);
    }
  };

  for (int i = 0; i < cantThreads; i++) {
    threads.emplace_back(cargarArchivoThread);
  }

  for (auto &t : threads) {
    t.join();
  }
}

#endif
