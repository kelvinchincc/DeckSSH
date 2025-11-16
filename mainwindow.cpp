#include "mainwindow.hpp"

#include <QPushButton>
#include <QScreen>
#include <QProcess>
#include <QStringList>

#include "fakedisplayoffoverlay.hpp"

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

const QString MainWindow::checkSSHType()
{
    QStringList sshServices { "sshd.service", "ssh.socket" };

    for (const auto sshService : sshServices) {
        QProcess serviceQuery;
        serviceQuery.start("/usr/bin/env", { "systemctl", "status", sshService });
        if (!serviceQuery.waitForFinished(5000)) {
            serviceQuery.kill();
            continue;
        }
        if (serviceQuery.exitStatus() != QProcess::NormalExit) {
            continue;
        }
        auto stdOut = serviceQuery.readAllStandardOutput().trimmed();
        if (stdOut.contains("Loaded: loaded")) {
            return sshService;
        }
    }

    return "";
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
    auto serviceName = checkSSHType();
    if (serviceName.isEmpty()) {
        ui->logViewer->append(
                QStringLiteral("<span style='color: red'>No supported ssh service found.</span>"));
        return;
    }

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

    if (!stdErr.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style='color: red'>%1</span>").arg(stdErr));

    if (!stdOut.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style=\"color: %1\">%2 is %3</span>")
                        .arg(stdOut == "active" ? "green" : "red")
                        .arg(serviceName)
                        .arg(stdOut));
}

void MainWindow::startSSHDService()
{
    auto sshService = checkSSHType();

    if (sshService.isEmpty()) {
        ui->logViewer->append(
                QStringLiteral("<span style='color: red'>No supported ssh service found.</span>"));
        return;
    }

    ui->logViewer->append(QStringLiteral("Starting SSH service using %1").arg(sshService));

    QProcess startProcess;
    startProcess.start("/usr/bin/env", { "systemctl", "enable", "--now", sshService });

    if (!startProcess.waitForFinished(5 * 60000)) {
        startProcess.kill();
        ui->logViewer->append("<span style=\"color: red;\">Killed, process timed out!</span>");
        return;
    }

    if (startProcess.exitStatus() != QProcess::NormalExit) {
        ui->logViewer->append(
                QString("<span style='color: red'>Start process crashed with code %1</span>")
                        .arg(startProcess.exitCode()));
        return;
    }

    auto stdErr = startProcess.readAllStandardError().trimmed();
    auto stdOut = startProcess.readAllStandardOutput().trimmed();

    if (!stdErr.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style='color: red'>%1</span>").arg(stdErr));

    if (!stdOut.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style='color: green'>%1</span>").arg(stdOut));

    querySSHDStatus();

    ui->logViewer->append("Start command completed.");
}

void MainWindow::stopSSHDService()
{
    auto sshService = checkSSHType();

    if (sshService.isEmpty()) {
        ui->logViewer->append(
                QStringLiteral("<span style='color: red'>No supported ssh service found.</span>"));
        return;
    }

    ui->logViewer->append(QStringLiteral("Stopping SSH service using %1").arg(sshService));

    QProcess startProcess;
    startProcess.start("/usr/bin/env", { "systemctl", "disable", "--now", sshService });

    if (!startProcess.waitForFinished(5 * 60000)) {
        startProcess.kill();
        ui->logViewer->append("<span style=\"color: red;\">Killed, process timed out!</span>");
        return;
    }

    if (startProcess.exitStatus() != QProcess::NormalExit) {
        ui->logViewer->append(
                QString("<span style='color: red'>Stop process crashed with code %1</span>")
                        .arg(startProcess.exitCode()));
        return;
    }

    auto stdErr = startProcess.readAllStandardError().trimmed();
    auto stdOut = startProcess.readAllStandardOutput().trimmed();

    if (!stdErr.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style='color: red'>%1</span>").arg(stdErr));

    if (!stdOut.isEmpty())
        ui->logViewer->append(QStringLiteral("<span style='color: green'>%1</span>").arg(stdOut));

    querySSHDStatus();

    ui->logViewer->append("Stop command completed.");
}
