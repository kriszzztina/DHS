#include "authorization_window.h"
#include "ui_authorization_window.h"

Authorization_window::Authorization_window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Authorization_window)
{
    ui->setupUi(this);
    ui->Enter_PIN_Group->hide();
}

Authorization_window::~Authorization_window()
{
    delete ui;
}
