#pragma once

#include <memory>
#include <vector>

namespace sm {
    class ErrorPanel {
        class IGenericError {
        public:
            virtual ~IGenericError() = default;
        };

        std::vector<std::unique_ptr<IGenericError>> mErrors;

    public:
        ErrorPanel() = default;

        void draw();

        template<typename E>
        void addError(E error);
    };
}
