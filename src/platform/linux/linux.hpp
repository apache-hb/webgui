#pragma once

#include "platform/generic.hpp"

namespace sm {
    struct Platform_Linux : public Platform_Generic {
        static int setup();
        static void finalize();
        static void run(void (*fn)());
    };
}
