#include "windows.hpp"

using sm::Platform_Windows;

int Platform_Windows::setup() {
    return 0;
}

void Platform_Windows::finalize() {

}

void Platform_Windows::run(void (*fn)()) {
    while (true) {
        fn();
    }
}
