#include "read_half_data.h"
#include "ui_read_half_data.h"
#include <Common.h>
#include "authorization_wnd.h"
#include "read_full_data.h"
#include <QMessageBox>
#include <QFile>
#include <QListWidget>
#include "cardwatcher.h"
#include <QtSql>
#include "create_patient_db.h"

extern HMODULE hModule;
extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_C_GetFunctionList pfGetFunctionList;
extern CK_RV rv;
extern CK_SLOT_ID_PTR aSlots; //указатель на массив идентификаторов слотов
extern CK_ULONG ulSlotCount; //количество идентификаторов слотов в массиве
extern CK_SLOT_ID SlotID;
extern CK_SESSION_HANDLE hDoctorSession;
extern CK_SESSION_HANDLE hPatientSession;

QMap<QString, QString> map_HZ, map_AS;

extern QString Read_critical();
extern QString Read_ID();
extern QString Read_FIO();

//Проверка целостности
bool CheckSign()
{
    // Получить открытый ключ с карты пациента
    CK_UTF8CHAR      Doctor_pub_key[]    = {"Certificate"};

    CK_ATTRIBUTE attrPubKeyFind[] = {
        {CKA_LABEL, &Doctor_pub_key, sizeof(Doctor_pub_key)-1}
    };

    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;

    while(TRUE)
    {
        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrPubKeyFind, arraysize(attrPubKeyFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);
        break;
    }
    rv = pFunctionList->C_FindObjectsFinal(hPatientSession);


    CK_ATTRIBUTE attrReadCert = {CKA_VALUE, NULL_PTR, NULL};

    while (TRUE)
    {
        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadCert, 1);

        attrReadCert.pValue = (CK_BYTE*)malloc(attrReadCert.ulValueLen);

        memset(attrReadCert.pValue,0, (attrReadCert.ulValueLen*sizeof(CK_BYTE)));

        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadCert, 1);
        break;
    }



    CK_OBJECT_HANDLE hPublicKey;

    CK_UTF8CHAR Key_label[] = "Public key";
    CK_ATTRIBUTE attrGOSTPublicKey[] =
    {
        { CKA_CLASS, &ocPubKey, sizeof(ocPubKey)},                      // Объект открытого ключа ГОСТ Р 34.10-2001 (#1)
        { CKA_LABEL, &Key_label, sizeof(Key_label) - 1},        // Метка ключа
        { CKA_ID, &KeyPairIDGOST1, sizeof(KeyPairIDGOST1) - 1},         // Идентификатор ключевой пары #1 (должен совпадать у открытого и закрытого ключей)
        { CKA_KEY_TYPE, &ktGOST34_10_2001, sizeof(ktGOST34_10_2001)},   // Тип ключа
        { CKA_TOKEN, &bFalse, sizeof(bFalse)},                            // Ключ является объектом токена
        { CKA_PRIVATE, &bFalse, sizeof(bFalse)},                        // Ключ доступен без авторизации на токене
        { CKA_GOSTR3410_PARAMS, GOST3410_params_oid, sizeof(GOST3410_params_oid) },
        { CKA_GOSTR3411_PARAMS, GOST3411_params_oid, sizeof(GOST3411_params_oid) },  // Параметры алгоритма ГОСТ Р 34.11-1994
        { CKA_VALUE, attrReadCert.pValue, attrReadCert.ulValueLen}
    };

    rv = pFunctionList->C_CreateObject(hPatientSession, attrGOSTPublicKey, arraysize(attrGOSTPublicKey), &hPublicKey);

    //Получить подпись
    CK_UTF8CHAR      Sign_label[]    = {"Signature"};

    CK_ATTRIBUTE attrSignFind[] = {
        {CKA_LABEL, &Sign_label, sizeof(Sign_label)-1}
    };

    phObject = NULL_PTR;
    ulObjectCount = 0;

    while(TRUE)
    {
        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrSignFind, arraysize(attrSignFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);
        break;
    }
    rv = pFunctionList->C_FindObjectsFinal(hPatientSession);

    //Получить значение подписи
    CK_ATTRIBUTE attrReadSign = {CKA_VALUE, NULL_PTR, NULL};

    while (TRUE)
    {
        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadSign, 1);

        attrReadSign.pValue = (CK_BYTE*)malloc(attrReadSign.ulValueLen);

        memset(attrReadSign.pValue,0, (attrReadSign.ulValueLen*sizeof(CK_BYTE)));

        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadSign, 1);
        break;
    }

    //Найти данные
    CK_UTF8CHAR      Data_label[]    = {"Critical data"};

    CK_ATTRIBUTE attrDataFind[] = {
        {CKA_LABEL, &Data_label, sizeof(Data_label)-1}
    };

    phObject = NULL_PTR;
    ulObjectCount = 0;

    while(TRUE)
    {
        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrDataFind, arraysize(attrDataFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);
        break;
    }
    rv = pFunctionList->C_FindObjectsFinal(hPatientSession);

    //Получить значение данных
    CK_ATTRIBUTE attrReadData = {CKA_VALUE, NULL_PTR, NULL};

    while (TRUE)
    {
        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadData, 1);

        attrReadData.pValue = (CK_BYTE*)malloc(attrReadData.ulValueLen);

        memset(attrReadData.pValue,0, (attrReadData.ulValueLen*sizeof(CK_BYTE)));

        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadData, 1);
        break;
    }

    //инициализировать операцию хэширования
    CK_BYTE_PTR pHash = NULL;
    CK_ULONG ulHashSize = 0;

    while (TRUE)
    {

        rv = pFunctionList->C_DigestInit(hPatientSession, &ckmGOST34_11_94Mech);

        rv = pFunctionList->C_Digest(hPatientSession,
                                     (CK_BYTE_PTR)attrReadData.pValue,
                                     attrReadData.ulValueLen,
                                     pHash,
                                     &ulHashSize);


        pHash = (CK_BYTE*)malloc(ulHashSize);
        memset(pHash, 0, (ulHashSize * sizeof(CK_BYTE)));


        rv = pFunctionList->C_Digest(hPatientSession,
                                     (CK_BYTE_PTR)attrReadData.pValue,
                                     attrReadData.ulValueLen,
                                     pHash,
                                     &ulHashSize);

        break;
    }
    //pHash[0] = 0; //нарушение целостности хэша
    //Операция проверки подписи
    rv = pFunctionList->C_VerifyInit(hPatientSession,       // Хэндл сессии
                                     &ckmGOST_34_10_2001SigVerMech,   // Механизм подписи
                                     hPublicKey);

    rv = pFunctionList->C_Verify(hPatientSession,           // Хэндл сессии
                                     pHash,            // Буфер с значением хеша исходногосообщения
                                     ulHashSize,        // Длина буфера
                                     (CK_BYTE_PTR)attrReadSign.pValue,      // Буфер с подписью
                                     attrReadSign.ulValueLen);

    if (rv != CKR_OK)
        return false;
    else
        return true;

}


void read_half_data::OpenMedicalCard()
{
    pFunctionList->C_Logout(hPatientSession);
    Authorization_wnd *wnd = new Authorization_wnd(0,1,&hPatientSession);
    wnd->exec();
    int n = wnd->GetResult();
    if (n)
    {
       delete wnd;
        QString str_ID = Read_ID();
        QStringList temp = Read_FIO().simplified().split(" ");
        QString surname = temp[0];
        QString name = temp[1];
        QString mn = temp[2];
        QSqlRecord rec;
        QSqlQuery query;
        query.exec(QString("SELECT Фамилия FROM patient WHERE idp = '%1';").arg(str_ID));
        rec = query.record();
        query.next();
        if (query.value(0).toString().isEmpty())
        {
            int n = QMessageBox::information(0,"","Пациента нет в базе данных! Хотите добавить его сейчас?", QMessageBox::Yes, QMessageBox::No);
            if (n == QMessageBox::Yes)
            {
                Create_patient_DB *cpdb = new Create_patient_DB(0, str_ID, surname, name, mn, false);
                cpdb->exec();
                delete cpdb;
            }
            this->close();
            return;
        }
       Read_full_data *rf = new Read_full_data();
       this->close();
       rf->exec();
       rv = pFunctionList->C_CloseSession(hPatientSession);
       hPatientSession = NULL_PTR;
       delete rf;
       return;
    }
    rv = pFunctionList->C_CloseSession(hPatientSession);
    hPatientSession = NULL_PTR;
    delete wnd;
}


read_half_data::read_half_data(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::read_half_data)
{
    ui->setupUi(this);
    QString str = Read_critical();
    if (str == "")
    {
        QMessageBox::critical(0,"","Информация не найдена!", QMessageBox::Ok);
        return;
    }
    if (!CheckSign())
        QMessageBox::critical(0,"Error", "\nЦелостность данных была нарушена!\n", QMessageBox::Ok);

    QStringList full_data = Read_critical().simplified().split(";");

    ui->AB0_Rh_edit->setText(full_data.at(0));
    switch (full_data.at(1).toInt())
    {
        case 0:
            ui->SD_edit->setText("нет диабета");
            break;
        case 1:
            ui->SD_edit->setText("диабет 1-го типа");
            break;
        case 2:
            ui->SD_edit->setText("диабет 2-го типа");
            break;
    }
    switch (full_data.at(2).toInt())
    {
        case 0:
            ui->AID_edit->setText("отрицательный");
            break;
        case 1:
            ui->AID_edit->setText("положительный");
            break;
    }
    ui->H_edit->setText(full_data.at(3));
    Parse(ui->HZ_list, ":/res/MKB.txt", map_HZ, 4);
    Parse(ui->AS_list, ":/res/ATC.txt", map_AS, 5);
    connect(ui->Read_full, SIGNAL(clicked(bool)), this, SLOT(OpenMedicalCard()));
}





void read_half_data::Parse(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index)
{
    QFile f(file_n);
    if(f.open(QIODevice::ReadOnly) == false)
        QMessageBox::information(0, "", "Failed", QMessageBox::Ok);
    QByteArray filetext = f.readAll();
    f.close();
    QString str_all_file(filetext); //преобразуем весь файл в строку
    str_all_file = str_all_file.simplified(); //удаляем спецсимволы
    QStringList illness_list = str_all_file.split(';');
    QStringList full_data = Read_critical().simplified().split(";");
    QStringList temp = full_data.at(index).simplified().split(",");

    for (int i=0; i<temp.count(); i++)
    {
        for (int j=0; j<illness_list.length(); j++)
        {

            QString code_and_name_temp = illness_list.at(j);
            QStringList code_and_name_list = code_and_name_temp.split(',');
            maps.insert(code_and_name_list.at(1),code_and_name_list.at(0));
            if (code_and_name_list.at(0) == temp.at(i))
            {
                list->addItem(code_and_name_list.at(1));
                break;
            }
        }
    }
}

read_half_data::~read_half_data()
{
    delete ui;
}
