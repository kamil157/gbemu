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
    virtual ~Debugger() override;

public slots:
    // Redraw debugger window.
    void redraw();

    void on_buttonPlayPause_clicked();

private:
    Ui::Debugger* ui;
    const Emulator& emulator;
    Cpu& cpu;

    // Is debugger paused.
    bool paused = false;

    void closeEvent(QCloseEvent* event) override;

signals:
    void pauseClicked();
    void playClicked();
    void stepClicked();
};

#endif // DEBUGGER_H
