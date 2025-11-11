#pragma once

#if defined(__EMSCRIPTEN__)
#   include "emscripten/emscripten.hpp"
namespace sm {
    using Platform = sm::Platform_Emscripten;
}
#elif defined(__APPLE__)
#   include "darwin/darwin.hpp"
namespace sm {
    using Platform = sm::Platform_Darwin;
}
#elif defined(_WIN32)
#   include "windows/windows.hpp"
namespace sm {
    using Platform = sm::Platform_Windows;
}
#elif defined(__linux__)
#   include "linux/linux.hpp"
namespace sm {
    using Platform = sm::Platform_Linux;
}
#else
#   error "Unsupported platform"
#endif
