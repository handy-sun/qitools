﻿#include "imagetoolwidget.h"
#include "ui_imagetoolwidget.h"
#include <QImageWriter>
#include <QFileDialog>

using namespace ImageConvert;

ImageToolWidget::ImageToolWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImageToolWidget)
    , m_image(QImage())
    , m_fileInfo(QFileInfo())
{
    ui->setupUi(this);
//    qDebug() << QImageReader::supportedImageFormats().join(" *.");
    for (const auto &baFmt : QImageWriter::supportedImageFormats())
        ui->comboBox->addItem(baFmt);
}

ImageToolWidget::~ImageToolWidget()
{
    delete ui;
}

void ImageToolWidget::on_pushButtonOpenImg_clicked()
{
    auto srcFileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), ""
                                                 ,"Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");

    if (!srcFileName.isEmpty())
    {
        if (m_image.load(srcFileName))
        {
            QPixmap _pm = QPixmap::fromImage(m_image);
            if (_pm.width() > ui->labelPm->width() || _pm.height() > ui->labelPm->height())
                ui->labelPm->setPixmap(_pm.scaled(ui->labelPm->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            else
                ui->labelPm->setPixmap(_pm);
            ui->spinBoxWidth->setValue(m_image.width());
            ui->spinBoxHeight->setValue(m_image.height());
        }
    }

    m_fileInfo.setFile(srcFileName);
    ui->labelSourcePath->setText(m_fileInfo.filePath());
}

void ImageToolWidget::on_pushButtonSaveDestDir_clicked()
{
    QString destDir = m_fileInfo.absolutePath() + "/" + m_fileInfo.completeBaseName();
    QString destName = QFileDialog::getSaveFileName(this, QStringLiteral("另存为"),
                                                    destDir, "*." + ui->comboBox->currentText());
    if (destName.isEmpty())
        return;

    QImage newImg = m_image.scaled(ui->spinBoxWidth->value(), ui->spinBoxHeight->value());
    newImg.save(destName, ui->comboBox->currentText().toStdString().c_str());
}
