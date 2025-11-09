#include "mainwindow.hpp"

#include <QPushButton>
#include <QScreen>

#include "fakedisplayoffoverlay.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui { std::make_unique<Ui::MainWindow>() }
{
    ui->setupUi(this);
    this->connectSlots();
}

void MainWindow::connectSlots()
{
    connect(ui->turnOffDisplayBtn, &QPushButton::clicked, this,
            &MainWindow::showFakeDisplayOffDialog);
}

void MainWindow::showFakeDisplayOffDialog()
{
    FakeDisplayOffOverlay *overlay = new FakeDisplayOffOverlay(this);
    QScreen *screen = this->screen();

    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    overlay->setAttribute(Qt::WA_DeleteOnClose);
    overlay->setGeometry(screen->geometry());
    overlay->showFullScreen();
    overlay->raise();
    overlay->activateWindow();
}
