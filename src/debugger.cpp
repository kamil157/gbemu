#include "debugger.h"
#include "ui_debugger.h"

#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QStyle>
#include <QTimer>

#include <fmt/format.h>

Debugger::Debugger(const Emulator& emulator, Cpu& cpu, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Debugger)
    , emulator(emulator)
    , cpu(cpu)
{
    ui->setupUi(this);
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("Debugger/geometry").toByteArray());

    ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    QShortcut* shortcutPlayPause = new QShortcut(Qt::Key_F5, this);
    QObject::connect(shortcutPlayPause, &QShortcut::activated, this, &Debugger::on_buttonPlayPause_clicked);
    QObject::connect(this, &Debugger::pauseClicked, &emulator, &Emulator::pause);
    QObject::connect(this, &Debugger::playClicked, &emulator, &Emulator::play);

    ui->buttonStep->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    QShortcut* shortcutStep = new QShortcut(Qt::Key_F10, this);
    QObject::connect(shortcutStep, &QShortcut::activated, &emulator, &Emulator::step);
    QObject::connect(ui->buttonStep, &QPushButton::clicked, &emulator, &Emulator::step);

    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Debugger::redraw);
    timer->start(1000 / 60);
}

Debugger::~Debugger()
{
    delete ui;
}

void Debugger::redraw()
{
    static auto setRegisterLabel = [](QLabel* label, uint16_t value) {
        label->setText(QString::fromStdString(fmt::format("0x{:04X}", value)));
    };
    setRegisterLabel(ui->valueAF, cpu.getAF());
    setRegisterLabel(ui->valueBC, cpu.getBC());
    setRegisterLabel(ui->valueDE, cpu.getDE());
    setRegisterLabel(ui->valueHL, cpu.getHL());
    setRegisterLabel(ui->valueSP, cpu.getSP());
    setRegisterLabel(ui->valuePC, cpu.getPC());
}

void Debugger::on_buttonPlayPause_clicked()
{
    paused = !paused;
    ui->buttonStep->setEnabled(paused);
    if (paused) {
        ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        emit pauseClicked();
    } else {
        ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        emit playClicked();
    }
}

void Debugger::closeEvent(QCloseEvent* event)
{
    QSettings settings("kamil157", "gbemu");
    settings.setValue("Debugger/geometry", saveGeometry());
    QWidget::closeEvent(event);
}
