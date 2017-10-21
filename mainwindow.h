#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void TokenDetached();
    void TokenAttached();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *eve);

};

#endif // MAINWINDOW_H
