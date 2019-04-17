#include "timer.h"

Timer::Timer()
{
}

int Timer::getCycles() const
{
    return cycles;
};

void Timer::setCycles(int n)
{
    cycles = n;
};

void Timer::increment(int n)
{
    cycles += n;
}

void Timer::reset()
{
    cycles = 0;
}
