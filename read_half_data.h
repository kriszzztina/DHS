#ifndef READ_HALF_DATA_H
#define READ_HALF_DATA_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class read_half_data;
}

class read_half_data : public QDialog
{
    Q_OBJECT

public:
    explicit read_half_data(QWidget *parent = 0);
    ~read_half_data();

private:
    Ui::read_half_data *ui;
    void Parse(QListWidget* list, QString file_n, QMap<QString,QString> &maps, char index);

private slots:
    void OpenMedicalCard();
};

#endif // READ_HALF_DATA_H
