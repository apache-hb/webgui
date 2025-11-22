#pragma once

#include "platform/generic.hpp"

namespace sm {
    struct Platform_Darwin : public Platform_Generic {
        static int setup(const PlatformCreateInfo& createInfo);
        static void finalize();
        static void run(void (*fn)());
        static bool begin();
        static void end();
        static void configureAwsSdkOptions(Aws::SDKOptions& options);
    };
}
