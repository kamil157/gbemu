#ifndef VRAMVIEW_H
#define VRAMVIEW_H

#include "emulator.h"

#include <QLabel>
#include <QMainWindow>

namespace Ui {
class VramView;
}

class VramView : public QMainWindow {
    Q_OBJECT

public:
    explicit VramView(const Emulator& emu, QWidget* parent = nullptr);
    virtual ~VramView() override;

public slots:
    // Draw contents of VRAM.
    void redraw();

private:
    const Emulator& emulator;
    Ui::VramView* ui;

    void closeEvent(QCloseEvent* event) override;

    void drawTileData();
    void drawBgMap();
};

#endif // VRAMVIEW_H
