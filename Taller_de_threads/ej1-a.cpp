#include <thread>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <mutex>

#define CANT_THREADS 5

using namespace std;

// g++ ej1-a.cpp -pthread --std=c++2a //comando para compilar

mutex m;               //mutex solo puede ser liberado por el que lo bloqueo, a diferencia del semaforo.


void f (int i) {
    lock_guard<mutex> lk(m);       //trata lo que sigue como zona critica. Trata como si cuando termina el proceso hay un unlock
    //m.lock();
    cout << "Hola! Soy el thread: " << i <<"\n";
    this_thread::sleep_for(100ms);
    cout << "Chau! Saludos desde: " << i <<"\n";
   // m.unlock();
}

int main() {
    vector<thread> v ;
    for (size_t i = 0; i < CANT_THREADS; i++){
        //new thread(f);
        //opcion1 b)
        v.emplace_back(f, i);
        //opcion2
        //v.detach();
    }
    for (auto &t: v){
        t.join();             //esperar a que los threads terminen
    }
    
    //sleep(1);
    return 0;
}