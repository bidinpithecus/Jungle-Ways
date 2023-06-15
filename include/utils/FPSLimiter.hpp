#pragma once
#include <SDL2/SDL.h>

class FPSLimiter {
public:
	explicit FPSLimiter(int64_t frameTime = 30)
        : m_frameTime{ 1000 / frameTime }
        , m_startTime{ SDL_GetTicks() }
        , m_sleepTime{ 0 }
        , m_frameTimeDebt{ 0 }
    {
    }

    void run() {
        m_sleepTime = (m_frameTime - m_frameTimeDebt) - (SDL_GetTicks() - m_startTime);
        if (m_sleepTime > 0) {
            SDL_Delay(m_sleepTime);
        }
        m_frameTimeDebt = (SDL_GetTicks() - m_startTime) - (m_frameTime - m_frameTimeDebt);
        m_startTime     = SDL_GetTicks();
    }

  private:
    int64_t m_frameTime;
    int64_t m_startTime;
    int64_t m_sleepTime;
    int64_t m_frameTimeDebt;
};
