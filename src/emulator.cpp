#include "emulator.h"

#include "cpu.h"
#include "debugger.h"
#include "utils.h"

#include <exception>
#include <memory>
#include <stdexcept>

#include "spdlog/spdlog.h"

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QTimer>

#include <QObject>

Emulator::Emulator(const std::shared_ptr<Mmu>& mmu, Cpu& cpu, const std::string& romFilename)
    : cpu(cpu)
    , mmu(mmu)
{
    auto bootstrapRom = readFile("../gbemu/res/bootstrap.bin");
    mmu->loadBootstrap(bootstrapRom);
    auto cartridgeRom = readFile(romFilename);
    mmu->loadCartridge(cartridgeRom);
}

std::vector<uint8_t> Emulator::getVram() const
{
    return mmu->getVram();
}

void Emulator::run()
{
    Instruction instr = disassemble(mmu->getMemory(), cpu.getPC());
    if (cpu.execute()) {
        spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
        QTimer::singleShot(0, [this] { emit next(); });
    } else {
        spdlog::info("{:04x} {:<10} {:<6} {:<13}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
    }
}

class Gui : public QObject {
    Q_OBJECT

public:
    Gui(const Emulator& emu)
        : emulator(emu)
    {
        QImage image(256, 512, QImage::Format_RGB32);
        label.setPixmap(QPixmap::fromImage(image));
        label.show();
    }

public slots:
    // Draw contents of VRAM in 60 FPS.
    void drawSlot()
    {
        static const auto linesPerTile = 8;
        static const auto pixelsPerLine = 8;
        static const auto tilesPerRow = 16;

        auto vram = emulator.getVram();
        QImage image(128, 256, QImage::Format_RGB32);
        for (auto tileY = 0; tileY < 32; ++tileY) {
            for (auto tileX = 0; tileX < tilesPerRow; ++tileX) {
                for (auto line = 0; line < linesPerTile; ++line) {
                    // 2 bytes per line, 8 lines per tile, 16 tiles per row
                    uint16_t index = static_cast<uint16_t>(2 * (line + linesPerTile * (tileX + tilesPerRow * tileY)));
                    auto byte1 = vram.at(index);
                    auto byte0 = vram.at(index + 1);
                    for (auto pixel = pixelsPerLine - 1; pixel >= 0; --pixel) {
                        auto bit1 = (byte1 >> pixel) & 1;
                        auto bit0 = (byte0 >> pixel) & 1;
                        auto color = (bit0 << 1) + bit1;

                        static const std::vector<QColor> colorMap = { Qt::white, Qt::darkGray, Qt::gray, Qt::black };
                        QColor qColor = colorMap.at(static_cast<uint8_t>(color));
                        auto x = tileX * pixelsPerLine + pixelsPerLine - 1 - pixel;
                        auto y = tileY * linesPerTile + line;
                        image.setPixelColor(x, y, qColor);
                    }
                }
            }
        }
        QPixmap pixmap = QPixmap::fromImage(image.scaled(256, 512));
        label.setPixmap(pixmap);
        QTimer::singleShot(1000 / 60.0, [this] { emit drawSignal(); });
    }

signals:
    void drawSignal();

private:
    QLabel label;
    const Emulator& emulator;
};

int runGui(int argc, char** argv)
{
    QApplication app(argc, argv);
    auto romFilename = argv[1];
    auto mmu = std::make_shared<Mmu>();
    Cpu cpu{ mmu };
    Emulator emu{ mmu, cpu, romFilename };
    Gui gui{ emu };
    Debugger debugger{ emu, cpu };
    debugger.show();

    QObject::connect(&emu, &Emulator::next, &emu, &Emulator::run);
    QObject::connect(&gui, &Gui::drawSignal, &gui, &Gui::drawSlot);
    emu.run();
    gui.drawSlot();

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

#include "emulator.moc"
