#include "colorconvert.h"
#include "ui_colorconvert.h"

static bool toColorValue(const QStringList &strlist, int arr[])
{
    bool ok;
    int val;
    QString temp;
    for (int i = 0; i < strlist.size(); ++i)
    {
        temp = strlist.at(i);
        val = temp.toInt(&ok);
        if (ok && val < 256)
        {
            arr[i] = val;
        }
        else
        {
            return false;
        }
    }
    return true;
}

ColorConvert::ColorConvert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorConvert)
{
    ui->setupUi(this);
}

ColorConvert::~ColorConvert()
{
    delete ui;
}

void ColorConvert::on_toolButtonRgbValue_clicked()
{    
    QString rgb = ui->lineEditRgbValue->text();
    int colVal[4] = { 0, 0, 0, 255 };
    for (const QString &_head : { "rgb(", "rgba(", "(" })
    {
        if (rgb.startsWith(_head, Qt::CaseInsensitive) && rgb.endsWith(")"))
        {
            rgb.remove(0, _head.length());
            rgb.resize(rgb.size() - 1);

            if (rgb.count(",") == 2 || rgb.count(",") == 3)
            {
                if (toColorValue(rgb.split(","), colVal))
                {
                    m_color = QColor(colVal[0], colVal[1], colVal[2], colVal[3]);
                    ui->lineEditRgbHex->setText(m_color.name());
                }
            }
            break;
        }
    }
}
