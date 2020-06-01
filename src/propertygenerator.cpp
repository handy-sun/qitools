#include "propertygenerator.h"
#include "ui_propertygenerator.h"
#include <QDebug>

PropertyGenerator::PropertyGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertyGenerator)
{
    ui->setupUi(this);
    ui->lineEditPrefix->setText("m_");
    ui->lineEditType->setText("int");
    ui->lineEditVariation->setText("number");

    on_pushBtnPropertyGen_clicked();
}

PropertyGenerator::~PropertyGenerator()
{
    delete ui;
}

void PropertyGenerator::on_pushBtnCodeGen_clicked()
{
    QString qPropertyContent = ui->textEditPropertyGen->toPlainText();
    qDebug() << qPropertyContent;
}

void PropertyGenerator::on_pushBtnPropertyGen_clicked()
{
    QString type = ui->lineEditType->text().trimmed();
    QString varName = ui->lineEditVariation->text().trimmed();
    QString upperVarName(varName);

    upperVarName.replace(0, 1, upperVarName.constData()->toUpper());

    QString qProperty = QString("Q_PROPERTY(%1 %2 READ %2 WRITE set%3 NOTIFY %3Changed)")
            .arg(type).arg(varName).arg(upperVarName);

    ui->textEditPropertyGen->setText(qProperty);
}
