#include "FrameTimer.h"

#include <iostream>
#include <ostream>
#include <GLFW/glfw3.h>

FrameTimer::FrameTimer() : m_lastTime(glfwGetTime()), m_frameCount(0), m_deltaTime(0.0f) { }

void FrameTimer::update() {
    const double currentTime = glfwGetTime();
    m_deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;
    m_frameCount++;

    static double elapsedTime = 0.0;
    elapsedTime += m_deltaTime;

    if (elapsedTime >= 1.0) {
        if (m_frameCount > 0) {
            const double fps = m_frameCount;
            const double msPerFrame = 1000.0 / m_frameCount;
            std::cout << "fps: " << fps << " | ms/frame: " << msPerFrame << std::endl;
        }
        m_frameCount = 0;
        elapsedTime -= 1.0;
    }
}

float FrameTimer::getDeltaTime() const { return m_deltaTime; }
