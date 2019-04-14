#ifndef GUI_H
#define GUI_H

#include "emulator.h"

#include <QLabel>
#include <QMainWindow>

namespace Ui {
class Gui;
}

class Gui : public QMainWindow {
    Q_OBJECT

public:
    explicit Gui(const Emulator& emu, QWidget* parent = nullptr);
    virtual ~Gui() override;

public slots:
    // Draw contents of VRAM.
    void redraw();

private:
    const Emulator& emulator;
    QTimer* timer;
    Ui::Gui* ui;

    void closeEvent(QCloseEvent* event) override;
};

#endif // GUI_H
