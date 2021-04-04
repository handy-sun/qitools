#include "glslcodeconvert.h"
#include "ui_glslcodeconvert.h"

GlslCodeConvert::GlslCodeConvert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GlslCodeConvert)
{
    ui->setupUi(this);
    ui->textEditCode->setReadOnly(true);
    connect(ui->textEditFile, &QTextEdit::textChanged, this, &GlslCodeConvert::onTextCodeChanged);
}

GlslCodeConvert::~GlslCodeConvert()
{
    delete ui;
}

void GlslCodeConvert::onTextCodeChanged()
{
    QString fileContent = ui->textEditFile->toPlainText();
    fileContent.replace("\n", "\\n\"\n\"");
    fileContent.prepend("\"");
    fileContent.append("\\n\"");
    ui->textEditCode->setText(fileContent);
}
