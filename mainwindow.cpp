#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Common.h>
#include <QMessageBox>
#include "authorization_wnd.h"
#include "write_data.h"
#include "read_full_data.h"
#include "read_half_data.h"
#include <QCloseEvent>
#include <fstream>
#include <QtSql>
#include "cardwatcher.h"
#include "change_data.h"

extern HMODULE hModule;
extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_C_GetFunctionList pfGetFunctionList;
extern CK_RV rv;
extern CK_SLOT_ID_PTR aSlots; //указатель на массив идентификаторов слотов
extern CK_ULONG ulSlotCount; //количество идентификаторов слотов в массиве
extern CK_SLOT_ID SlotID;
extern CK_SESSION_HANDLE hDoctorSession;
extern CK_SESSION_HANDLE hPatientSession;
CardWatcher cw;
bool flag = false;

extern QString Read_critical();


extern QString Doctors_FIO;

enum Get_Token_result{Success, Fail, Cancel};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&cw, SIGNAL(Attached()),this, SLOT(TokenAttached()));
    connect(&cw, SIGNAL(Detached()),this, SLOT(TokenDetached()));
    ui->Who_am_I->setText(QString("Вы авторизованы как %1").arg(Doctors_FIO));
    cw.start();
}

Get_Token_result GetToken(char slots_count)
{
    Get_Token_result result;
    while(TRUE)
    {
        /* Получить количество слотов c подключенными токенами */
        rv = pFunctionList->C_GetSlotList(CK_TRUE, NULL_PTR, &ulSlotCount);
        if (ulSlotCount < slots_count)
        {
            int n = QMessageBox::critical(0,"Information","Вставьте Smart-карту или токен", QMessageBox::Ok, QMessageBox::Cancel);
            if (n == QMessageBox::Cancel)
            {
                result = Get_Token_result::Cancel;
                return result;
            }
            else
            {
                result = Get_Token_result::Fail;
                return result;
            }
        }
        aSlots = (CK_SLOT_ID*)malloc(ulSlotCount*sizeof(CK_SLOT_ID));
        if (aSlots == NULL)
        {
            result = Get_Token_result::Fail;
            return result;
        }
        memset(aSlots, 0, (ulSlotCount*sizeof(CK_SLOT_ID)));
        /* Получить список слотов с подключенными токенами*/
        rv = pFunctionList->C_GetSlotList(CK_TRUE, aSlots, &ulSlotCount);
        if (rv != CKR_OK)
        {
            result = Get_Token_result::Fail;
            return result;
        }
        result = Get_Token_result::Success;
        return result;
    }
}



void MainWindow::TokenAttached()
{
    rv = pFunctionList->C_OpenSession(1, CKF_SERIAL_SESSION|CKF_RW_SESSION, NULL_PTR, NULL_PTR, &hPatientSession);
    if (Read_critical() == "")
    {
        int ans = QMessageBox::information(0,"", "На карте нет информации! Вы хотите записать информацию?", QMessageBox::Yes, QMessageBox::No);
        if (ans == QMessageBox::Yes)
        {
            Authorization_wnd *wnd = new Authorization_wnd(0, 1, &hPatientSession);
            wnd->exec();
            int n = wnd->GetResult();
            if (n)
            {
                write_data *wd = new write_data();
                int n = wd->exec();
                delete wd;
                if (n == -1)
                {
                    QMessageBox::information(0,"Fatal error","Информация не была записана", QMessageBox::Ok);
                    return;
                }
                else
                {
                    pFunctionList->C_Logout(hPatientSession);
                    read_half_data *rh = new read_half_data();
                    rh->exec();
                    delete rh;
                }

            }
            else
            {
                return;
            }
        }
    }
    else
    {

        read_half_data *rh = new read_half_data();
        rh->exec();
        delete rh;
    }
}

void MainWindow::TokenDetached()
{
    if (aSlots)
    {
        free(aSlots);
        aSlots = NULL_PTR;
    }
    ui->Who_am_I->setText("Вы не авторизованы");
    rv = pFunctionList->C_CloseSession(hDoctorSession);
    hDoctorSession = NULL_PTR;
    QApplication::closeAllWindows();
}


void Deinitialize()
{

    if (aSlots)
    {
        pFunctionList->C_Logout(hDoctorSession);
        /*закрытие сессии*/
        pFunctionList->C_CloseAllSessions(aSlots[0]);
        if (aSlots)
        {
            free(aSlots);
            aSlots = NULL_PTR;
        }
    }


    /*деинициализация библиотеки*/
    if (pFunctionList)
    {
        pFunctionList->C_Finalize(NULL_PTR);
        pFunctionList = NULL_PTR;
    }
    if (hModule)
        hModule = NULL_PTR;
}

void MainWindow::closeEvent(QCloseEvent *eve)
{
    cw.exit();
    Deinitialize();
    eve->accept();
}




MainWindow::~MainWindow()
{
    delete ui;
}
