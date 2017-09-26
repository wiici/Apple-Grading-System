#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QComboBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString WindowName, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // show all available video capture devices in the ComboBox
    void ShowConnectedCameras(QComboBox* ComboBox);
};

#endif // MAINWINDOW_H
