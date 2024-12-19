#pragma once

class FrameTimer {
private:
    double m_lastTime;
    int m_frameCount;
    float m_deltaTime;
public:
    FrameTimer();
    void update();
    [[nodiscard]] float getDeltaTime() const;
};