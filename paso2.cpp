
#include <iostream>

class RedPrinterFunctor {
public:
    // ¿Por qué sería autodestructivo un Command con un "run" que recibe parámetros? 
    void operator () (const char *redString, int times) {
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
        }
    }
};

class GreenPrinterFunctor {
private:
    const char *greenString;
    int times;

public:
    GreenPrinterFunctor(const char *greenString, int times) : 
        greenString(greenString), times(times) {
    }

    void operator () () {
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
        }
    }
};

/* ************************************************************************* *
 * SPAWN - SEGUNDA TECNICA: functor en el constructor de std::thread
 * ************************************************************************* */

#include <thread>

void parallelWithFunctors(int times) {
    RedPrinterFunctor redPrinter;
    GreenPrinterFunctor greenPrinter("GREEN", times);

    // Spawn two threads.
    std::thread redThread(redPrinter, "RED", times);
    std::thread greenThread(greenPrinter);

    // Ey, es acaso esto LO MISMO que el paso anterior?
    // La cuestión es que el primer parámetro del constructor de std::thread 
    // es algo "llamable", vamos a charlar más adelante en la materia de este 
    // concepto.

    // You spawn a thread, you join a thread.
    greenThread.join();
    redThread.join();
}

/* ************************************************************************* *
 * SPAWN - TERCERA TECNICA: lambda en el constructor de std::thread
 * ************************************************************************* */

void parallelWithLambda(int times) {
    // Las lambdas TAMBIÉN son "llamables", entonces las podemos usar en el
    // constructor de std::thread.

    // Spawn two threads.
    std::thread redThread([&] {
        const char *redString = "RED";
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
        }
    });
    std::thread greenThread([&] {
        const char *greenString = "GREEN";
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
        }
    });

    // You spawn a thread, you join a thread.
    greenThread.join();
    redThread.join();
}

int main(int argc, char const *argv[]) {
    parallelWithFunctors(5);
    //parallelWithLambda(5);
    return 0;
}
