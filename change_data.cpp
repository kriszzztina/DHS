#include "change_data.h"
#include "ui_change_data.h"
#include "read_full_data.h"
#include <QMap>
#include <QComboBox>
#include "Common.h"
#include <QMessageBox>
#include <QTextCodec>
#include <QtSql>
/*
extern QMap<QString,QString> HZ_map, AS_map;

extern QString Read_ID();

extern QTextCodec *codec;

extern QString Doctors_FIO;

extern void Parse_for_list(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index);

extern HMODULE hModule;
extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_C_GetFunctionList pfGetFunctionList;
extern CK_RV rv;
extern CK_SLOT_ID_PTR aSlots; //указатель на массив идентификаторов слотов
extern CK_ULONG ulSlotCount; //количество идентификаторов слотов в массиве
extern CK_SLOT_ID SlotID;
extern CK_SESSION_HANDLE hDoctorSession;
extern CK_SESSION_HANDLE hPatientSession;

Change_data::Change_data(QWidget *parent, QString critical_data_str, QString surname_str, QString name_str, QString MN_str) :
    QDialog(parent),
    ui(new Ui::Change_data)
{
    ui->setupUi(this);
    ui->Surname_line->setText(surname_str);
    ui->Name_line->setText(name_str);
    ui->MN_line->setText(MN_str);
    this->critical_data_str = critical_data_str;
    this->surname_str = surname_str;
    this->name_str = name_str;
    this->MN_str = MN_str;

    QStringList full_data = critical_data_str.simplified().split(";");
    ui->AB0_Rh_combo->setCurrentText(full_data.at(0));
    switch (full_data.at(1).toInt())
    {
        case 0:
            ui->SD_combo->setCurrentText("нет диабета");
            break;
        case 1:
            ui->SD_combo->setCurrentText("диабет 1-го типа");
            break;
        case 2:
            ui->SD_combo->setCurrentText("диабет 2-го типа");
            break;
    }
    switch (full_data.at(2).toInt())
    {
        case 0:
            ui->AID_combo->setCurrentText("отрицательный");
            break;
        case 1:
            ui->AID_combo->setCurrentText("положительный");
            break;
    }
    ui->H_combo->setCurrentText(full_data.at(3));
    Parse_for_list(ui->HZ_list, ":/res/MKB.txt", HZ_map, 4);
    Parse_for_list(ui->AS_list, ":/res/ATC.txt", AS_map, 5);
    connect(ui->Save_change, SIGNAL(clicked(bool)),this,SLOT(Save_change()));
    fill_comboBox(ui->HZ_combo,":/res/MKB.txt", CDs);
    connect(ui->HZ_combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(Show_HZ_code(QString)));
    connect(ui->HZ_check, SIGNAL(clicked(bool)),this, SLOT(Activate_HZ_group()));
    connect(ui->HZ_button, SIGNAL(clicked(bool)),this, SLOT(Add_to_List()));
    fill_comboBox(ui->AS_combo,":/res/ATC.txt", AIs);
    connect(ui->AS_combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(Show_HZ_code(QString)));
    connect(ui->AS_check, SIGNAL(clicked(bool)),this, SLOT(Activate_HZ_group()));
    connect(ui->AS_button, SIGNAL(clicked(bool)),this, SLOT(Add_to_List()));
    ui->HZ_combo->setCurrentIndex(0);
    ui->AS_combo->setCurrentText("");
}


void Delete(CK_UTF8CHAR *label, size_t size)
{
    CK_ATTRIBUTE attrIDFind[] = {
        {CKA_LABEL, label, size}
    };

    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;
    CK_RV rvTemp = CKR_OK;

    while(TRUE)
    {
        while(TRUE)
        {
            rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrIDFind, arraysize(attrIDFind));

            phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));

            memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
            rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);
            break;
        }

        for (int i=0; i<ulObjectCount; i++)
            rv = pFunctionList->C_DestroyObject(hPatientSession, phObject[i]);
        if (rv != CKR_OK)
            QMessageBox::information(0,"", " не были удалены", QMessageBox::Ok);


        rvTemp = pFunctionList->C_FindObjectsFinal(hPatientSession);
        break;
    }
}


void Change_data::Save_change()
{

    CK_OBJECT_CLASS ocData = CKO_DATA;
    std::string str;
    const char* s;
    CK_BYTE *data;
    QString str_temp = "";
    CK_RV rv = CKR_OK;

    CK_UTF8CHAR label_1[] = "Certificate";
    CK_UTF8CHAR label_2[] = "Signature";
    CK_UTF8CHAR label_3[] = "FIO";
    CK_UTF8CHAR label_4[] = "Critical data";
    CK_UTF8CHAR application[] = "Medical card";
    QByteArray ba;

    CK_OBJECT_HANDLE hData;

    Delete(label_1, sizeof(label_1)-1);
    Delete(label_2, sizeof(label_2)-1);

    Delete(label_4, sizeof(label_4)-1);


    if (ui->Surname_line->text() != surname_str || ui->Name_line->text() != name_str || ui->MN_line->text() != MN_str)
    {
        QString idp = Read_ID();
        Delete(label_3, sizeof(label_3)-1);
        QString str_temp = "";

        CK_UTF8CHAR FIO_label[] = "FIO";

        str_temp = ui->Surname_line->text() + " " + ui->Name_line->text() + " " + ui->MN_line->text();

        ba = codec->fromUnicode(str_temp);
        s = ba.data();
        CK_BYTE *data_FIO = (CK_BYTE_PTR)(s);

        CK_ATTRIBUTE attrFIOTmpl[] = {
            {CKA_CLASS, &ocData, sizeof(ocData)},
            {CKA_LABEL, &FIO_label, sizeof(FIO_label)-1},
            {CKA_VALUE, data_FIO, str_temp.length()},
            {CKA_APPLICATION, &application, sizeof(application)-1},
            {CKA_TOKEN, &bTrue, sizeof(bTrue)},
            {CKA_PRIVATE, &bTrue, sizeof(bTrue)},
            {CKA_MODIFIABLE, &bFalse, sizeof(bFalse)}
        };

        rv = pFunctionList->C_CreateObject(hPatientSession, attrFIOTmpl, arraysize(attrFIOTmpl), &hData);
        if (rv != CKR_OK)
        {
            this->close();
            this->setResult(-1);

            return;
        }
        QSqlQuery query;
        QSqlRecord rec;
        query.exec(QString("UPDATE patient SET Фамилия = '%1' WHERE idp = '%2';").arg(ui->Surname_line->text()).arg(idp));
    }


    str_temp = ui->AB0_Rh_combo->currentText()+ ";";
    if (ui->SD_combo->currentText() == "0 - нет диабета")
    {
        str_temp += "0;";
    }
    else if (ui->SD_combo->currentText() == "диабет 1-го типа")
    {
        str_temp += "1;";
    }
    else if (ui->SD_combo->currentText() == "диабет 2-го типа")
    {
        str_temp += "2;";
    }
    if (ui->AID_combo->currentText() == "положительный")
        str_temp += "1;";
    else
        str_temp += "0;";
    if (ui->H_combo->currentText() == "0 - нет гепатита")
        str_temp += "0;";
    else
        str_temp += ui->H_combo->currentText() + ";";
    for (int i=0; i<ui->HZ_list->count(); i++)
    {
        if (i < ui->HZ_list->count()-1)
        {
            str_temp += ui->HZ_list->item(i)->text();
            str_temp += ",";
        }
        else
            str_temp += ui->HZ_list->item(i)->text();
    }
    str_temp += ";";
    for (int i=0; i<ui->AS_list->count(); i++)
    {
        if (i < ui->AS_list->count()-1)
        {
            str_temp += ui->AS_list->item(i)->text();
            str_temp += ",";
        }
        else
            str_temp += ui->AS_list->item(i)->text();
    }
    str_temp += ";";



    str = str_temp.toStdString();
    s = str.c_str();
    data = (CK_BYTE_PTR)s;

    CK_ATTRIBUTE attrDataTmpl[] = {
        {CKA_CLASS, &ocData, sizeof(ocData)},
        {CKA_LABEL, &label_4, sizeof(label_4)-1},
        {CKA_VALUE, data, str.length()},
        {CKA_APPLICATION, &application, sizeof(application)-1},
        {CKA_TOKEN, &bTrue, sizeof(bTrue)},
        {CKA_PRIVATE, &bFalse, sizeof(bFalse)}
    };



    rv = pFunctionList->C_CreateObject(hPatientSession, attrDataTmpl, arraysize(attrDataTmpl), &hData);
    if (rv != CKR_OK)
    {
        this->close();
        this->setResult(-1);

        return;
    }


    CK_UTF8CHAR      Doctor_priv_key[]    = {"Doctor private key"};

    CK_ATTRIBUTE attrPrivKeyFind[] = {
        {CKA_LABEL, &Doctor_priv_key, sizeof(Doctor_priv_key)-1}
    };

    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;

    while(TRUE)
    {
        rv = pFunctionList->C_FindObjectsInit(hDoctorSession, attrPrivKeyFind, arraysize(attrPrivKeyFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hDoctorSession, phObject, 100, &ulObjectCount);
        break;
    }
    rv = pFunctionList->C_FindObjectsFinal(hDoctorSession);
    CK_OBJECT_HANDLE hPrivateKey = phObject[0];


    CK_BYTE_PTR pbtSignature = NULL_PTR;                 // Указатель на буфер, содержащий подпись для исходных данных
    CK_ULONG ulSignatureSize = 0;
    rv = pFunctionList->C_SignInit(hDoctorSession,         // Хэндл сессии
                                   &ckmGOST_34_10_2001_with_34_11 ,   // Механизм подписи
                                   hPrivateKey );      // Хэндл закрытого ключа

    rv = pFunctionList->C_Sign(hDoctorSession,         // Хэндл сессии
                               data,            // Буфер с данными для подписи
                               str.length(), // Длина подписываемых данных
                               pbtSignature,       // Буфер с подписью
                               &ulSignatureSize);  // Длина подписи
    pbtSignature = (CK_BYTE*)malloc(ulSignatureSize);
    memset( pbtSignature, 0, ulSignatureSize * sizeof(CK_BYTE));
    rv = pFunctionList->C_Sign(hDoctorSession,         // Хэндл сессии
                               data,            // Буфер с данными для подписи
                               str.length(), // Длина подписываемых данных
                               pbtSignature,       // Буфер с подписью
                               &ulSignatureSize);  // Длина подписи

    CK_UTF8CHAR Sign_label[] = "Signature";
    CK_ATTRIBUTE attrSignTemp[] = {
        {CKA_CLASS, &ocData, sizeof(ocData)},
        {CKA_LABEL, &Sign_label, sizeof(Sign_label)-1},
        {CKA_VALUE, pbtSignature, ulSignatureSize},
        {CKA_APPLICATION, &application, sizeof(application)-1},
        {CKA_TOKEN, &bTrue, sizeof(bTrue)},
        {CKA_PRIVATE, &bFalse, sizeof(bFalse)},
        {CKA_MODIFIABLE, &bFalse, sizeof(bFalse)}
    };
    rv = pFunctionList->C_CreateObject(hPatientSession, attrSignTemp, arraysize(attrSignTemp), &hData);

    CK_UTF8CHAR      Doctor_pub_key[]    = {"Doctor public key"};

    CK_ATTRIBUTE attrPublicKeyFind[] = {
        {CKA_LABEL, &Doctor_pub_key, sizeof(Doctor_pub_key)-1}
    };

    phObject = NULL_PTR;
    ulObjectCount = 0;

    while(TRUE)
    {
        rv = pFunctionList->C_FindObjectsInit(hDoctorSession, attrPublicKeyFind, arraysize(attrPublicKeyFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hDoctorSession, phObject, 100, &ulObjectCount);
        break;
    }
    rv = pFunctionList->C_FindObjectsFinal(hDoctorSession);

    CK_ATTRIBUTE attrRead = {CKA_VALUE, NULL_PTR, NULL};

    while (TRUE)
    {
        rv = pFunctionList->C_GetAttributeValue(hDoctorSession, phObject[0], &attrRead, 1);

        attrRead.pValue = (CK_BYTE*)malloc(attrRead.ulValueLen);

        memset(attrRead.pValue,0, (attrRead.ulValueLen*sizeof(CK_BYTE)));

        rv = pFunctionList->C_GetAttributeValue(hDoctorSession, phObject[0], &attrRead, 1);
        break;
    }



    CK_UTF8CHAR Key_label[] = "Public key";
    CK_ATTRIBUTE attrGOSTPublicKey[] =
    {
        { CKA_CLASS, &ocPubKey, sizeof(ocPubKey)},                      // Объект открытого ключа ГОСТ Р 34.10-2001 (#1)
        { CKA_LABEL, &Key_label, sizeof(Key_label) - 1},        // Метка ключа
        { CKA_ID, &KeyPairIDGOST1, sizeof(KeyPairIDGOST1) - 1},         // Идентификатор ключевой пары #1 (должен совпадать у открытого и закрытого ключей)
        { CKA_KEY_TYPE, &ktGOST34_10_2001, sizeof(ktGOST34_10_2001)},   // Тип ключа
        { CKA_TOKEN, &bTrue, sizeof(bTrue)},                            // Ключ является объектом токена
        { CKA_PRIVATE, &bFalse, sizeof(bFalse)},                        // Ключ доступен без авторизации на токене
        { CKA_GOSTR3410_PARAMS, GOST3410_params_oid, sizeof(GOST3410_params_oid) },
        { CKA_GOSTR3411_PARAMS, GOST3411_params_oid, sizeof(GOST3411_params_oid) },  // Параметры алгоритма ГОСТ Р 34.11-1994
        { CKA_VALUE, attrRead.pValue, attrRead.ulValueLen}
    };

    //rv = pFunctionList->C_CreateObject(hPatientSession, attrGOSTPublicKey, arraysize(attrGOSTPublicKey), &hData);
    CK_UTF8CHAR Cert_label[] = "Certificate";

    ba = codec->fromUnicode(Doctors_FIO);
    s = ba.data();
    CK_BYTE *Doctor = (CK_BYTE_PTR)(s);

    CK_ATTRIBUTE attrCertPubKey[] = {
      {CKA_CLASS, &ocCert, sizeof(ocCert)},
      {CKA_CERTIFICATE_TYPE, &certType, sizeof(certType)},
      {CKA_TOKEN, &bTrue, sizeof(bTrue)},
      {CKA_LABEL, Cert_label, sizeof(Cert_label)-1},
      {CKA_SUBJECT, Doctor, Doctors_FIO.length()},
      {CKA_VALUE, attrRead.pValue, attrRead.ulValueLen}
    };
    rv = pFunctionList->C_CreateObject(hPatientSession, attrCertPubKey, arraysize(attrCertPubKey), &hData);

}



Change_data::~Change_data()
{
    delete ui;
}
*/
