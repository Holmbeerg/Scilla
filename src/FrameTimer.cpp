#include "FrameTimer.h"

#include <iostream>
#include <ostream>
#include <GLFW/glfw3.h>

FrameTimer::FrameTimer() : lastTime(glfwGetTime()), frameCount(0), deltaTime(0.0f) { }

void FrameTimer::update() {
    const double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    frameCount++;

    static double elapsedTime = 0.0;
    elapsedTime += deltaTime;

    if (elapsedTime >= 1.0) {
        if (frameCount > 0) {
            const double msPerFrame = (1000.0 / frameCount);
            std::cout << "ms/frame: " << msPerFrame << std::endl;
        }
        frameCount = 0;
        elapsedTime -= 1.0;
    }
}

float FrameTimer::getDeltaTime() const { return deltaTime; }
