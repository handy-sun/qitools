﻿#include "glslcodeconvert.h"
#include "ui_glslcodeconvert.h"

using namespace GlslTool;

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
