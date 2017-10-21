#ifndef WRITE_DATA_H
#define WRITE_DATA_H

#include <QDialog>
#include <QComboBox>

namespace Ui {
class write_data;
}

class write_data : public QDialog
{
    Q_OBJECT

public:
    explicit write_data(QWidget *parent = 0, QString critical_data_str = "", QString surname_str = "", QString name_str = "", QString MN_str = "");
    ~write_data();

private:
    Ui::write_data *ui;
    void fill_comboBox(QComboBox* combo, QString filename, QMap<QString,QString> &map);
    QString critical_data_str, surname_str, name_str, MN_str;

private slots:
    void Show_HZ_code(QString str);
    void Activate_HZ_group();
    void Add_to_List();
    void Save_data();
};

#endif // WRITE_DATA_H
