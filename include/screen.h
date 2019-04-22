#ifndef SCREEN_H
#define SCREEN_H

#include "emulator.h"

#include <QLabel>
#include <QMainWindow>

namespace Ui {
class Screen;
}

class Screen : public QMainWindow {
    Q_OBJECT

public:
    explicit Screen(const Emulator& emu, QWidget* parent = nullptr);
    virtual ~Screen() override;

public slots:
    // Draw contents of LCD screen.
    void redraw();

private:
    const Emulator& emulator;
    Ui::Screen* ui;

    void closeEvent(QCloseEvent* event) override;
};

#endif // SCREEN_H
