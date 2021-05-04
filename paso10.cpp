// En este código hay un posible "deadlock":
//

#include <iostream>
#include <mutex>
#include <thread>

int main(int argc, char const *argv[]) {
    std::mutex firstMutex;
    std::mutex secondMutex;

    std::thread firstThread([&] {
        firstMutex.lock();
        secondMutex.lock();

        std::cout << "First thread" << std::endl;

        secondMutex.unlock();
        firstMutex.unlock();
    });

    std::thread secondThread([&] {
        secondMutex.lock();
        firstMutex.lock();

        std::cout << "Second thread" << std::endl;

        firstMutex.unlock();
        secondMutex.unlock();
    });

    secondThread.join();
    firstThread.join();

    return 0;
}

// Este código puede funcionar! Pero qué pasa si se da el interleaving en el que firstThread llega
// a la línea 14 y secondThread a la 24? Cada uno tiene su mutex, y quiere obtener el que tiene el 
// otro.
// 
// Sospechamos que hay un deadlock pero no pasa siempre? Vamos a gdb y lo reproducimos!
// Cómo? Queremos ver qué pasa si llegamos a las líneas 14 y 24 en 2 threads distintos. Ponemos un
// breakpoint en cada una, y corremos el proceso!
// 
// De nuevo, a pensar y no frustrarse!!
//