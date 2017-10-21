#include "write_data.h"
#include "ui_write_data.h"
#include <QFile>
#include <QMessageBox>
#include <Common.h>
#include <QTextCodec>
#include "read_full_data.h"
#include "create_patient_db.h"
#include <QtSql>

extern HMODULE hModule;
extern CK_FUNCTION_LIST_PTR pFunctionList;
extern CK_C_GetFunctionList pfGetFunctionList;
extern CK_RV rv;
extern CK_SLOT_ID_PTR aSlots; //указатель на массив идентификаторов слотов
extern CK_ULONG ulSlotCount; //количество идентификаторов слотов в массиве
extern CK_SLOT_ID SlotID;
extern CK_SESSION_HANDLE hDoctorSession;
extern CK_SESSION_HANDLE hPatientSession;


extern QString Read_ID();
extern QString Doctors_FIO;

QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

QMap<QString, QString> CDs, AIs;



extern QSqlDatabase db;



extern void Parse_for_list(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index);

write_data::write_data(QWidget *parent, QString critical_data_str, QString surname_str, QString name_str, QString MN_str) :
    QDialog(parent),
    ui(new Ui::write_data)
{
    ui->setupUi(this);
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
    connect(ui->Write_on_card, SIGNAL(clicked(bool)), this, SLOT(Save_data()));
    if (critical_data_str != "")
    {
        ui->title_label->setText("Изменение данных на карте");
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
        QStringList temp = full_data.at(4).simplified().split(",");
        for(int i=0; i<temp.size(); i++)
        {
            ui->HZ_list->addItem(temp[i]);
        }
        temp.clear();
        temp = full_data.at(5).simplified().split(",");
        for(int i=0; i<temp.size(); i++)
        {
            ui->AS_list->addItem(temp[i]);
        }

        ui->HZ_combo->setCurrentIndex(0);
        ui->AS_combo->setCurrentText("");
    }
}

void write_data::Activate_HZ_group()
{
    if (sender() == ui->HZ_check)
    {
        if (ui->HZ_check->isChecked() == true)
        {
            ui->HZ_combo->setCurrentText("");
            if (ui->HZ_list->count() < 10)
                ui->HZ_group->setEnabled(true);
            else
            {
                QMessageBox::information(0,"","Нельзя добавлять больше 10 заболеваний", QMessageBox::Ok);
                ui->HZ_check->setChecked(false);
            }

        }
        else
            ui->HZ_group->setEnabled(false);
    }
    if (sender() == ui->AS_check)
    {
        if (ui->AS_check->isChecked() == true)
        {
            ui->AS_combo->setCurrentText("");
            if (ui->AS_list->count() < 10)
                ui->AS_group->setEnabled(true);
            else
            {
                QMessageBox::information(0,"","Нельзя добавлять больше 10 веществ", QMessageBox::Ok);
                ui->AS_check->setChecked(false);
            }
        }
        else
            ui->AS_group->setEnabled(false);
    }
}


void write_data::fill_comboBox(QComboBox* combo, QString filename, QMap<QString,QString> &map)
{
    QFile f(filename);
    if(f.open(QIODevice::ReadOnly) == false)
        QMessageBox::information(0, "", "Failed", QMessageBox::Ok);
    QByteArray filetext = f.readAll();
    f.close();
    QString str_all_file(filetext); //преобразуем весь файл в строку
    str_all_file = str_all_file.simplified(); //удаляем спецсимволы
    QStringList illness_list = str_all_file.split(';'); //разделяем на болезни и коды
    combo->addItem("");
    for (int i=0; i<illness_list.length(); i++)
    {
        QString code_and_name_temp = illness_list.at(i);
        QStringList code_and_name_list = code_and_name_temp.split(',');
        map.insert(code_and_name_list.at(1),code_and_name_list.at(0));
        combo->addItem(code_and_name_list.at(1));
    }
}

void write_data::Show_HZ_code(QString str)
{
    if (sender() == ui->HZ_combo)
    {
        if (ui->HZ_combo->currentText() == "")
        {
            ui->HZ_edit->text() = "";
                return;
        }
        else
            ui->HZ_edit->setText(CDs.find(str).value());
    }
    if (sender() == ui->AS_combo)
    {
        if (ui->AS_combo->currentText() == "")
        {
            ui->AS_edit->text() = "";
                return;
        }
        else
            ui->AS_edit->setText(AIs.find(str).value());
    }
}

void write_data::Add_to_List()
{
    int f = 0;
    if (sender() == ui->HZ_button)
    {

            if (ui->HZ_edit->text() == "")
            {
                QMessageBox::information(0,"", "Выберите заболевание", QMessageBox::Ok);
            }
            else
            {
                for (int i=0; i<ui->HZ_list->count(); i++)
                {
                    if (ui->HZ_edit->text() == ui->HZ_list->item(i)->text())
                        f++;
                }
                if (f == 0)
                {
                    ui->HZ_list->addItem(ui->HZ_edit->text());
                    ui->HZ_check->setChecked(false);
                    ui->HZ_edit->setText("");
                    ui->HZ_group->setEnabled(false);
                }
                else
                    QMessageBox::information(0,"", "Такой заболевание уже добавлено", QMessageBox::Ok);
            }

     f = 0;
    }
    if (sender() == ui->AS_button)
    {

        if (ui->AS_edit->text() == "")
        {
            QMessageBox::information(0,"", "Выберите лекарственный препарат", QMessageBox::Ok);
        }
        else
        {
            for (int i=0; i<ui->AS_list->count(); i++)
            {
                if (ui->AS_edit->text() == ui->AS_list->item(i)->text())
                    f++;
            }
            if (f == 0)
            {
                ui->AS_list->addItem(ui->AS_edit->text());
                ui->AS_check->setChecked(false);
                ui->AS_edit->setText("");
                ui->AS_group->setEnabled(false);
            }
            else
                QMessageBox::information(0,"", "Такой лекарственный препарат уже добавлен", QMessageBox::Ok);
        }
        f = 0;
    }
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

        /*Деинициализировать операцию поиска*/
        rvTemp = pFunctionList->C_FindObjectsFinal(hPatientSession);
        break;
    }
}


void write_data::Save_data()
{
    if (critical_data_str.isEmpty() == false)
    {
        CK_UTF8CHAR label_1[] = "Certificate";
        CK_UTF8CHAR label_2[] = "Signature";
        CK_UTF8CHAR label_3[] = "FIO";
        CK_UTF8CHAR label_4[] = "Critical data";


        Delete(label_1, sizeof(label_1)-1);
        Delete(label_2, sizeof(label_2)-1);
        Delete(label_3, sizeof(label_3)-1);
        Delete(label_4, sizeof(label_4)-1);
    }
    QSqlQuery query;
    QString str_temp = ui->AB0_Rh_combo->currentText()+ ";";
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


    CK_OBJECT_CLASS ocData = CKO_DATA;
    CK_UTF8CHAR label[] = "Critical data";
    CK_UTF8CHAR application[] = "Medical_card";
    std::string str = str_temp.toStdString();
    const char* s = str.c_str();
    CK_BYTE *data = (CK_BYTE_PTR)(s);
    CK_RV rv = CKR_OK;

    CK_ATTRIBUTE attrDataTmpl[] = {
        {CKA_CLASS, &ocData, sizeof(ocData)},
        {CKA_LABEL, &label, sizeof(label)-1},
        {CKA_VALUE, data, str.length()},
        {CKA_APPLICATION, &application, sizeof(application)-1},
        {CKA_TOKEN, &bTrue, sizeof(bTrue)},
        {CKA_PRIVATE, &bFalse, sizeof(bFalse)}
    };
    QString FIO_str = ui->Surname_line->text() + " " + ui->Name_line->text() + " " + ui->MN_line->text();
    CK_UTF8CHAR FIO_label[] = "FIO";
    QByteArray ba = codec->fromUnicode(FIO_str);
    s = ba.data();
    CK_BYTE *data_FIO = (CK_BYTE_PTR)(s);

    CK_ATTRIBUTE attrFIOTmpl[] = {
        {CKA_CLASS, &ocData, sizeof(ocData)},
        {CKA_LABEL, &FIO_label, sizeof(FIO_label)-1},
        {CKA_VALUE, data_FIO, FIO_str.length()},
        {CKA_APPLICATION, &application, sizeof(application)-1},
        {CKA_TOKEN, &bTrue, sizeof(bTrue)},
        {CKA_PRIVATE, &bTrue, sizeof(bTrue)},
        {CKA_MODIFIABLE, &bFalse, sizeof(bFalse)}
    };

    CK_UTF8CHAR ID_label[] = "ID";
    CK_BYTE data_ID[] = "9379992";

    CK_ATTRIBUTE attrIDTmpl[] = {
        {CKA_CLASS, &ocData, sizeof(ocData)},
        {CKA_LABEL, &ID_label, sizeof(ID_label)-1},
        {CKA_VALUE, data_ID, sizeof(data_ID)-1},
        {CKA_APPLICATION, &application, sizeof(application)-1},
        {CKA_TOKEN, &bTrue, sizeof(bTrue)},
        {CKA_PRIVATE, &bTrue, sizeof(bTrue)},
        {CKA_MODIFIABLE, &bFalse, sizeof(bFalse)}
    };


    CK_OBJECT_HANDLE hData;
    rv = pFunctionList->C_CreateObject(hPatientSession, attrFIOTmpl, arraysize(attrDataTmpl), &hData);
    if (rv != CKR_OK)
    {
        this->close();
        this->setResult(-1);

        return;
    }
    rv = pFunctionList->C_CreateObject(hPatientSession, attrDataTmpl, arraysize(attrDataTmpl), &hData);
    if (rv != CKR_OK)
    {
        this->close();
        this->setResult(-1);

        return;
    }
    if (critical_data_str.isEmpty())
        rv = pFunctionList->C_CreateObject(hPatientSession, attrIDTmpl, arraysize(attrIDTmpl), &hData);
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
    /* Подписание критических данных */

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

    /*******************************************************Тут остановочка***************************************/
    /*************************************************************************************************************/
    /*************************************************************************************************************/
    QString str_ID = Read_ID();
    QStringList temp = FIO_str.simplified().split(" ");
    QString surname = temp[0];
    QString name = temp[1];
    QString mn = temp[2];
    QSqlRecord rec;
    QString a = QString("SELECT Фамилия FROM patient WHERE idp = %1;").arg(str_ID);
    query.exec(a);
    rec = query.record();
    query.next();
    if (query.value(0).toString().isEmpty())
    {
        int n = QMessageBox::information(0,"","Пациента нет в базе данных! Хотите добавить его сейчас?", QMessageBox::Yes, QMessageBox::No);
        if (n == QMessageBox::Yes)
        {
            Create_patient_DB *cpdb = new Create_patient_DB(0, str_ID, surname, name, mn);
            cpdb->exec();
            delete cpdb;
        }
        this->close();
    }
    bool ok = query.exec(QString("UPDATE patient SET Фамилия = '%1', Имя = '%2', Отчество = '%3' WHERE idp = %4;").arg(surname).arg(name).arg(mn).arg(str_ID));
    this->close();

}

write_data::~write_data()
{
    delete ui;
}
