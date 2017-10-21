#ifndef READ_FULL_DATA_H
#define READ_FULL_DATA_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class Read_full_data;
}

class Read_full_data : public QDialog
{
    Q_OBJECT

public:
    explicit Read_full_data(QWidget *parent = 0);
    ~Read_full_data();

private:
    Ui::Read_full_data *ui;
    //void Parse_for_list(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index);


private slots:
    void Add_to_DB();
    void Open_change();
    void Update();

};

#endif // READ_FULL_DATA_H
