#include <thread>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <semaphore.h>

#define CANT_THREADS 2
#define MSG_COUNT 5

using namespace std;

sem_t semf1, semf2;


void f1_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (A)\n";
        this_thread::sleep_for(100ms);
    }
}

void f1_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F1 (B)\n";
        this_thread::sleep_for(200ms);
    }
}

void f2_a() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (A)\n";
        this_thread::sleep_for(500ms);
    }
}

void f2_b() {
    for (int i = 0; i < MSG_COUNT; ++i) {
        cout << "Ejecutando F2 (B)\n";
        this_thread::sleep_for(50ms);
    }
}

void f1() {
    f1_a();
    sem_post(&semf1);
    sem_wait(&semf2);
    f1_b();
}

void f2() {
    f2_a();
    sem_post(&semf2);
    sem_wait(&semf1);
    f2_b();
}

int main() {
    sem_init(&semf1, 0, 0);      
    sem_init(&semf2, 0, 0);
    vector<thread> v ;

    v.emplace_back(f1);
    v.emplace_back(f2);

    for (auto &t: v){
        t.join();
    }
    
    return 0;
}