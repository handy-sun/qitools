#include "glslcodeconvert.h"
#include "ui_glslcodeconvert.h"
#include <QFontDatabase>

using namespace GlslTool;

GlslCodeConvert::GlslCodeConvert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GlslCodeConvert)
{
    ui->setupUi(this);

    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSizeF(qMax(13.0, codeFont.pointSizeF()));
    ui->textEditFile->setFont(codeFont);
    ui->textEditCode->setFont(codeFont);

    ui->textEditCode->setReadOnly(true);
    connect(ui->textEditFile, &QTextEdit::textChanged, this, &GlslCodeConvert::onTextCodeChanged);
}

GlslCodeConvert::~GlslCodeConvert()
{
    delete ui;
}

void GlslCodeConvert::onTextCodeChanged()
{
    if (ui->textEditFile->toPlainText().isEmpty())
    {
        ui->textEditCode->clear();
        return;
    }

    QString fileContent = ui->textEditFile->toPlainText();
    fileContent.replace("\n", "\\n\"\n\"");
    fileContent.prepend("\"");
    fileContent.append("\\n\"");
    ui->textEditCode->setText(fileContent);
}
