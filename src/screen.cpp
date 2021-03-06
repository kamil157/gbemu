#include "screen.h"
#include "ui_screen.h"

#include <QSettings>

Screen::Screen(const Emulator& emu, QWidget* parent)
    : QMainWindow(parent)
    , emulator(emu)
    , ui(new Ui::Screen)
{
    ui->setupUi(this);
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("Screen/geometry").toByteArray());
    redraw();
}

void Screen::redraw()
{
    auto buffer = emulator.getGpu().getScreenBuffer();
    QImage image(buffer.getData(), 160, 144, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image.scaled(size()));
    ui->labelVram->setPixmap(pixmap);
}

Screen::~Screen()
{
    delete ui;
}

void Screen::resizeEvent(QResizeEvent* event)
{
    ui->labelVram->resize(size());
    redraw();
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
