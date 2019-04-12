#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu.h"
#include "mmu.h"

#include <QObject>

class Emulator : public QObject {
    Q_OBJECT

public:
    Emulator(const std::shared_ptr<Mmu>& mmu, Cpu& cpu, const std::string& romFilename);

    std::vector<uint8_t> getVram() const;

public slots:
    // Run CPU commands in a loop.
    void run();

signals:
    void next();

private:
    Cpu& cpu;
    std::shared_ptr<Mmu> mmu;
};

#endif // EMULATOR_H
