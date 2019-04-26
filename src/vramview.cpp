#include "vramview.h"
#include "ui_vramview.h"

#include <QSettings>

VramView::VramView(const Emulator& emu, QWidget* parent)
    : QMainWindow(parent)
    , emulator(emu)
    , ui(new Ui::VramView)
{
    ui->setupUi(this);
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("VramView/geometry").toByteArray());
}

void VramView::drawTileData()
{
    auto buffer = emulator.getGpu().getTileData();
    QImage image(buffer.data(), 128, 256, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image.scaled(256, 512));
    ui->labelVram->setPixmap(pixmap);
}

void VramView::drawBgMap()
{
    auto buffer = emulator.getGpu().getBgMap();
    QImage image(buffer.data(), 256, 256, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image);
    ui->labelBgMap->setPixmap(pixmap);
}

void VramView::redraw()
{
    drawTileData();
    drawBgMap();
}

VramView::~VramView()
{
    delete ui;
}

void VramView::closeEvent(QCloseEvent* event)
{
    QSettings settings("kamil157", "gbemu");
    settings.setValue("VramView/geometry", saveGeometry());

    auto childList = findChildren<QMainWindow*>();
    for (auto child : childList) {
        child->close();
    }
    QWidget::closeEvent(event);
}
