#include "screen.h"
#include "ui_screen.h"

#include <QSettings>

Screen::Screen(const Emulator& emu, QWidget* parent)
    : QMainWindow(parent)
    , emulator(emu)
    , ui(new Ui::Screen)
{
    ui->setupUi(this);
    setFixedSize(QSize(640, 576));
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("Screen/geometry").toByteArray());
}

void Screen::redraw()
{
    auto buffer = emulator.getGpu().getScreenBuffer();
    QImage image(buffer.data(), 160, 144, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image.scaled(640, 576));
    ui->labelVram->setPixmap(pixmap);
}

Screen::~Screen()
{
    delete ui;
}

void Screen::closeEvent(QCloseEvent* event)
{
    QSettings settings("kamil157", "gbemu");
    settings.setValue("Screen/geometry", saveGeometry());

    auto childList = findChildren<QMainWindow*>();
    for (auto child : childList) {
        child->close();
    }
    QWidget::closeEvent(event);
}
