#pragma once

class FrameTimer {
private:
    double lastTime;
    int frameCount;
    float deltaTime;
public:
    FrameTimer();
    void update();
    [[nodiscard]] float getDeltaTime() const;
};