#include "vramview.h"
#include "ui_vramview.h"

#include <QSettings>

VramView::VramView(const Emulator& emu, QWidget* parent)
    : QMainWindow(parent)
    , emulator(emu)
    , ui(new Ui::VramView)
{
    ui->setupUi(this);
    setFixedSize(QSize(256, 512));
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("VramView/geometry").toByteArray());

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &VramView::redraw);
    timer->start(1000 / 5);
}

void VramView::redraw()
{
    auto buffer = emulator.getGpu().getTileData();
    QImage image(buffer.data(), 128, 256, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image.scaled(256, 512));
    ui->labelVram->setPixmap(pixmap);
}

VramView::~VramView()
{
    delete ui;
    delete timer;
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
