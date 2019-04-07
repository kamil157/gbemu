#include "cpu.h"

#include "utils.h"

#include <exception>
#include <memory>
#include <stdexcept>

#include "spdlog/spdlog.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QTimer>

#include <QObject>

class Emulator : public QObject {
    Q_OBJECT

public:
    Emulator(const std::string& romFilename)
        : cpu(nullptr)
    {
        mmu = std::make_shared<Mmu>();
        auto bootstrapRom = readFile("../gbemu/res/bootstrap.bin");
        mmu->loadBootstrap(bootstrapRom);
        auto cartridgeRom = readFile(romFilename);
        mmu->loadCartridge(cartridgeRom);
        cpu = Cpu{ mmu };
    }

public slots:
    // Run one command and signal Gui.
    void run()
    {
        bool result = cpu.execute();
        if (result) {
            Instruction instr = disassemble(mmu->getMemory(), pc);
            spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
            pc = cpu.getPC();

            QTimer::singleShot(0, [this] { emit next(mmu->getVram()); });
            return;
        }
        Instruction instr = disassemble(mmu->getMemory(), pc);
        spdlog::debug("{:04x} {:<10} {:<6} {:<13}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
    }

signals:
    void next(const std::vector<uint8_t>&);

private:
    Cpu cpu;
    std::shared_ptr<Mmu> mmu;
    uint16_t pc = 0u;
};

class Gui : public QObject {
    Q_OBJECT

public:
    Gui()
    {
        QImage image(128, 256, QImage::Format_RGB32);
        label.setPixmap(QPixmap::fromImage(image));
        label.show();
    }

public slots:
    // Draw contents of vram on label and signal Emulator.
    void draw(const std::vector<uint8_t>& vram)
    {
        //        QPixmap pixmap = QPixmap::fromImage(QImage(vram.data(), 256, 256, QImage::Format::Format_Mono));
        QImage image(128, 256, QImage::Format::Format_RGB32);
        image.fill(Qt::green);
        for (auto tileY = 0; tileY < 32; ++tileY) {
            for (auto tileX = 0; tileX < 16; ++tileX) {
                for (auto line = 0; line < 8; ++line) {
                    auto index = 2 * line + tileY * 64 + tileX * 8;
                    auto byte0 = vram[index + 1];
                    auto byte1 = vram[index];
                    for (auto pixel = 7; pixel >= 0; --pixel) {
                        auto bit0 = (byte0 >> pixel) & 1;
                        auto bit1 = (byte1 >> pixel) & 1;
                        auto color = (bit0 << 1) + bit1;

                        QColor qColor;
                        switch (color) {
                        case 0:
                            qColor = Qt::gray;
                            break;
                        case 1:
                            qColor = Qt::darkGray;
                            break;
                        case 2:
                            qColor = Qt::white;
                            break;
                        case 3:
                            qColor = Qt::black;
                            break;
                        }
                        auto x = tileX * 8 + 7 - pixel;
                        auto y = tileY * 8 + line;
                        image.setPixelColor(x, y, qColor);
                    }
                }
            }
        }
        QPixmap pixmap = QPixmap::fromImage(image);
        label.setPixmap(pixmap);
        emit next();
    }

signals:
    void next();

private:
    QLabel label;
};

int runGui(int argc, char** argv)
{
    QApplication app(argc, argv);
    Gui gui;
    auto romFilename = argv[1];
    Emulator emu{ romFilename };

    QObject::connect(&emu, SIGNAL(next(const std::vector<uint8_t>&)), &gui, SLOT(draw(const std::vector<uint8_t>&)));
    QObject::connect(&gui, SIGNAL(next()), &emu, SLOT(run()));
    emu.run();

    return app.exec();
}

int main(int argc, char** argv)
{
    try {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("%v");
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
