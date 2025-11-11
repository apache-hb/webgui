#include "darwin.hpp"

using sm::Platform_Darwin;

int Platform_Darwin::setup() {
    return 0;
}

void Platform_Darwin::finalize() {

}

void Platform_Darwin::run(void (*fn)()) {
    while (true) {
        fn();
    }
}
