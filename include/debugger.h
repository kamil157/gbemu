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
    virtual ~Debugger();

public slots:
    void redraw();
    void playPause();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
    Cpu& cpu;
    bool paused = false;

    void setRegisterLabel(QLabel* label, uint16_t value);

signals:
    void pauseClicked();
    void playClicked();
    void stepClicked();
};

#endif // DEBUGGER_H
