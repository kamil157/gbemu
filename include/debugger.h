#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "emulator.h"

#include <QLabel>
#include <QMainWindow>
#include <QObject>

namespace Ui {
class Debugger;
}

class Debugger : public QMainWindow {
    Q_OBJECT

public:
    explicit Debugger(const Emulator& emulator, QWidget* parent = nullptr);
    virtual ~Debugger() override;

public slots:
    // Redraw debugger window.
    void redraw();

    // Execution was paused from an external source.
    void onEmulationPaused();

    // Execution was resumed from an external source.
    void onEmulationResumed();

    // Play or pause execution.
    void on_buttonPlayPause_clicked();

    // Emulate one instruction.
    void on_buttonStep_clicked();

    // Emulate one instruction.
    void on_buttonStepFrame_clicked();

    // Set or unset breakpoint at PC.
    void on_textBreakpointPC_returnPressed();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
    const Cpu& cpu;

    // Is debugger paused.
    bool paused = true;

    void closeEvent(QCloseEvent* event) override;

signals:
    void pauseClicked();
    void playClicked();
    void stepClicked();
    void stepFrameClicked();
    void breakpointSet(uint16_t pc);
    void breakpointUnset();
};

#endif // DEBUGGER_H
