#include "mainwindow.hpp"

#include <QPushButton>
#include <QScreen>
#include <QProcess>

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
    connect(ui->getSSHStatusBtn, &QPushButton::clicked, this, &MainWindow::querySSHDStatus);
}

void MainWindow::showFakeDisplayOffDialog()
{
    FakeDisplayOffOverlay *overlay = new FakeDisplayOffOverlay(this);
    // QScreen *screen = this->screen();

    // if (!screen) {
    //     screen = QGuiApplication::primaryScreen();
    // }

    overlay->setAttribute(Qt::WA_DeleteOnClose);
    // overlay->setGeometry(screen->geometry());
    // overlay->showFullScreen();
    overlay->setGeometry(this->rect());
    overlay->show();
    overlay->raise();
    overlay->activateWindow();
}

void MainWindow::querySSHDStatus()
{
    QProcess query;
    auto serviceName = QStringLiteral("ssh.socket");

    query.start("/usr/bin/env", { "pkexec", "systemctl", "is-active", serviceName });
    ui->logViewer->append("Quering systemctl on sshd status.");

    if (!query.waitForFinished(5000)) {
        query.kill();
        ui->logViewer->append("Killed, process timed out!");
        return;
    }

    if (query.exitStatus() != QProcess::NormalExit) {
        ui->logViewer->append(QString("Query process crashed with code %1").arg(query.exitCode()));
        return;
    }

    ui->logViewer->append("Done!");

    auto stdErr = query.readAllStandardError().trimmed();
    auto stdOut = query.readAllStandardOutput().trimmed();

    if (!stdErr.isEmpty()) ui->logViewer->append(stdErr);
    if (!stdOut.isEmpty())
        ui->logViewer->append(QStringLiteral("%1 is %2").arg(serviceName).arg(stdOut));
}
