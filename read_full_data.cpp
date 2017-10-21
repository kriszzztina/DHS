#include "read_full_data.h"
#include "ui_read_full_data.h"
#include "authorization_wnd.h"
#include <QMessageBox>
#include <QFile>
#include <QTextCodec>
#include <QtSql>
#include <QDebug>
#include "change_data.h"
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


extern bool CheckSign();

extern QSqlDatabase db;

extern QTextCodec *codec;

QMap<QString,QString> HZ_map, AS_map;

void Parse_for_list(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index);


/*********************Получаем критические данные с карты*********************************************/
/*************************************************************************************/
QString Read_critical()
{
    QString result = "";
    CK_UTF8CHAR label[] = "Critical data";
    CK_ATTRIBUTE attrFind[] = {
        {CKA_LABEL, &label, sizeof(label)-1}
    };


    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;

    while(TRUE)
    {

        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrFind, arraysize(attrFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);
        if (ulObjectCount == 0)
        {
            rv = pFunctionList->C_FindObjectsFinal(hPatientSession);
            return result;
        }

        CK_ATTRIBUTE attrRead = {CKA_VALUE, NULL_PTR, NULL};

        while (TRUE)
        {
            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);

            attrRead.pValue = (CK_BYTE*)malloc(attrRead.ulValueLen);

            memset(attrRead.pValue,0, (attrRead.ulValueLen*sizeof(CK_BYTE)));

            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);
            for (int i = 0; i<attrRead.ulValueLen; i++)
            {
                result += QString(((CK_BYTE*)attrRead.pValue)[i]);
            }
            rv = pFunctionList->C_FindObjectsFinal(hPatientSession);
            return result;
        }
    }
}


/*********************Получаем FIO с карты*********************************************/
/*************************************************************************************/
QString Read_FIO()
{
    QString result = "";
    CK_UTF8CHAR label[] = "FIO";
    CK_ATTRIBUTE attrFind[] = {
        {CKA_LABEL, &label, sizeof(label)-1}
    };


    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;

    while(TRUE)
    {

        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrFind, arraysize(attrFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);

        if (ulObjectCount == 0)
        {
            return result;
        }
        CK_ATTRIBUTE attrRead = {CKA_VALUE, NULL_PTR, NULL};

        while (TRUE)
        {
            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);

            attrRead.pValue = (CK_BYTE*)malloc(attrRead.ulValueLen);

            memset(attrRead.pValue,0, (attrRead.ulValueLen*sizeof(CK_BYTE)));

            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);

            result = codec->toUnicode((char*)attrRead.pValue,attrRead.ulValueLen);

            rv = pFunctionList->C_FindObjectsFinal(hPatientSession);
            return result;
        }
    }
}

/*********************Получаем ID с карты*********************************************/
/*************************************************************************************/
QString Read_ID()
{
    QString result = "";
    CK_UTF8CHAR label[] = "ID";
    CK_ATTRIBUTE attrFind[] = {
        {CKA_LABEL, &label, sizeof(label)-1}
    };


    CK_OBJECT_HANDLE_PTR phObject = NULL_PTR;
    CK_ULONG ulObjectCount = 0;

    while(TRUE)
    {

        rv = pFunctionList->C_FindObjectsInit(hPatientSession, attrFind, arraysize(attrFind));

        phObject = (CK_OBJECT_HANDLE*)malloc(100*sizeof(CK_OBJECT_HANDLE));
        memset(phObject, 0, 100*sizeof(CK_OBJECT_HANDLE));
        rv = pFunctionList->C_FindObjects(hPatientSession, phObject, 100, &ulObjectCount);

        if (ulObjectCount == 0)
        {
            rv = pFunctionList->C_FindObjectsFinal(hPatientSession);
            return result;
        }
        CK_ATTRIBUTE attrRead = {CKA_VALUE, NULL_PTR, NULL};

        while (TRUE)
        {
            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);

            attrRead.pValue = (CK_BYTE*)malloc(attrRead.ulValueLen);

            memset(attrRead.pValue,0, (attrRead.ulValueLen*sizeof(CK_BYTE)));

            rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrRead, 1);

            result = codec->toUnicode((char*)attrRead.pValue,attrRead.ulValueLen);

            rv = pFunctionList->C_FindObjectsFinal(hPatientSession);
            return result;
        }
    }
}


void Read_full_data::Open_change()
{
    write_data *cd = new write_data(0, Read_critical(), ui->Surname_line->text(), ui->Name_line->text(), ui->M_line->text());
    cd->exec();
    Update();
    delete cd;
}


void Read_full_data::Update()
{
    QStringList FIO_str = Read_FIO().split(" ");
    QString surname_str = FIO_str[0];
    QString name_str = FIO_str[1];
    QString MN_str = FIO_str[2];
    QString ID_str = Read_ID();

    ui->HZ_list->clear();
    ui->AS_list->clear();

    ui->Surname_line->setText(surname_str);
    ui->Name_line->setText(name_str);
    ui->M_line->setText(MN_str);

    QStringList full_data = Read_critical().simplified().split(";");
    ui->BG_line->setText(full_data.at(0));
    switch (full_data.at(1).toInt())
    {
        case 0:
            ui->SD_line->setText("нет диабета");
            break;
        case 1:
            ui->SD_line->setText("диабет 1-го типа");
            break;
        case 2:
            ui->SD_line->setText("диабет 2-го типа");
            break;
    }
    switch (full_data.at(2).toInt())
    {
        case 0:
            ui->AID_line->setText("отрицательный");
            break;
        case 1:
            ui->AID_line->setText("положительный");
            break;
    }
    ui->H_line->setText(full_data.at(3));
    Parse_for_list(ui->HZ_list, ":/res/MKB.txt", HZ_map, 4);
    Parse_for_list(ui->AS_list, ":/res/ATC.txt", AS_map, 5);

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


    CK_ATTRIBUTE attrReadCert = {CKA_SUBJECT, NULL_PTR, NULL};

    while (TRUE)
    {
        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadCert, 1);

        attrReadCert.pValue = (CK_BYTE*)malloc(attrReadCert.ulValueLen);

        memset(attrReadCert.pValue,0, (attrReadCert.ulValueLen*sizeof(CK_BYTE)));

        rv = pFunctionList->C_GetAttributeValue(hPatientSession, phObject[0], &attrReadCert, 1);

        QString temp = codec->toUnicode((char*)attrReadCert.pValue,attrReadCert.ulValueLen);

        ui->Author_line->setText(temp);

        break;
    }

    QSqlQuery query;
    QSqlRecord rec;

    /***********************Обновляем вкладку главная*******************************************/
    query.exec(QString("SELECT * FROM patient WHERE idp = '%1';").arg(ID_str));
    rec = query.record();
    query.next();


    ui->Surname->setText(query.value(1).toString());

    ui->Name->setText(query.value(2).toString());

    ui->MN->setText(query.value(3).toString());
    ui->SMO->setText(query.value(4).toString());
    ui->Policy_number->setText(query.value(5).toString());
    ui->CB->setText(query.value(6).toString());
    ui->SNILS->setText(query.value(7).toString());
    if (query.value(8).toString() == "M")
        ui->Male_radio->setChecked(true);
    else
        ui->Female_radio->setChecked(true);
    ui->Birthday->setText(query.value(9).toString());
    ui->APM_area->setText(query.value(10).toString());
    ui->APM_region->setText(query.value(11).toString());
    ui->APM_locality->setText(query.value(12).toString());
    ui->APM_street->setText(query.value(13).toString());
    ui->APM_house->setText(query.value(14).toString());
    ui->APM_housing->setText(query.value(15).toString());
    ui->APM_appartment->setText(query.value(16).toString());
    ui->R_area->setText(query.value(17).toString());
    ui->R_region->setText(query.value(18).toString());
    ui->R_locality->setText(query.value(19).toString());
    ui->R_street->setText(query.value(20).toString());
    ui->R_house->setText(query.value(21).toString());
    ui->R_housing->setText(query.value(22).toString());
    ui->R_appartment->setText(query.value(23).toString());
    ui->Home_phone->setText(query.value(24).toString());
    ui->Official_phone->setText(query.value(25).toString());
    ui->Document_for_l->setText(query.value(26).toString());
    ui->disability->setText(query.value(27).toString());
    ui->Job->setText(query.value(28).toString());
    ui->Profession->setText(query.value(29).toString());
    ui->Post->setText(query.value(30).toString());
    ui->Izh->setText(query.value(31).toString());

}

Read_full_data::Read_full_data(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Read_full_data)
{
    ui->setupUi(this);
    connect(ui->Add_to_BD,SIGNAL(clicked(bool)),this,SLOT(Add_to_DB()));
    connect(ui->Change,SIGNAL(clicked(bool)),this,SLOT(Open_change()));
    QString critical_data_str = Read_critical();
    QStringList FIO_str = Read_FIO().split(" ");
    QString surname_str = FIO_str[0];
    QString name_str = FIO_str[1];
    QString MN_str = FIO_str[2];
    QString ID_str = Read_ID();
    ui->Male_radio->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->Male_radio->setFocusPolicy(Qt::NoFocus);
    ui->Female_radio->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->Female_radio->setFocusPolicy(Qt::NoFocus);
    if (ID_str == "")
    {
        int ans = QMessageBox::information(0,"Информация не найдена", "Идентифицирующая информация о пациенте не найдена.\nКарту необходимо перезаписать. Хотите сделать это сейчас?", QMessageBox::Yes, QMessageBox::No);
        if (ans == QMessageBox::Yes)
        {
           write_data *cd = new write_data(0, critical_data_str, surname_str, name_str, MN_str);
           cd->exec();
           delete cd;
        }
    }
    QSqlQuery query;
    QSqlRecord rec;
    query.exec(QString("SELECT Фамилия FROM patient WHERE idp = '%1';").arg(ID_str));
    rec = query.record();
    query.next();
    if (query.value(0) == "")
    {
        int n = QMessageBox::information(0,"", "Пациента нет в базе данных! Хотите добавить сейчас?", QMessageBox::Yes, QMessageBox::No);
        if (n = QMessageBox::Yes)
        {
            Create_patient_DB *cpdb = new Create_patient_DB(0, ID_str, surname_str, name_str, MN_str, false);
            cpdb->exec();
            delete cpdb;
        }
    }


    ui->IDP->setText(ID_str);
    ui->IDP_2->setText(ID_str);
    Update();
}


void Read_full_data::Add_to_DB()
{
    QStringList FIO_str = Read_FIO().split(" ");
    QString surname_str = FIO_str[0];
    QString name_str = FIO_str[1];
    QString MN_str = FIO_str[2];
    QString ID_str = Read_ID();
    Create_patient_DB *cpdb = new Create_patient_DB(0, ID_str, surname_str, name_str, MN_str, true);
    cpdb->exec();
    Update();
    delete cpdb;
}

void Parse_for_list(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index)
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
    bool empt = maps.isEmpty();
    for (int i=0; i<temp.count(); i++)
    {
        for (int j=0; j<illness_list.length(); j++)
        {

            QString code_and_name_temp = illness_list.at(j);
            QStringList code_and_name_list = code_and_name_temp.split(',');
            if (empt)
                maps.insert(code_and_name_list.at(1),code_and_name_list.at(0));
            if (code_and_name_list.at(0) == temp.at(i))
            {
                list->addItem(code_and_name_list.at(1));
                break;
            }
        }
    }
}


Read_full_data::~Read_full_data()
{
    delete ui;
}
