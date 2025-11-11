#pragma once

#include <string>
#include <array>

namespace Aws {
    struct SDKOptions;
}

namespace sm {
    struct PlatformCreateInfo {
        std::string title;
        std::array<float, 4> clear;
    };

    struct Platform_Generic {
        [[gnu::error("Platform::setup() Is not implemented for this platform")]]
        static int setup(const PlatformCreateInfo& createInfo);

        [[gnu::error("Platform::finalize() Is not implemented for this platform")]]
        static void finalize();

        [[gnu::error("Platform::run() Is not implemented for this platform")]]
        static void run(void (*fn)());

        [[gnu::error("Platform::begin() Is not implemented for this platform")]]
        static void begin();

        [[gnu::error("Platform::end() Is not implemented for this platform")]]
        static void end();

        static void configure_aws_sdk_options([[maybe_unused]] Aws::SDKOptions& options) {
            // Default no-op
        }
    };
}
