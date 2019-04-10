#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "emulator.h"

#include <QMainWindow>

namespace Ui {
class Debugger;
}

class Debugger : public QMainWindow {
    Q_OBJECT

public:
    explicit Debugger(const Emulator& emulator, QWidget* parent = nullptr);
    ~Debugger();

public slots:
    void redraw();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
};

#endif // DEBUGGER_H
