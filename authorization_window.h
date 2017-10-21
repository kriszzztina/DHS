#ifndef AUTHORIZATION_WINDOW_H
#define AUTHORIZATION_WINDOW_H

#include <QWidget>

namespace Ui {
class Authorization_window;
}

class Authorization_window : public QWidget
{
    Q_OBJECT

public:
    explicit Authorization_window(QWidget *parent = 0);
    ~Authorization_window();

private:
    Ui::Authorization_window *ui;
};

#endif // AUTHORIZATION_WINDOW_H
