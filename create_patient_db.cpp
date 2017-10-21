#include "create_patient_db.h"
#include "ui_create_patient_db.h"
#include <QtSql>
#include <QMessageBox>
#include "Common.h"


extern void Delete(CK_UTF8CHAR *label, size_t size);

Create_patient_DB::Create_patient_DB(QWidget *parent, QString str_ID, QString surname, QString name, QString MN, bool upd) :
    QDialog(parent),
    ui(new Ui::Create_patient_DB)
{
    ui->setupUi(this);
    this->upd = upd;
    this->surname = surname;
    this->name = name;
    mn = MN;
    ui->IDP_5->setText(str_ID);
    ui->Surname_3->setText(surname);
    ui->Name_3->setText(name);
    ui->MN_3->setText(MN);
    connect(ui->Add_to_BD_3, SIGNAL(clicked(bool)), this, SLOT(Add_to_DB()));

    if (upd)
    {
        QSqlQuery query;
        QSqlRecord rec;
        query.exec(QString("SELECT * FROM patient WHERE idp = '%1';").arg(str_ID));
        rec = query.record();
        query.next();



        ui->SMO_3->setText(query.value(4).toString());
        ui->Policy_number_3->setText(query.value(5).toString());
        ui->CB_3->setText(query.value(6).toString());
        ui->SNILS_3->setText(query.value(7).toString());
        if (query.value(8).toString() == "M")
            ui->Male_radio_3->setChecked(true);
        else
            ui->Female_radio_3->setChecked(true);
        ui->Birthday_3->setText(query.value(9).toString());
        ui->APM_area_3->setText(query.value(10).toString());
        ui->APM_region_3->setText(query.value(11).toString());
        ui->APM_locality_3->setText(query.value(12).toString());
        ui->APM_street_3->setText(query.value(13).toString());
        ui->APM_house_3->setText(query.value(14).toString());
        ui->APM_housing_3->setText(query.value(15).toString());
        ui->APM_appartment_3->setText(query.value(16).toString());
        ui->R_area_3->setText(query.value(17).toString());
        ui->R_region_3->setText(query.value(18).toString());
        ui->R_locality_3->setText(query.value(19).toString());
        ui->R_street_3->setText(query.value(20).toString());
        ui->R_house_3->setText(query.value(21).toString());
        ui->R_housing_3->setText(query.value(22).toString());
        ui->R_appartment_3->setText(query.value(23).toString());
        ui->Home_phone_3->setText(query.value(24).toString());
        ui->Official_phone_3->setText(query.value(25).toString());
        ui->Document_for_l_3->setText(query.value(26).toString());
        ui->disability_3->setText(query.value(27).toString());
        ui->Job_3->setText(query.value(28).toString());
        ui->Profession_3->setText(query.value(29).toString());
        ui->Post_3->setText(query.value(30).toString());
        ui->Izh_3->setText(query.value(31).toString());

    }
}

void Create_patient_DB::Add_to_DB()
{
    QSqlQuery query;
    QSqlRecord rec;
    QString sex;
    bool f;
    if (ui->Female_radio_3->isChecked())
        sex = "Ж";
    if (ui->Male_radio_3->isChecked())
        sex = "M";
    if (upd == false)
    {
        f = query.exec(QString("INSERT INTO patient (idp, Фамилия, Имя, Отчество, Страховая_мед_организация, Номер_полиса, Код_льготы,"
                                 "СНИЛС, Пол, Дата_рождения, Пост_адрес_область, Пост_адрес_район, Пост_адрес_нас_пункт, Пост_адрес_улица,"
                                 "Пост_адрес_дом, Пост_адрес_корпус, Пост_адрес_кв, Регистрация_область, Регистрация_район,"
                                 "Регистрация_нас_пункт, Регистрация_улица, Регистрация_дом, Регистрация_корпус, Регистрация_кв,"
                                 "Телефон_домашний, Телефон_служебный, Документ_на_льготы, Инвалидность, Место_работы, Профессия, Должность, Иждивенец)"
                                 " VALUES ('%1', '%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20',"
                                 "'%21','%22','%23','%24','%25','%26','%27','%28','%29','%30','%31','%32');").arg(ui->IDP_5->text())
                                 .arg(ui->Surname_3->text())
                                 .arg(ui->Name_3->text())
                                 .arg(ui->MN_3->text())
                                 .arg(ui->SMO_3->text())
                                 .arg(ui->Policy_number_3->text())
                                 .arg(ui->CB_3->text())
                                 .arg(ui->SNILS_3->text())
                                 .arg(sex)
                                 .arg(ui->Birthday_3->text())
                                 .arg(ui->APM_area_3->text())
                                 .arg(ui->APM_region_3->text())
                                 .arg(ui->APM_locality_3->text())
                                 .arg(ui->APM_street_3->text())
                                 .arg(ui->APM_house_3->text())
                                 .arg(ui->APM_housing_3->text())
                                 .arg(ui->APM_appartment_3->text())
                                 .arg(ui->R_area_3->text())
                                 .arg(ui->R_region_3->text())
                                 .arg(ui->R_locality_3->text())
                                 .arg(ui->R_street_3->text())
                                 .arg(ui->R_house_3->text())
                                 .arg(ui->R_housing_3->text())
                                 .arg(ui->R_appartment_3->text())
                                 .arg(ui->Home_phone_3->text())
                                 .arg(ui->Official_phone_3->text())
                                 .arg(ui->Document_for_l_3->text())
                                 .arg(ui->disability_3->text())
                                 .arg(ui->Job_3->text())
                                 .arg(ui->Profession_3->text())
                                 .arg(ui->Post_3->text())
                                 .arg(ui->Izh_3->text()));
    }
    else
    {
        f = query.exec(QString("UPDATE patient SET Страховая_мед_организация = '%1', Номер_полиса = '%2', Код_льготы = '%3',"
                                 "СНИЛС = '%4', Пол = '%5', Дата_рождения = '%6', Пост_адрес_область = '%7', Пост_адрес_район = '%8', Пост_адрес_нас_пункт = '%9', Пост_адрес_улица = '%10',"
                                 "Пост_адрес_дом = '%11', Пост_адрес_корпус = '%12', Пост_адрес_кв = '%13', Регистрация_область = '%14', Регистрация_район = '%15',"
                                 "Регистрация_нас_пункт = '%16', Регистрация_улица = '%17', Регистрация_дом = '%18', Регистрация_корпус = '%19', Регистрация_кв = '%20',"
                                 "Телефон_домашний = '%21', Телефон_служебный = '%22', Документ_на_льготы = '%23', Инвалидность ='%24', Место_работы = '%25', Профессия = '%26', Должность = '%27', Иждивенец = '%28' WHERE idp = '%29';")
                                 .arg(ui->SMO_3->text())
                                 .arg(ui->Policy_number_3->text())
                                 .arg(ui->CB_3->text())
                                 .arg(ui->SNILS_3->text())
                                 .arg(sex)
                                 .arg(ui->Birthday_3->text())
                                 .arg(ui->APM_area_3->text())
                                 .arg(ui->APM_region_3->text())
                                 .arg(ui->APM_locality_3->text())
                                 .arg(ui->APM_street_3->text())
                                 .arg(ui->APM_house_3->text())
                                 .arg(ui->APM_housing_3->text())
                                 .arg(ui->APM_appartment_3->text())
                                 .arg(ui->R_area_3->text())
                                 .arg(ui->R_region_3->text())
                                 .arg(ui->R_locality_3->text())
                                 .arg(ui->R_street_3->text())
                                 .arg(ui->R_house_3->text())
                                 .arg(ui->R_housing_3->text())
                                 .arg(ui->R_appartment_3->text())
                                 .arg(ui->Home_phone_3->text())
                                 .arg(ui->Official_phone_3->text())
                                 .arg(ui->Document_for_l_3->text())
                                 .arg(ui->disability_3->text())
                                 .arg(ui->Job_3->text())
                                 .arg(ui->Profession_3->text())
                                 .arg(ui->Post_3->text())
                                 .arg(ui->Izh_3->text())
                                 .arg(ui->IDP_5->text()));
    }
    if (f)
        QMessageBox::information(0,"","Запись успешно добавлена в базу данных", QMessageBox::Ok);
    this->close();


}

Create_patient_DB::~Create_patient_DB()
{
    delete ui;
}
