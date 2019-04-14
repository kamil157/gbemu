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
    Emulator(const std::shared_ptr<Mmu>& mmu, Cpu& cpu, const std::string& romFilename);
    virtual ~Emulator();
    std::vector<uint8_t> getVram() const;
    std::shared_ptr<Mmu> getMmu() const;

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
    Cpu& cpu;
    std::shared_ptr<Mmu> mmu;
    QTimer* timer;
    std::optional<uint16_t> breakpoint;

    void step();
};

#endif // EMULATOR_H
