// Vimos que la salida de los pasos anteriores tenía problemas, y hablamos
// sobre falta de sincronización.

/* ************************************************************************* *
 * CRITICAL SECTIONS - INTRODUCCIÓN: yo también quiero mi mutex
 * ************************************************************************* */

#include <iostream>
#include <mutex>
#include <thread>

void redPrint(const char *redString, int times) {
    std::mutex mutex;
    mutex.lock();
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
    }
    mutex.unlock();
}

void greenPrint(const char *greenString, int times) {
    std::mutex mutex;
    mutex.lock();
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
    }
    mutex.unlock();
}

int main(int argc, char const *argv[]) {
    std::thread redThread(redPrint, "RED", 5);
    std::thread greenThread(greenPrint, "GREEN", 5);

    // You spawn a thread, you join a thread.
    greenThread.join();
    redThread.join();
}

// ¿Qué está PEOR acá?
//
// Cada función usa un mutex, entonces no se interbloquean, no se excluyen!
//
