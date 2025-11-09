#ifndef FAKEDISPLAYOFFOVERLAY_HPP
#define FAKEDISPLAYOFFOVERLAY_HPP

#include <QWidget>
#include <QTimer>

namespace Ui {
class FakeDisplayOffOverlay;
}

class FakeDisplayOffOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit FakeDisplayOffOverlay(QWidget *parent = nullptr);
    ~FakeDisplayOffOverlay();

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::FakeDisplayOffOverlay *ui;
    QTimer hideCursorTimer;
    bool isCursorHidden { false };

private slots:
    void hideOverlayHint();
    void hideMouseCursor();
    void showMouseCursor();
};

#endif // FAKEDISPLAYOFFOVERLAY_HPP
