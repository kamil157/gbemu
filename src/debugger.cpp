#include "debugger.h"
#include "ui_debugger.h"

#include <QTimer>
#include <fmt/format.h>

Debugger::Debugger(const Emulator& emulator, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Debugger)
    , emulator(emulator)
{
    ui->setupUi(this);
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
    timer->start(1000 / 60.0);
}

Debugger::~Debugger()
{
    delete ui;
}

void Debugger::redraw()
{
    Registers reg = emulator.getRegisters();
    ui->valueAF->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.af)));
    ui->valueBC->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.bc)));
    ui->valueDE->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.de)));
    ui->valueHL->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.hl)));
    ui->valueSP->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.sp)));
    ui->valuePC->setText(QString::fromStdString(fmt::format("0x{:04X}", reg.pc)));
}
