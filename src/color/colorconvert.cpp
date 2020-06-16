#include "colorconvert.h"
#include "ui_colorconvert.h"

static bool toColorValue(const QStringList &strlist, uchar arr[])
{
    bool isInt, isFloat;
    int val;
    QString temp, alpha;

    for (int i = 0; i < 3; ++i)
    {
        temp = strlist.at(i);
        val = temp.toInt(&isInt);
        if (isInt && val >= 0 && val < 256)
        {
            arr[i] = val;
        }
        else
        {
            return false;
        }
    }

    if (strlist.size() == 3) // rgb
        return true;

    // rgba
    alpha = strlist.at(3);
    val = alpha.toInt(&isInt);
    if (isInt && val >= 0 && val < 256)
    {
        arr[3] = val;
    }
    else
    {
        float f = alpha.toFloat(&isFloat);
        if (isFloat && f >= 0.0f && f <= 1.0f)
        {
            arr[3] = f * 255;
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
    uchar colVal[4] = { 0, 0, 0, 255 };
    int cnt;
    for (const QString &_head : { "rgb(", "rgba(", "(" })
    {
        if (rgb.startsWith(_head, Qt::CaseInsensitive) && rgb.endsWith(")"))
        {
            rgb.remove(0, _head.length());
            rgb.resize(rgb.size() - 1);
            cnt = rgb.count(",");

            if (cnt == 2 || cnt == 3)
            {
                if (toColorValue(rgb.split(","), colVal))
                {
                    m_color = QColor(colVal[0], colVal[1], colVal[2], colVal[3]);
                    ui->lineEditRgbHex->setText(m_color.name(cnt == 2 ? QColor::HexRgb : QColor::HexArgb));
                }
            }
            break;
        }
    }
}
