/* ************************************************************************* *
 * SPAWN - CUARTA TECNICA: template method (The Java way)
 * ************************************************************************* */

#include <iostream>
#include <thread>

/**
 * @brief      Abstract class wrapping a thread. It catches exceptions in order
 *             to avoid crashes with confusing messages.
 *             
 *             User should extend this class and implement the run() method with
 *             the behavior to be executed in a new thread.
 */
class Thread {
private:
    std::thread t;

    // This template method catches exceptions (they can't be caught from main)
    void runExpecting() {
        try {
            run();
        } catch (const std::exception &e) {
            std::cerr << "Exception caught in a thread: '" << e.what() << "'" << std::endl;
        } catch (...) {
            std::cerr << "Unknown error caught in thread" << std::endl;
        }
    }

protected:
    virtual void run() = 0;

public:
    /**
     * @brief      Starts a thread with the behavior of the method run().
     */
    void start() {
        t = std::thread(&Thread::runExpecting, this);
    }

    /**
     * @brief      Waits the thread to finish.
     */
    void join() {
        t.join();
    }

    virtual ~Thread() = default;
};

// Ahora nuestros threads son objetos que heredan de Thread e implementan run().
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

int main(int argc, char const *argv[]) {
    RedPrinterThread redPrinter("RED", 5);
    GreenPrinterThread greenPrinter("GREEN", 5);

    redPrinter.start();
    greenPrinter.start();

    // De nuevo, qué hace el thread principal??

    greenPrinter.join();
    redPrinter.join();
    return 0;
}
