// Nunca está demás repasar lo mismo en C, ya que muchísimos lenguajes tienen
// "bindings", y toleran llamar a código C de alguna manera.

/* ************************************************************************* *
 * SPAWN - QUINTA TECNICA: wrapping C
 * ************************************************************************* */

#include <iostream>

void redPrint(const char *redString, int times) {
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
    }
}

void greenPrint(const char *greenString, int times) {
    for (int i = 0; i < times; ++i) {
        std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
    }
}

#include <pthread.h>

typedef struct {
    const char *string;
    int times;
} print_ctx_t;

static void *_red_print_adapter(void *ctx) {
    print_ctx_t *prt_ctx = (print_ctx_t*) ctx;
    redPrint(prt_ctx->string, prt_ctx->times);
    return NULL;
}

static void *_green_print_adapter(void *ctx) {
    print_ctx_t *prt_ctx = (print_ctx_t*) ctx;
    greenPrint(prt_ctx->string, prt_ctx->times);
    return NULL;
}

void usingCallbacks() {
    // la técnica de usar callbacks y punteros genéricos está muy presente en C,
    // no solamente en el ejemplo de la clase 2 de taller ;)
    pthread_t red_thread;
    print_ctx_t red_ctx;
    red_ctx.string = "RED";
    red_ctx.times = 5;
    pthread_create(&red_thread, NULL, _red_print_adapter, &red_ctx);

    pthread_t green_thread;
    print_ctx_t green_ctx;
    green_ctx.string = "GREEN";
    green_ctx.times = 5;
    pthread_create(&green_thread, NULL, _green_print_adapter, &green_ctx);

    // Se ve la simetría? init(a), init(b), uninit(b), uninit(a)
    // La simetría es la clave del éxito

    // You spawn a thread, you join a thread.
    pthread_join(green_thread, NULL);
    pthread_join(red_thread, NULL);
}

// Y qué pasaría si no tuviéramos std::thread? Fácil! Hacemos un wrapper de lo anterior!


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

// Y las clases que heredan implementan la misma interfaz!
class RedPrinterThread: public Thread {
private:
    const char *redString;
    int times;

protected:
    void run() override {
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[31m" << redString << "\033[0m" << std::endl;
        }
    }

public:
    RedPrinterThread(const char *redString, int times) :
        redString(redString), times(times) {
    }
};

class GreenPrinterThread: public Thread {
private:
    const char *greenString;
    int times;

protected:
    void run() override {
        for (int i = 0; i < times; ++i) {
            std::cout << "\x1B[32m" << greenString << "\033[0m" << std::endl;
        }
    }

public:
    GreenPrinterThread(const char *greenString, int times) :
        greenString(greenString), times(times) {
    }
};

int usingWrappers() {
    RedPrinterThread redPrinter("RED", 5);
    GreenPrinterThread greenPrinter("GREEN", 5);

    redPrinter.start();
    greenPrinter.start();

    // De nuevo, qué hace el thread principal??

    greenPrinter.join();
    redPrinter.join();
    return 0;
}


int main(int argc, char const *argv[]) {
    //usingCallbacks();
    usingWrappers();
    return 0;
}