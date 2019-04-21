#include "gui.h"
#include "ui_gui.h"

#include <QSettings>

Gui::Gui(const Emulator& emu, QWidget* parent)
    : QMainWindow(parent)
    , emulator(emu)
    , ui(new Ui::Gui)
{
    ui->setupUi(this);
    setFixedSize(QSize(256, 512));
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("Gui/geometry").toByteArray());

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Gui::redraw);
    timer->start(1000 / 5);
}

struct Color {
    uint8_t r, g, b;
};

void Gui::redraw()
{
    static const auto linesPerTile = 8;
    static const auto pixelsPerLine = 8;
    static const auto tilesPerRow = 16;

    auto vram = emulator.getMmu().getVram();
    auto w = 128u;
    auto h = 256u;
    std::vector<uint8_t> buffer(4 * w * h, 0);
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
                    auto colorIndex = (bit0 << 1) + bit1;

                    static const std::vector<Color> colorMap = { { 255, 255, 255 }, { 170, 170, 170 }, { 85, 85, 85 }, { 0, 0, 0 } };
                    Color color = colorMap.at(static_cast<uint8_t>(colorIndex));
                    auto x = tileX * pixelsPerLine + pixelsPerLine - 1 - pixel;
                    auto y = tileY * linesPerTile + line;
                    buffer[4 * (y * w + x)] = color.r;
                    buffer[4 * (y * w + x) + 1] = color.g;
                    buffer[4 * (y * w + x) + 2] = color.b;
                }
            }
        }
    }
    QImage image(buffer.data(), w, h, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image.scaled(256, 512));
    ui->labelVram->setPixmap(pixmap);
}

Gui::~Gui()
{
    delete ui;
    delete timer;
}

void Gui::closeEvent(QCloseEvent* event)
{
    QSettings settings("kamil157", "gbemu");
    settings.setValue("Gui/geometry", saveGeometry());

    auto childList = findChildren<QMainWindow*>();
    for (auto child : childList) {
        child->close();
    }
    QWidget::closeEvent(event);
}
