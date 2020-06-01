#ifndef PROPERTYGENERATOR_H
#define PROPERTYGENERATOR_H

#include <QWidget>

namespace Ui {
class PropertyGenerator;
}

class PropertyGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyGenerator(QWidget *parent = 0);
    ~PropertyGenerator();

private:
    Ui::PropertyGenerator *ui;

private slots:
    void on_pushBtnCodeGen_clicked();

    void on_pushBtnPropertyGen_clicked();

};

#endif // PROPERTYGENERATOR_H
