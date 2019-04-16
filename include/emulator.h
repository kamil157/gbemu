#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "mmu.h"

#include <optional>

#include <QObject>
#include <QTimer>

class Emulator : public QObject {
    Q_OBJECT

public:
    Emulator(const std::string& romFilename);
    virtual ~Emulator();

    const Mmu &getMmu() const { return mmu; }
    Mmu& getMmu() { return mmu; }

    const Cpu& getCpu() const { return cpu; }
    Cpu& getCpu() { return cpu; }

    // Start execution.
    void startLoop();

public slots:
    // Execute a single CPU instruction.
    void executeInstruction();

    // Execute CPU instructions in a loop.
    void play();

    // Pause execution.
    void pause();

    // Set breakpoint at PC.
    void breakpointSet(uint16_t pc);

    // Unset current breakpoint.
    void breakpointUnset();

signals:
    void executionPaused();

private:
    Mmu mmu;
    Cpu cpu;
    QTimer* timer;
    std::optional<uint16_t> breakpoint;

    void step();
};

#endif // EMULATOR_H
