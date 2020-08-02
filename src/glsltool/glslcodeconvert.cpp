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
    QString to_find_text("un");
    QColor col;
    QTextCursor tempCursor;
    QTextCharFormat plainFormat, colorFormat;

    if (ui->textEditFile->find(to_find_text, QTextDocument::FindBackward))
    {
        tempCursor = ui->textEditFile->textCursor();
        plainFormat = tempCursor.charFormat();
        colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);
        ui->textEditFile->mergeCurrentCharFormat(colorFormat);
    }
//    else
//    {
//        col = Qt::black;
//    }
//    tempCursor = ui->textEditFile->textCursor();
//    plainFormat = tempCursor.charFormat();
//    colorFormat = plainFormat;
//    colorFormat.setForeground(col);

    //tempCursor.clearSelection();

    QString fileContent = ui->textEditFile->toPlainText();
    fileContent.replace("\n", "\\n\"\n\"");
    fileContent.prepend("\"");
    fileContent.append("\\n\"");
    ui->textEditCode->setText(fileContent);
}
