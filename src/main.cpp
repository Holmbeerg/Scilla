#include "core/Engine.h"

int main() {
    if (const auto engine = std::make_unique<Engine>(); engine->initialize(1280, 960, "Scilla Engine")) {
        engine->run();
    }
    Engine::shutdown();
    return 0;
}