#include "mainwindow.h"
#include <QApplication>
#include <Common.h>
#include <QMessageBox>
#include "authorization_wnd.h"
#include <QtSql>
#include <stdlib.h>
#include <QDebug>


HMODULE hModule = NULL_PTR;
CK_FUNCTION_LIST_PTR pFunctionList = NULL_PTR;
CK_C_GetFunctionList pfGetFunctionList = NULL_PTR;
CK_RV rv = CKR_OK;
CK_SLOT_ID_PTR aSlots = NULL_PTR; //указатель на массив идентификаторов слотов
CK_ULONG ulSlotCount = 0; //количество идентификаторов слотов в массиве
CK_SLOT_ID SlotID = 0;
CK_SESSION_HANDLE hDoctorSession = NULL_PTR;
CK_SESSION_HANDLE hPatientSession = NULL_PTR;
QSqlDatabase db;

QString Doctors_FIO;

enum Get_Token_result{Success, Fail, Cancel};
extern Get_Token_result GetToken(char slots_count);

void InitLib()
{
    while(TRUE)
    {
        hModule = LoadLibrary(PKCS11ECP_LIBRARY_NAME);
        if (hModule == NULL_PTR)
        {
            break;
        }
        pfGetFunctionList = (CK_C_GetFunctionList)GetProcAddress(hModule, "C_GetFunctionList");
        if (pfGetFunctionList == NULL_PTR)
        {
            break;
        }
        rv = pfGetFunctionList(&pFunctionList);
        if (rv != CKR_OK)
        {
            break;
        }
        break;
    }

    /* инициализация библиотеки*/
    rv = pFunctionList->C_Initialize(NULL_PTR);
}


bool Challenge()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("medical_card");
    db.setUserName("root");
    db.setPassword("root");
    bool ok = db.open();
    srand(time(NULL));
    rand();
    int n = rand();
    QString s = QString::number(n);
    const char* n_str = s.toStdString().c_str();
    CK_BYTE_PTR data = (CK_BYTE_PTR)n_str;

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


    CK_BYTE_PTR pHash = NULL;
    CK_ULONG ulHashSize = 0;

    while (TRUE)
    {

        rv = pFunctionList->C_DigestInit(hDoctorSession, &ckmGOST34_11_94Mech);

        rv = pFunctionList->C_Digest(hDoctorSession,
                                     data,
                                     s.length(),
                                     pHash,
                                     &ulHashSize);


        pHash = (CK_BYTE*)malloc(ulHashSize);
        memset(pHash, 0, (ulHashSize * sizeof(CK_BYTE)));


        rv = pFunctionList->C_Digest(hDoctorSession,
                                     data,
                                     s.length(),
                                     pHash,
                                     &ulHashSize);

        break;
    }


    CK_BYTE_PTR pbtSignature = NULL_PTR;                 // Указатель на буфер, содержащий подпись для исходных данных
    CK_ULONG ulSignatureSize = 0;
    rv = pFunctionList->C_SignInit(hDoctorSession,         // Хэндл сессии
                                   &ckmGOST_34_10_2001SigVerMech,   // Механизм подписи
                                   hPrivateKey );      // Хэндл закрытого ключа

    rv = pFunctionList->C_Sign(hDoctorSession,         // Хэндл сессии
                               pHash,            // Буфер с данными для подписи
                               ulHashSize, // Длина подписываемых данных
                               pbtSignature,       // Буфер с подписью
                               &ulSignatureSize);  // Длина подписи
    pbtSignature = (CK_BYTE*)malloc(ulSignatureSize);
    memset( pbtSignature, 0, ulSignatureSize * sizeof(CK_BYTE));
    rv = pFunctionList->C_Sign(hDoctorSession,         // Хэндл сессии
                               pHash,            // Буфер с данными для подписи
                               ulHashSize, // Длина подписываемых данных
                               pbtSignature,       // Буфер с подписью
                               &ulSignatureSize);  // Длина подписи


    /******************************************************************/
    /******************************************************************/

    QSqlQuery query;

    CK_TOKEN_INFO tokenInfo;

    rv = pFunctionList->C_GetTokenInfo(aSlots[0], &tokenInfo);
    QString serial = "";
    for (int i=0; i<arraysize(tokenInfo.serialNumber); i++)
    {
        if (tokenInfo.serialNumber[i] > 0xF)
            serial += QString::number(tokenInfo.serialNumber[i],16);
        else
            serial += "0" + QString::number(tokenInfo.serialNumber[i], 16);
    }
    QSqlRecord rec;
    query.exec("SELECT Public_key, Фамилия, Имя, Отчество FROM doctor WHERE idd = 100001;");
    rec = query.record();
    query.next();

    QString PubKeyStr = query.value(0).toString();
    if(PubKeyStr.isEmpty())
        return false;
    Doctors_FIO = query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString();
    QString temp;
    CK_BYTE PubKey[64];
    for (int i=0; i<128; i+=2)
    {
        temp = PubKeyStr.mid(i,2);
        PubKey[i/2] = temp.toInt(NULL,16);
    }

    CK_ATTRIBUTE PublicKeyTmpl[] = {
        { CKA_CLASS, &ocPubKey, sizeof(ocPubKey)},                      // Объект открытого ключа
        { CKA_LABEL, &PubLabelGOST1, sizeof(PubLabelGOST1)-1},              // Метка ключа
        { CKA_ID, &KeyPairIDGOST1, sizeof(KeyPairIDGOST1)-1},                     // Идентификатор ключевой пары
        { CKA_KEY_TYPE, &ktGOST34_10_2001, sizeof(ktGOST34_10_2001)},                     // Тип ключа
        { CKA_TOKEN, &bFalse, sizeof(bFalse)},                            // Ключ является объектом токена
        { CKA_PRIVATE, &bFalse, sizeof(bFalse)},                        // Ключ доступен без авторизации на токене
        { CKA_DERIVE, &bFalse, sizeof(bFalse)},                           // Ключ поддерживает деривацию (из него могут быть получены другие ключи)
        { CKA_GOSTR3410_PARAMS, GOST3410_params_oid, sizeof(GOST3410_params_oid)},// Параметры алгоритма
        { CKA_GOSTR3411_PARAMS, GOST3411_params_oid, sizeof(GOST3411_params_oid)},
        { CKA_VALUE, PubKey, sizeof(PubKey)} // Параметры алгоритма
    };
    CK_OBJECT_HANDLE hPubKey;
    rv = pFunctionList->C_CreateObject(hDoctorSession, PublicKeyTmpl, arraysize(PublicKeyTmpl),&hPubKey);

    //операция хеширования



    rv = pFunctionList->C_VerifyInit(hDoctorSession,       // Хэндл сессии
                                     &ckmGOST_34_10_2001SigVerMech,   // Механизм подписи
                                     hPubKey);

    rv = pFunctionList->C_Verify(hDoctorSession,           // Хэндл сессии
                                 pHash,            // Буфер с значением хеша исходного сообщения
                                 ulHashSize,        // Длина буфера
                                 pbtSignature,      // Буфер с подписью
                                 ulSignatureSize);
    if (rv != CKR_OK)
        return false;
    return true;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitLib();


    while (true)
    {
        Get_Token_result res = GetToken(1);
        if (res == Success)
            break;
        else if (res == Cancel)
            exit(-1);
    }

    rv = pFunctionList->C_OpenSession(aSlots[0], CKF_SERIAL_SESSION|CKF_RW_SESSION, NULL_PTR, NULL_PTR, &hDoctorSession);

    Authorization_wnd *wnd = new Authorization_wnd(0, 0, &hDoctorSession);
    wnd->exec();
    if (wnd->GetResult() == false)
    {
        delete wnd;
        QMessageBox::critical(0,"Error", "Ошибка авторизации", QMessageBox::Ok);
        exit(-1);
    }
    delete wnd;
    if(Challenge() == false)
    {
        QMessageBox::critical(0,"Error","Никакой ты не докторишка!",QMessageBox::Ok);
        exit(-1);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
