#include "fakedisplayoffoverlay.hpp"
#include "ui_fakedisplayoffoverlay.h"

#include <QTimer>
#include <QGuiApplication>

FakeDisplayOffOverlay::FakeDisplayOffOverlay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FakeDisplayOffOverlay)
{
    ui->setupUi(this);
    // this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setWindowFlags(Qt::Window);
    this->setWindowModality(Qt::WindowModal);
    this->setMouseTracking(true);
    ui->closeBtn->setMouseTracking(true);
    ui->closeBtn->installEventFilter(this);
    this->setAttribute(Qt::WA_Hover, true);

    QTimer::singleShot(5000, this, &FakeDisplayOffOverlay::hideOverlayHint);
    hideCursorTimer.setInterval(2000);
    connect(&hideCursorTimer, &QTimer::timeout, this, &FakeDisplayOffOverlay::hideMouseCursor);
    hideCursorTimer.start();
}

FakeDisplayOffOverlay::~FakeDisplayOffOverlay()
{
    showMouseCursor();
    delete ui;
}

void FakeDisplayOffOverlay::mouseMoveEvent(QMouseEvent *ev)
{
    QWidget::mouseMoveEvent(ev);
    // This will be called when the mouse is over the overlay itself (not over the button).
    if (isCursorHidden) {
        showMouseCursor();
    }
    hideCursorTimer.start();
    Q_UNUSED(ev);
}

bool FakeDisplayOffOverlay::eventFilter(QObject *watched, QEvent *event)
{
    // Intercept mouse move events from the button (and any other installed widgets).
    if ((event->type() == QEvent::MouseMove || event->type() == QEvent::HoverMove)
            && watched == ui->closeBtn) {
        // Show cursor and restart idle timer
        if (isCursorHidden) {
            showMouseCursor();
        }
        hideCursorTimer.start();
        // let the button also get the event (return false)
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

void FakeDisplayOffOverlay::hideOverlayHint()
{
    this->ui->closeBtn->setText("");
}

void FakeDisplayOffOverlay::hideMouseCursor()
{
    if (isCursorHidden) return;

    QGuiApplication::setOverrideCursor(Qt::BlankCursor);
    isCursorHidden = true;
}

void FakeDisplayOffOverlay::showMouseCursor()
{
    if (!isCursorHidden) return;

    QGuiApplication::restoreOverrideCursor();
    isCursorHidden = false;
}
