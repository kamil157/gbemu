#include "debugger.h"
#include "ui_debugger.h"

#include <QTimer>
#include <fmt/format.h>

Debugger::Debugger(const Emulator& emulator, Cpu& cpu, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Debugger)
    , emulator(emulator)
    , cpu(cpu)
{
    ui->setupUi(this);
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
    timer->start(1000 / 60);
}

Debugger::~Debugger()
{
    delete ui;
}

void Debugger::setRegisterLabel(QLabel* label, uint16_t value)
{
    label->setText(QString::fromStdString(fmt::format("0x{:04X}", value)));
}

void Debugger::redraw()
{
    setRegisterLabel(ui->valueAF, cpu.getAF());
    setRegisterLabel(ui->valueBC, cpu.getBC());
    setRegisterLabel(ui->valueDE, cpu.getDE());
    setRegisterLabel(ui->valueHL, cpu.getHL());
    setRegisterLabel(ui->valueSP, cpu.getSP());
    setRegisterLabel(ui->valuePC, cpu.getPC());
}
