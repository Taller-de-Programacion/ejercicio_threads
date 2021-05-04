// Una solución subóptima al problema del paso anterior es instanciar un mutex
// y pasárselo a cada hilo:

/* ************************************************************************* *
 * CRITICAL SECTIONS - CONTENCIÓN: yo también quiero mi mutex
 * ************************************************************************* */

#include <iostream>
#include <mutex>
#include <thread>

void redPrint(const char *redString, int times, std::mutex &mutex) {
    mutex.lock();
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
    }
    mutex.unlock();
}

void greenPrint(const char *greenString, int times, std::mutex &mutex) {
    mutex.lock();
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
    }
    mutex.unlock();
}

int main(int argc, char const *argv[]) {
    std::mutex sharedMutex;
    std::thread redThread(redPrint, "RED", 5, std::ref(sharedMutex));
    std::thread greenThread(greenPrint, "GREEN", 5, std::ref(sharedMutex));

    // You spawn a thread, you join a thread. Should I remove this line?
    // No, I'll repeat it with uppercase: YOU SPAWN A THREAD, YOU JOIN A THREAD
    greenThread.join();
    redThread.join();
}

// Ahora el problema es que las critical sections se pueden achicar, entonces
// los hilos esperan demasiado. Son "interbloqueados" durante demasiado tiempo, hay 
// demasiada "contención".
//
