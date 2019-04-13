#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "mmu.h"

#include <QObject>
#include <QTimer>

class Emulator : public QObject {
    Q_OBJECT

public:
    Emulator(const std::shared_ptr<Mmu>& mmu, Cpu& cpu, const std::string& romFilename);
    virtual ~Emulator();
    std::vector<uint8_t> getVram() const;

public slots:
    // Execute a single CPU instruction.
    void step();

    // Execute CPU instructions in a loop.
    void play();

    // Pause execution.
    void pause();

private:
    Cpu& cpu;
    std::shared_ptr<Mmu> mmu;
    QTimer* timer;
};

#endif // EMULATOR_H
