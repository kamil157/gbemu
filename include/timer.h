#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    Timer();

    int getCycles() const;
    void setCycles(int n);
    void increment(int n);
    void reset();

private:
    int cycles = 0;
};

#endif // TIMER_H
