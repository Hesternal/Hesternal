export module CopiumEngine.Utils.Time;

import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    class Time final
    {
    public:
        static void Init();
        static void Update();

        [[nodiscard]] static float64 GetCurrent() noexcept { return m_currentTime; }
        [[nodiscard]] static float64 GetDelta() noexcept { return m_deltaTime; }

    private:
        static inline float64 m_currentTime = 0;
        static inline float64 m_deltaTime   = 0;
    };

} // export namespace Copium
