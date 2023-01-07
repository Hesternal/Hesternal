module HesternalEngine.Utils.Time;

import <chrono>;


namespace Hesternal
{

    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<float64>;


    void Time::Init()
    {
        m_currentTime = std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch()).count();
    }

    void Time::Update()
    {
        const float64 now = std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch()).count();
        m_deltaTime = now - m_currentTime;
        m_currentTime = now;
    }

} // namespace Hesternal
