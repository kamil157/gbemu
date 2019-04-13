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
    explicit Debugger(const Emulator& emulator, Cpu& cpu, QWidget* parent = nullptr);
    ~Debugger();

public slots:
    void redraw();
    void playPauseClicked();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
    Cpu& cpu;
    bool paused = false;

    void setRegisterLabel(QLabel* label, uint16_t value);

signals:
    void pauseClicked();
    void playClicked();
};

#endif // DEBUGGER_H
