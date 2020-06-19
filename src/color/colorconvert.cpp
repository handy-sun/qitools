#include "colorconvert.h"
#include "ui_colorconvert.h"
#include <QLabel>
#include <QDebug>

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

ColorConvert::ColorConvert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ColorConvert)
    , m_pmDecoration(QPixmap(36, 36))
{
    ui->setupUi(this);
    m_table = ui->tableWidget;
    m_pmDecoration.fill(Qt::transparent);
    initTableWidget();
    //ui->tableWidget->item(0, 0)->setText("rgb");
    connect(m_table, &QTableWidget::itemPressed, [=](QTableWidgetItem *item){qtout << item->text();});
}

ColorConvert::~ColorConvert()
{
    delete ui;
}

void ColorConvert::on_toolButtonRgbValue_clicked()
{    
    QString rgb = m_table->item(0, 1)->text();
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
                    m_pmDecoration.fill(m_color);
                    m_table->item(1, 1)->setData(Qt::DecorationRole, m_pmDecoration);
                    m_table->item(2, 1)->setText(m_color.name(cnt == 2 ? QColor::HexRgb : QColor::HexArgb));
                    m_table->item(3, 1)->setText(QString::number(m_color.rgba(), 10));
                    m_table->item(4, 1)->setText(QString("glColor4f(%1f, %2f, %3f, %4f)").
                                               arg(m_color.redF(), 0, 'f', 2, 0).
                                               arg(m_color.greenF(), 0, 'f', 2, 0).
                                               arg(m_color.blueF(), 0, 'f', 2, 0).
                                               arg(m_color.alphaF(), 0, 'f', 2, 0));
                    m_table->item(5, 1)->setText(QString("(%1, %2, %3, %4)").
                                               arg(m_color.cyan()).
                                               arg(m_color.magenta()).
                                               arg(m_color.yellow()).
                                               arg(m_color.black()));
                    m_table->item(6, 1)->setText(QString("(%1, %2, %3)").
                                               arg(m_color.hue()).
                                               arg(m_color.saturation()).
                                               arg(m_color.value()));
                }
            }
            break;
        }
    }
}

void ColorConvert::initTableWidget()
{
    QTableWidgetItem *item;
    QFont fontCol0("Microsoft Yahei", 14);
    QFont fontCol1("Consolas", 16);   
    QStringList horizontalLabels, verticalContents;
    horizontalLabels << QStringLiteral("表达方式") << QStringLiteral("值");

    verticalContents << QStringLiteral("RGB") << QStringLiteral("色块")
                     << QStringLiteral("十六进制") << QStringLiteral("十进制")
                     << QStringLiteral("OpenGL") << QStringLiteral("CMYK")
                     << QStringLiteral("HSV");
    QLabel *colorBlock = new QLabel(this);
    colorBlock->setFixedSize(45, 45);


    m_table->setColumnCount(2);
    m_table->setRowCount(7);

    m_table->setColumnWidth(0, 150);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setObjectName("hHeader");
    m_table->setHorizontalHeaderLabels(horizontalLabels);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setHidden(true);
    m_table->setAlternatingRowColors(true); // 奇偶行交替颜色
    m_table->setFont(fontCol1);
//    m_table->setEditTriggers(QAbstractItemView::AllEditTriggers);

    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        for (int col = 0; col < m_table->columnCount(); ++col)
        {
            item = new QTableWidgetItem;
            if (0 == col)
            {
                item->setFlags(item->flags() & (~Qt::ItemIsEditable)); // 第一列不可编辑
                item->setText(verticalContents.at(row));
                item->setFont(fontCol0);
            }
//            else // 表格整体设置字体后，第一列字体默认为表格字体
//            {
//                item->setFont(fontCol1);
//            }
            if (1 == row && 1 == col) // 色块的展示采用 Qt::DecorationRole 设置一张 pixmap
            {
                item->setData(Qt::DecorationRole, m_pmDecoration);
                item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            }

            m_table->setItem(row, col, item);
        }
    }

    m_table->item(0, 1)->setText("rgba(102, 204, 253, 0.76)");
}
