#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>

#include "./ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void connectSlots();

private:
    std::unique_ptr<Ui::MainWindow> ui{nullptr};

private slots:
    void showFakeDisplayOffDialog();
    void querySSHDStatus();
};
#endif // MAINWINDOW_HPP
