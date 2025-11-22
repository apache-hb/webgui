#include "windows.hpp"

using sm::Platform_Windows;

int Platform_Windows::setup(const PlatformCreateInfo& createInfo) {
    return 0;
}

void Platform_Windows::finalize() {

}

void Platform_Windows::run(void (*fn)()) {
    while (true) {
        fn();
    }
}

bool Platform_Windows::begin() {
    return true;
}

void Platform_Windows::end() {

}

void Platform_Windows::configureAwsSdkOptions(Aws::SDKOptions& options) {

}
