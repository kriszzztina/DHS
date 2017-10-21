#ifndef AUTHORIZATION_WND_H
#define AUTHORIZATION_WND_H

#include <QDialog>
#include <Common.h>

namespace Ui {
class Authorization_wnd;
}

class Authorization_wnd : public QDialog
{
    Q_OBJECT

public:
    explicit Authorization_wnd(QWidget *parent = 0, char slot_ID = 0, CK_SESSION_HANDLE_PTR session = 0);
    ~Authorization_wnd();
   bool GetResult()
   {
       return res;
   }

private:
    Ui::Authorization_wnd *ui;
    bool res;
    CK_SESSION_HANDLE_PTR session;
    char slot_ID;

protected:
    void showEvent(QShowEvent *ev);
signals:
    void window_loaded();
private slots:
    void Open_Session();
    void Authorization();
};



#endif // AUTHORIZATION_WND_H
