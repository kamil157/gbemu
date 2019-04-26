#include "emulator.h"

#include "debugger.h"
#include "screen.h"
#include "utils.h"
#include "vramview.h"

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

Emulator::Emulator(const std::string& romFilename)
    : mmu()
    , timer()
    , cpu(mmu, timer)
    , gpu(mmu, timer)
{
    auto bootstrapRom = readFile("../gbemu/res/bootstrap.bin");
    mmu.loadBootstrap(bootstrapRom);
    auto cartridgeRom = readFile(romFilename);
    mmu.loadCartridge(cartridgeRom);
    qtimer = new QTimer(this);
}

Emulator::~Emulator()
{
    delete qtimer;
}

void Emulator::emulateFrame()
{
    bool isFrameFinished = false;
    while (!isFrameFinished) {
        if (cpu.getPC() == breakpoint) {
            pause();
            return;
        }
        isFrameFinished = executeInstruction();
    }
    emit frameFinished();
}

bool Emulator::executeInstruction()
{
    auto previousPC = cpu.getPC();
    if (cpu.execute()) {
        if (spdlog::default_logger()->level() <= spdlog::level::trace) {
            Instruction instr = disassemble(mmu.getMemory(), previousPC);
            spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
        }
    } else {
        Instruction instr = disassemble(mmu.getMemory(), previousPC);
        spdlog::info("{:04x} {:<10} {:<6} {:<13}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
        pause();
        return true;
    }
    return gpu.step();
}
void Emulator::play()
{
    spdlog::info("Starting emulation.");
    QObject::connect(qtimer, &QTimer::timeout, this, &Emulator::emulateFrame);
    qtimer->start(1000 / 60);
    emit emulationResumed();
}

void Emulator::pause()
{
    spdlog::info("Emulation paused.");
    QObject::disconnect(qtimer, &QTimer::timeout, this, &Emulator::emulateFrame);
    qtimer->stop();
    emit emulationPaused();
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

    Emulator emulator{ romFilename };
    Screen screen{ emulator };
    Debugger debugger{ emulator, &screen };
    VramView vramView{ emulator, &screen };

    QObject::connect(&debugger, &Debugger::pauseClicked, &emulator, &Emulator::pause);
    QObject::connect(&debugger, &Debugger::playClicked, &emulator, &Emulator::play);
    QObject::connect(&debugger, &Debugger::stepClicked, &emulator, &Emulator::executeInstruction);
    QObject::connect(&debugger, &Debugger::stepFrameClicked, &emulator, &Emulator::emulateFrame);
    QObject::connect(&debugger, &Debugger::breakpointSet, &emulator, &Emulator::breakpointSet);
    QObject::connect(&debugger, &Debugger::breakpointUnset, &emulator, &Emulator::breakpointUnset);

    QObject::connect(&emulator, &Emulator::emulationPaused, &debugger, &Debugger::onEmulationPaused);
    QObject::connect(&emulator, &Emulator::emulationResumed, &debugger, &Debugger::onEmulationResumed);

    QObject::connect(&emulator, &Emulator::frameFinished, &screen, &Screen::redraw);
    QObject::connect(&emulator, &Emulator::frameFinished, &vramView, &VramView::redraw);
    screen.redraw();
    vramView.redraw();

    debugger.show();
    screen.show();
    vramView.show();
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
