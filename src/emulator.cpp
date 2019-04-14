#include "emulator.h"

#include "cpu.h"
#include "debugger.h"
#include "gui.h"
#include "utils.h"

#include <exception>
#include <memory>
#include <stdexcept>

#include "spdlog/spdlog.h"

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QSettings>
#include <QTimer>

Emulator::Emulator(const std::shared_ptr<Mmu>& mmu, Cpu& cpu, const std::string& romFilename)
    : cpu(cpu)
    , mmu(mmu)
{
    auto bootstrapRom = readFile("../gbemu/res/bootstrap.bin");
    mmu->loadBootstrap(bootstrapRom);
    auto cartridgeRom = readFile(romFilename);
    mmu->loadCartridge(cartridgeRom);
    timer = new QTimer(this);
}

Emulator::~Emulator()
{
    delete timer;
}

std::vector<uint8_t> Emulator::getVram() const
{
    return mmu->getVram();
}

std::shared_ptr<Mmu> Emulator::getMmu() const
{
    return mmu;
}

void Emulator::executeInstruction()
{
    Instruction instr = disassemble(mmu->getMemory(), cpu.getPC());
    if (cpu.execute()) {
        spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
    } else {
        spdlog::info("{:04x} {:<10} {:<6} {:<13}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
    }
}

void Emulator::step()
{
    if (cpu.getPC() == breakpoint) {
        pause();
        emit executionPaused();
        return;
    }

    executeInstruction();
}

void Emulator::play()
{
    spdlog::info("Resuming execution.");
    executeInstruction();
    startLoop();
}

void Emulator::startLoop()
{
    QObject::connect(timer, &QTimer::timeout, this, &Emulator::step);
    timer->start(0);
}

void Emulator::pause()
{
    spdlog::info("Execution paused.");
    QObject::disconnect(timer, &QTimer::timeout, this, &Emulator::step);
    timer->stop();
}

void Emulator::breakpointSet(uint16_t pc)
{
    spdlog::info("Set breakpoint at pc=0x{:04x}.", pc);
    breakpoint = pc;
}

void Emulator::breakpointUnset()
{
    spdlog::info("Unset breakpoint.");
    breakpoint = {};
}

int runGui(int argc, char** argv)
{
    QApplication app(argc, argv);
    auto romFilename = argv[1];
    auto mmu = std::make_shared<Mmu>();
    Cpu cpu{ mmu };
    Emulator emulator{ mmu, cpu, romFilename };
    Gui gui{ emulator };
    Debugger debugger{ emulator, cpu, &gui };

    QObject::connect(&debugger, &Debugger::pauseClicked, &emulator, &Emulator::pause);
    QObject::connect(&debugger, &Debugger::playClicked, &emulator, &Emulator::play);
    QObject::connect(&debugger, &Debugger::stepClicked, &emulator, &Emulator::executeInstruction);
    QObject::connect(&debugger, &Debugger::breakpointSet, &emulator, &Emulator::breakpointSet);
    QObject::connect(&debugger, &Debugger::breakpointUnset, &emulator, &Emulator::breakpointUnset);
    QObject::connect(&emulator, &Emulator::executionPaused, &debugger, &Debugger::onExecutionPaused);

    debugger.show();
    gui.show();
    emulator.startLoop();
    return app.exec();
}

int main(int argc, char** argv)
{
    try {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%H:%M:%S] %v");
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        runGui(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return 1;
    }
}
