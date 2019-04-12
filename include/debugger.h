#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "emulator.h"

#include <QLabel>
#include <QMainWindow>

namespace Ui {
class Debugger;
}

class Debugger : public QMainWindow {
    Q_OBJECT

public:
    explicit Debugger(const Emulator& emulator, Cpu& cpu, QWidget* parent = nullptr);
    ~Debugger();

public slots:
    void redraw();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
    Cpu& cpu;

    void setRegisterLabel(QLabel* label, uint16_t value);
};

#endif // DEBUGGER_H
