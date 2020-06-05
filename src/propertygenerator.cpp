#include "propertygenerator.h"
#include "ui_propertygenerator.h"
#include "propertymanager.h"
#include <QMessageBox>
#include <QDebug>

PropertyGenerator::PropertyGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropertyGenerator),
    m_manager(new PropertyManager)
{
    ui->setupUi(this);
    ui->lineEditPrefix->setText("m_");
    ui->lineEditType->setText("int");
    ui->lineEditVariation->setText("number");

    ui->comboBoxArg->addItems({"T", "const T &"});

    on_pushBtnGenProperty_clicked();
}

PropertyGenerator::~PropertyGenerator()
{
    delete ui;
    delete m_manager;
}

void PropertyGenerator::on_pushBtnGenCode_clicked()
{
    QString property = ui->textEditGenProperty->toPlainText();
    if (!property.contains("READ"))
    {
        QMessageBox::critical(this, "error", QStringLiteral("必须含有 READ 属性！"));
        return;
    }

    ui->textBrowserGenCode->clear();
    m_manager->setPrefix(ui->lineEditPrefix->text());
    m_manager->setArgumentType(ui->comboBoxArg->currentIndex());
    ui->textBrowserGenCode->setText(m_manager->generateCode(property, ui->checkBoxInline->isChecked()));
}

void PropertyGenerator::on_pushBtnGenProperty_clicked()
{
    QString type = ui->lineEditType->text().trimmed();
    QString varName = ui->lineEditVariation->text().trimmed();
    QString upperVarName(varName);

    upperVarName.replace(0, 1, upperVarName.constData()->toUpper());

    QString qProperty = QString("Q_PROPERTY(%1 %2 READ %2 WRITE set%3 NOTIFY %3Changed)")
            .arg(type).arg(varName).arg(upperVarName);

    ui->textEditGenProperty->setText(qProperty);
}
