#include "mainwindow.hpp"

#include <QPushButton>
#include <QScreen>
#include <QProcess>

#include "fakedisplayoffoverlay.hpp"

const QString serviceName { "ssh.socket" };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui { std::make_unique<Ui::MainWindow>() }
{
    ui->setupUi(this);
    connectSlots();
    querySSHDStatus();
}

void MainWindow::connectSlots()
{
    connect(ui->turnOffDisplayBtn, &QPushButton::clicked, this,
            &MainWindow::showFakeDisplayOffDialog);
    connect(ui->getSSHStatusBtn, &QPushButton::clicked, this, &MainWindow::querySSHDStatus);
    connect(ui->startSSHBtn, &QPushButton::clicked, this, &MainWindow::startSSHDService);
    connect(ui->stopSSHBtn, &QPushButton::clicked, this, &MainWindow::stopSSHDService);
    connect(ui->showAuthKeyManager, &QPushButton::clicked, this,
            [this]() { ui->logViewer->append("Comming soon!"); });
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
    ui->logViewer->append(QStringLiteral("Quering systemctl on %1 status.").arg(serviceName));
    QProcess query;
    query.start("/usr/bin/env", { "systemctl", "is-active", serviceName });

    if (!query.waitForFinished(5000)) {
        query.kill();
        ui->logViewer->append("Killed, process timed out!");
        return;
    }

    if (query.exitStatus() != QProcess::NormalExit) {
        ui->logViewer->append(QString("Query process crashed with code %1").arg(query.exitCode()));
        return;
    }

    ui->logViewer->append("Query completed.");

    auto stdErr = query.readAllStandardError().trimmed();
    auto stdOut = query.readAllStandardOutput().trimmed();

    if (!stdErr.isEmpty()) ui->logViewer->append(stdErr);
    if (!stdOut.isEmpty())
        ui->logViewer->append(QStringLiteral("%1 is %2").arg(serviceName).arg(stdOut));
}

void MainWindow::startSSHDService()
{
    ui->logViewer->append("Not implemented yet!");
}

void MainWindow::stopSSHDService()
{
    ui->logViewer->append("Not implemented yet!");
}
