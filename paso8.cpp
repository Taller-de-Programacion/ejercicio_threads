// Cambiemos de ejemplo: Vamos al clásico "mapa protegido" y a un diseño autodestructivo.
// 
// Extraigamos la interfaz de un objeto mapa y deleguemos comportamiento en él.

#include <iostream>
#include <map>
#include <mutex>
#include <thread>

class ProtectedMap {
private:
    std::map<int, int> internal;
    std::mutex mutex;

public:
    // Si bien hay más métodos, estos tres representan una interfaz "natural" de diccionario.

    void put(int key, int value) {
        std::lock_guard<std::mutex> lock(mutex);
        internal[key] = value;
    }
    int get(int key) {
        std::lock_guard<std::mutex> lock(mutex);
        return internal.at(key);
    }
    bool contains(int key) {
        std::lock_guard<std::mutex> lock(mutex);
        return internal.find(key) != internal.end();
    }
    void remove(int key) {
        std::lock_guard<std::mutex> lock(mutex);
        internal.erase(key);
    }
};


/* ************************************************************************* *
 * CRITICAL SECTIONS - Mala interfaz de un Monitor
 * ************************************************************************* */

void usingTheWeakMonitor() {
    ProtectedMap map;
    // Llenamos el mapa...
    for (int key = 0; key < 100; ++key) {
        map.put(key, key);
    }

    // Este thread va a intentar remover todas las entradas del mapa.
    std::thread remover_thread([&] {
        for (int key = 0; key < 100; ++key) {
            if (map.contains(key)) {
                map.remove(key);
            }
        }
    });

    // En este segundo thread, vamos a intentar "rescatar" algunos pares (key, value)
    std::thread printer_thread([&] {
        for (int key = 99; key >= 0; --key) {
            if (map.contains(key)) {
                std::cout << "Par rescatado! (" << key << ", " << map.get(key) << ")" << std::endl;
            }
        }
    });

    printer_thread.join();
    remover_thread.join();
}

// El problema de esta aproximación, es que nuestras critical sections 
// deberían ser del estilo "removeIfPresent" y "logIfPresent"
// 
// Una opción que introduce deuda técnica sería complementar con locks desde el lugar
// del uso del monitor

void complementWithLocks() {
    ProtectedMap map;
    for (int key = 0; key < 100; ++key) {
        map.put(key, key);
    }

    // Por supuesto, el mutex es compartido:
    std::mutex shared_mutex;

    std::thread remover_thread([&] {
        for (char key = 0; key < 100; ++key) {
            // Delimitamos las critical-sections que detectamos.
            std::lock_guard<std::mutex> lock(shared_mutex);
            if (map.contains(key)) {
                map.remove(key);
            }
        }
    });

    std::thread printer_thread([&] {
        for (char key = 99; key >= 0; --key) {
            std::lock_guard<std::mutex> lock(shared_mutex);
            if (map.contains(key)) {
                std::cout << "Par rescatado! (" << key << ", " << map.get(key) << ")" << std::endl;
            }
        }
    });

    printer_thread.join();
    remover_thread.join();
}


// Esta segunda opción nos da siempre un resultado válido, pero para qué nos sirvió
// ese Monitor?
// 
// Un buen Monitor tiene como interfaz las critical sections detectadas:
// 
class MapMonitor {
private:
    std::map<int, int> internal;
    std::mutex mutex;

    bool contains(int key) {
        return internal.find(key) != internal.end();
    }

public:
    // Un buen monitor nos encapsula LAS CRITICAL SECTIONS!!

    void putIfAbsent(int key, int value) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!contains(key)) {
            internal[key] = value;
        }
    }
    void printIfPresent(int key) {
        std::lock_guard<std::mutex> lock(mutex);
        if (contains(key)) {
            std::cout << "Par rescatado! (" << (int) key << ", " << internal.at(key) << ")" << std::endl;
        }
    }
    void removeIfPresent(int key) {
        std::lock_guard<std::mutex> lock(mutex);
        if (contains(key)) {
            internal.erase(key);
        }
    }
};

void usingTheGoodMonitor() {
    MapMonitor map;
    for (int key = 0; key < 100; ++key) {
        map.putIfAbsent(key, key);
    }

    // Ahora que el Monitor es thread-safe, lo podemos bombardear tranquilos 
    // que no vamos a romper un invariante, sin ver desde afuera los locks.
    std::thread remover_thread([&] {
        for (int key = 0; key < 100; ++key) {
            map.removeIfPresent(key);
        }
    });

    std::thread printer_thread([&] {
        for (int key = 99; key >= 0; --key) {
            map.printIfPresent(key);
        }
    });

    printer_thread.join();
    remover_thread.join();
}


int main(int argc, char const *argv[]) {
    usingTheWeakMonitor();
    // complementWithLocks();
    // usingTheGoodMonitor();
    return 0;
}

// A tener en cuenta:
// 1. Hay que tener muy claro cuáles son las critical sections al acceder a un recurso
// 2. Crear una clase Monitor cuyos métodos son esas critical sections y manejan la 
//    sincronización
// 3. El Monitor tiene que poder ser "bombardeado" sin romper invariantes
// 4. Un Monitor que simplemente le mete un lock a los métodos inseguros no va a tener 
//    buenos resultados (puede que sí, pero sería mera coincidencia)
//