#ifndef CREATE_PATIENT_DB_H
#define CREATE_PATIENT_DB_H

#include <QDialog>

namespace Ui {
class Create_patient_DB;
}

class Create_patient_DB : public QDialog
{
    Q_OBJECT

public:
    explicit Create_patient_DB(QWidget *parent = 0, QString str_ID = "", QString surname = "", QString name = "", QString MN = "", bool upd = false);
    ~Create_patient_DB();

private:
    Ui::Create_patient_DB *ui;
    bool upd;
    QString surname;
    QString name;
    QString mn;

private slots:
    void Add_to_DB();
};

#endif // CREATE_PATIENT_DB_H
