#include "darwin.hpp"

using sm::Platform_Darwin;

int Platform_Darwin::setup(const PlatformCreateInfo& createInfo) {
    return 0;
}

void Platform_Darwin::finalize() {

}

void Platform_Darwin::run(void (*fn)()) {
    while (true) {
        fn();
    }
}

bool Platform_Darwin::begin() {
    return true;
}

void Platform_Darwin::end() {

}

void Platform_Darwin::configureAwsSdkOptions(Aws::SDKOptions& options) {

}
