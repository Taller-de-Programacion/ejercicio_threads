// Y por último, nos falta ver cómo usar esos mutex en C. Vamos directo al wrapper.

#include "pthread.h"

class Mutex {
private:
    pthread_mutex_t c_mutex;

public:
    Mutex() {
        pthread_mutex_init(&c_mutex, NULL);
    }

    void lock() {
        pthread_mutex_lock(&c_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&c_mutex);
    }

    ~Mutex() {
        pthread_mutex_destroy(&c_mutex);
    }
};

// Y por qué no usar RAII?
class Lock {
private:
    Mutex &mutex;

public:
    Lock(Mutex &mutex) : mutex(mutex) {
        mutex.lock();
    }

    ~Lock() {
        mutex.unlock();
    }    
};

#include <iostream>

// Esta clase es la misma que la del ejemplo de C
class Thread {
private:
    pthread_t t;

    static void *runExpecting(void *self) {
        try {
            ((Thread*) self)->run();
        } catch (const std::exception &e) {
            std::cerr << "Exception caught in a thread: '" << e.what() << "'" << std::endl;
        } catch (...) {
            std::cerr << "Unknown error caught in thread" << std::endl;
        }
        return NULL;
    }

protected:
    virtual void run() = 0;

public:
    void start() {
        pthread_create(&t, NULL, &Thread::runExpecting, this);
    }

    void join() {
        pthread_join(t, NULL);
    }

    virtual ~Thread() = default;
};

// Y ahora falta que cada thread reciba un mutex (o bien implementar un Monitor)
class RedPrinterThread: public Thread {
private:
    const char *redString;
    int times;
    Mutex &shared_mutex;

protected:
    void run() override {
        for (int i = 0; i < times; ++i) {
            Lock lock(shared_mutex);
            std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
        }
    }

public:
    RedPrinterThread(const char *redString, int times, Mutex &shared_mutex) :
        redString(redString), times(times), shared_mutex(shared_mutex) {
    }
};

class GreenPrinterThread: public Thread {
private:
    const char *greenString;
    int times;
    Mutex &shared_mutex;

protected:
    void run() override {
        for (int i = 0; i < times; ++i) {
            Lock lock(shared_mutex);
            std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
        }
    }

public:
    GreenPrinterThread(const char *greenString, int times, Mutex &shared_mutex) :
        greenString(greenString), times(times), shared_mutex(shared_mutex) {
    }
};

// Y el código de alto nivel queda igual!!
int usingAllWrappers() {
    Mutex shared_mutex;

    RedPrinterThread redPrinter("RED", 5, shared_mutex);
    GreenPrinterThread greenPrinter("GREEN", 5, shared_mutex);

    redPrinter.start();
    greenPrinter.start();


    greenPrinter.join();
    redPrinter.join();
    return 0;
}

int main(int argc, char const *argv[]) {
    usingAllWrappers();
    return 0;
}

// Qué le falta a esto? std::thread es movible! Entonces nuestras clases Thread deberían serlo también.
// Y de paso no ser copiables, porque qué es copiar un thread?
//

// Y debuggear esto? Para probar esto con gdb, probá levantarlo con un breakpoint en cada uso de cout,
// y considerá usar los comandos "info threads", "thread N"
// 
// En un escenario concurrente en gdb, que un hilo se suspenda en un breakpoint hace que todos los 
// threads se suspendan donde están, y un next en el hilo actual hace un continue en los demás!
// 
// Qué pasa si, con los breakpoints planteados, le damos continue repetidas veces? Y si le damos next
// repetidas veces?
// Podemos reproducir dos "interleavings" distintos! Estas técnicas de depuración son bastante
// avanzadas y requieren mucha atención. A no frustrarse y a usar el ingenio!
//
