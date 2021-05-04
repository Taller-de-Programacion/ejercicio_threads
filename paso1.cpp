// Este ejercicio ilustra los temas vistos en la clase, con implementaciones en C++
//
// La propuesta en este paso es ejecutar dos "funciones" en paralelo en un mismo proceso.
//

#include <iostream>

/**
 * @brief      Prints the parameter in the console using the color red.
 *
 * @param[in]  redString  The red string
 * @param[in]  times Determines how many times should the string be shown
 */
void redPrint(const char *redString, int times) {
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
    }
}

/**
 * @brief      Prints the parameter in the console using the color green.
 *
 * @param[in]  greenString  The green string
 * @param[in]  times Determines how many times should the string be shown
 */
void greenPrint(const char *greenString, int times) {
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
    }
}

/**
 * @brief      Executes the redPrint and greenPrint functions sequentially.
 * 
 * @param[in]  times Determines how many times should each string be shown
 */
void sequential(int times) {
    redPrint("This string should be red", times);
    greenPrint("This string should be green", times);
}

#include <thread>

/* ************************************************************************* *
 * SPAWN - PRIMERA TECNICA: puntero a función en el constructor de std::thread
 * ************************************************************************* */

/**
 * @brief      Executes both functions, but now in parallel.
 * 
 * @param[in]  times Determines how many times should each string be shown
 */
void parallel(int times) {
    // Spawn two threads.
    std::thread redThread(redPrint, "RED", times);
    std::thread greenThread(greenPrint, "GREEN", times);

    // What's this thread doing??

    // You spawn a thread, you join a thread.
    greenThread.join();
    redThread.join();
}

int main(int argc, char const *argv[]) {
    // Por defecto, este programa imprime una vez en cada color.
    sequential(1);

    // Si habilitamos esta segunda línea, el programa va a imprimir una vez en 
    // cada color, pero cada impresión se va a ejecutar en un thread 
    // independiente.
    //parallel(1);
    
    // Esta última línea imprime 5 veces cada string, y está en este ejemplo
    // para mostrar que std::cout no es "del todo" thread-safe! Qué significan
    // esas comillas??
    //parallel(5);
    return 0;
}
