#ifndef PROPERTYGENERATOR_H
#define PROPERTYGENERATOR_H

#include <QWidget>

namespace Ui {
class PropertyGenerator;
}

class PropertyManager;

class PropertyGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyGenerator(QWidget *parent = 0);
    ~PropertyGenerator();

private:
    Ui::PropertyGenerator   *ui;
    PropertyManager         *m_manager;

private slots:
    void on_pushBtnGenCode_clicked();
    void on_pushBtnGenProperty_clicked();

};

#endif // PROPERTYGENERATOR_H
