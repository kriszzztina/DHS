#include "authorization_wnd.h"
#include "ui_authorization_wnd.h"
#include <QMessageBox>

extern HMODULE hModule;
extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_C_GetFunctionList pfGetFunctionList;
extern CK_RV rv;
extern CK_SLOT_ID_PTR aSlots; //указатель на массив идентификаторов слотов
extern CK_ULONG ulSlotCount; //количество идентификаторов слотов в массиве
extern CK_SLOT_ID SlotID;
extern CK_SESSION_HANDLE hSession;

Authorization_wnd::Authorization_wnd(QWidget *parent, char slot_ID, CK_SESSION_HANDLE_PTR session) :
    QDialog(parent),
    ui(new Ui::Authorization_wnd)
{
    ui->setupUi(this);
    res = false;
    connect(this,SIGNAL(window_loaded()),this,SLOT(Open_Session()));
    connect(ui->Enter_PIN_button,SIGNAL(clicked(bool)),this,SLOT(Authorization()));
    this->slot_ID = slot_ID;
    this->session = session;
}

void Authorization_wnd::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    emit window_loaded();
}


void Authorization_wnd::Open_Session()
{
    /*открытие сессии*/

    //rv = pFunctionList->C_OpenSession(aSlots[slot_ID], CKF_SERIAL_SESSION|CKF_RW_SESSION, NULL_PTR, NULL_PTR, session);
}

void Authorization_wnd::Authorization()
{
    static int counter = 2;
    /*аутентификация*/
    rv = pFunctionList->C_Login(*session, CKU_USER, (CK_UTF8CHAR_PTR)(ui->Enter_PIN_Editor->text().toStdString().c_str()), sizeof(USER_PIN));
    if (rv == CKR_OK)
    {
        res = true;
        counter = 2;
        close();
    }
    else
    {
        counter--;
        if (counter == 0)
        {
            res = false;
            close();
        }
        else
        {
            QMessageBox::information(0, "Ошибка", "Неверный PIN-код!", QMessageBox::Ok);
        }
    }

}


Authorization_wnd::~Authorization_wnd()
{
    delete ui;
}
