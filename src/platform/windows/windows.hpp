#pragma once

#include "platform/generic.hpp"

namespace sm {
    struct Platform_Windows : public Platform_Generic {
        static int setup();
        static void finalize();
        static void run(void (*fn)());
    };
}
