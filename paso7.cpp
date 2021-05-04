// Y RAII, dónde está RAII? Y si hay un error en el medio de la critical 
// section? No se libera el mutex, y eso resulta en un mutex no liberado,
// un deadlock, un tp desaprobado.

/* ************************************************************************* *
 * CRITICAL SECTIONS - RAII: Implementemos nuestro "Lock" RAII
 * ************************************************************************* */

#include <iostream>
#include <mutex>
#include <thread>

class Lock {
private:
    std::mutex &mutex;

public:
    Lock(std::mutex &mutex) : mutex(mutex) {
        mutex.lock();
    }

    ~Lock() {
        mutex.unlock();
    }
};

void redPrint(const char *redString, int times, std::mutex *mutex) {
    for (int i = 0; i < times; ++i) {
        Lock lock(*mutex);
        std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
    } // <---- En cada iteración del for se llama al destructor del lock y se
      //       libera el mutex.
}

void greenPrint(const char *greenString, int times, std::mutex *mutex) {
    for (int i = 0; i < times; ++i) {

        // Esto es un scope sin nombre, y es sintaxis válida de C++
        {
            Lock lock(*mutex);
            std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
        } // <---- Ahí entra el destructor de Lock y libera el mutex. Programar 
          //       TODO con el idiom RAII es C++
    }
}

void yellowPrint(const char *yellowString, int times, std::mutex *mutex) {
    for (int i = 0; i < times; ++i) {
        // Desde C++11, tenemos la clase RAII implementada, y no necesitamos
        // escribirla nosotros.
        const std::lock_guard<std::mutex> lock(*mutex);
        std::cout << "\x1B[33m" << yellowString << "\033[0m" << std::endl;
    }
}

int main(int argc, char const *argv[]) {
    std::mutex mutex;
    std::thread redThread(redPrint, "RED", 5, &mutex);
    std::thread greenThread(greenPrint, "GREEN", 5, &mutex);
    std::thread yellowThread(yellowPrint, "YELLOW", 5, &mutex);

    // REALLY: YOU SPAWN A THREAD, YOU JOIN A THREAD. 
    yellowThread.join();
    greenThread.join();
    redThread.join();
    // Existe el método detach(). Si leés la implementación dice que te 
    // desaprueban los TPs de taller cuando lo usás.
}
