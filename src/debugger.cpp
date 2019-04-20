#include "debugger.h"
#include "ui_debugger.h"

#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSettings>
#include <QShortcut>
#include <QString>
#include <QStyle>
#include <QTimer>

#include <fmt/format.h>

Debugger::Debugger(const Emulator& emulator, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Debugger)
    , emulator(emulator)
    , cpu(emulator.getCpu())
{
    ui->setupUi(this);
    QSettings settings("kamil157", "gbemu");
    restoreGeometry(settings.value("Debugger/geometry").toByteArray());

    ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    QShortcut* shortcutPlayPause = new QShortcut(Qt::Key_F5, this);
    QObject::connect(shortcutPlayPause, &QShortcut::activated, this, &Debugger::on_buttonPlayPause_clicked);

    ui->buttonStep->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    QShortcut* shortcutStep = new QShortcut(Qt::Key_F10, this);
    QObject::connect(shortcutStep, &QShortcut::activated, this, &Debugger::on_buttonStep_clicked);

    ui->buttonStepFrame->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    QShortcut* shortcutStepFrame = new QShortcut(Qt::Key_F9, this);
    QObject::connect(shortcutStepFrame, &QShortcut::activated, this, &Debugger::on_buttonStepFrame_clicked);

    QRegExp regex("[0-9a-fA-F]{0,4}");
    QValidator* validator = new QRegExpValidator(regex, this);
    ui->textBreakpointPC->setValidator(validator);

    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Debugger::redraw);
    timer->start(1000 / 30);

    onEmulationPaused();
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

    Instruction instr = disassemble(emulator.getMmu().getMemory(), cpu.getPC());
    auto nextInstructionStr = fmt::format("{} {}", instr.mnemonic, instr.operandsToString());
    ui->labelInstruction->setText(QString::fromStdString(nextInstructionStr));

    auto nextBytesStr = fmt::format("{}", instr.bytesToString());
    ui->labelBytes->setText(QString::fromStdString(nextBytesStr));
}

void Debugger::on_buttonPlayPause_clicked()
{
    paused = !paused;
    if (paused) {
        emit pauseClicked();
    } else {
        emit playClicked();
    }
}

void Debugger::on_textBreakpointPC_returnPressed()
{

    auto text = ui->textBreakpointPC->text();
    bool ok;
    auto pc = static_cast<uint16_t>(text.toInt(&ok, 16));
    if (ok) {
        emit breakpointSet(pc);
    } else {
        emit breakpointUnset();
    }
}

void Debugger::on_buttonStep_clicked()
{
    emit stepClicked();
}

void Debugger::on_buttonStepFrame_clicked()
{
    emit stepFrameClicked();
}

void Debugger::onEmulationPaused()
{
    paused = true;
    ui->buttonStep->setEnabled(paused);
    ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void Debugger::onEmulationResumed()
{
    paused = false;
    ui->buttonStep->setEnabled(paused);
    ui->buttonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
}

void Debugger::closeEvent(QCloseEvent* event)
{
    QSettings settings("kamil157", "gbemu");
    settings.setValue("Debugger/geometry", saveGeometry());
    QWidget::closeEvent(event);
}
