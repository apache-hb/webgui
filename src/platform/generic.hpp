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

    enum class PlatformType {
        Emscripten,
        Linux,
        Windows,
        Darwin,
        Unknown
    };

    constexpr bool isDesktopPlatform(PlatformType type) {
        return type == PlatformType::Linux ||
               type == PlatformType::Windows ||
               type == PlatformType::Darwin;
    }

    constexpr bool isWebPlatform(PlatformType type) {
        return type == PlatformType::Emscripten;
    }

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

        static void configureAwsSdkOptions([[maybe_unused]] Aws::SDKOptions& options) {
            // Default no-op
        }

        [[gnu::error("Platform::type() Is not implemented for this platform")]]
        static consteval PlatformType type();
    };
}
