#pragma once
#include <glm/glm.hpp>

namespace Math {
    inline float inverseLerp(const float min, const float max, const float value) {
        // Avoid division by zero if min and max are the same
        if (std::abs(max - min) < 0.00001f) return 0.0f;
        return (value - min) / (max - min);
    }
}