#include "linux.hpp"

using sm::Platform_Linux;

int Platform_Linux::setup() {
    return 0;
}

void Platform_Linux::finalize() {

}

void Platform_Linux::run(void (*fn)()) {
    while (true) {
        fn();
    }
}
