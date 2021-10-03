#include "colorconvert.h"
#include "ui_colorconvert.h"
#include "screencolorpicker.h"
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <typeinfo>
#include <QColorDialog>


static bool toColor(const QStringList &strlist, uchar arr[])
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

static bool toColorF(const QStringList &strlist, uchar arr[])
{
    bool isFloat;
    float f;
    QString temp;

    for (int i = 0; i < strlist.size(); ++i)
    {
        temp = strlist.at(i);
        f = temp.toFloat(&isFloat);
        if (isFloat && f >= 0.0f && f <= 1.0f)
        {
            arr[i] = f * 255;
        }
        else
        {
            return false;
        }
    }
    return true;
}

QColorDialog *ColorConvert::s_clrDlg = nullptr;

ColorConvert::ColorConvert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ColorConvert)
    , m_scrPicker(new ScreenColorPicker(this))
    , m_pmDecoration(QPixmap(36, 36))
{
    ui->setupUi(this);
    m_table = ui->tableWidgetColor;
    m_pmDecoration.fill(Qt::transparent);
    initTableWidget();
    connect(m_scrPicker, &ScreenColorPicker::pickFinished, this, &ColorConvert::slot_PickFinished);
    connect(m_table, &QTableWidget::cellChanged, this, &ColorConvert::onTableCellChanged);
    m_table->item(0, 1)->setText("rgba(102, 204, 253, 0.76)");

    s_clrDlg = new QColorDialog(this);
    s_clrDlg->resize(522, 411); // to avoid QWindowsWindow::setGeometry() warning;
    s_clrDlg->setOption(QColorDialog::ShowAlphaChannel);
}

ColorConvert::~ColorConvert()
{
    delete ui;
}

void ColorConvert::initTableWidget()
{
    QTableWidgetItem *item;
    QFont fontCol0("Microsoft Yahei", 14);
    QStringList horizontalLabels, verticalContents;
    horizontalLabels << QStringLiteral("颜色属性") << QStringLiteral("值");

    verticalContents << QStringLiteral("RGB") << QStringLiteral("色块")
                     << QStringLiteral("十六进制") << QStringLiteral("十进制(uint)")
                     << QStringLiteral("OpenGL") << QStringLiteral("CMYK")
                     << QStringLiteral("HSV");

    m_table->setColumnCount(horizontalLabels.size());
    m_table->setRowCount(verticalContents.size());

    m_table->setColumnWidth(0, 150);    
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setObjectName("hHeader");
    // 设置所有列不可拖动
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_table->setHorizontalHeaderLabels(horizontalLabels);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setHidden(true);
    m_table->setAlternatingRowColors(true); // 奇偶行交替颜色
    m_table->setFont(QFont("Verdana", 14));
//    m_table->setEditTriggers(QAbstractItemView::AllEditTriggers);

    for (int row = 0; row < m_table->rowCount(); ++row)
    {
        for (int col = 0; col < m_table->columnCount(); ++col)
        {
            item = new QTableWidgetItem;
            if (0 == col)
            {
                item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable)); // 第一列不可编辑
                item->setText(verticalContents.at(row));
                item->setFont(fontCol0);
            }
            else if (1 == row && 1 == col) // 色块的展示采用 Qt::DecorationRole 设置一张 pixmap
            {
                item->setData(Qt::DecorationRole, m_pmDecoration);
                item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            }
            else if (row > 4 && 1 == col)
            {
                item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~Qt::ItemIsSelectable));
            }

            m_table->setItem(row, col, item);
        }
    }
}

void ColorConvert::convertFromRgb()
{    
    QString rgb = m_table->item(0, 1)->text().trimmed();
    uchar clrVal[4] = { 0, 0, 0, 255 };
    int _count;
    if (!rgb.endsWith(")"))
        return;

    for (const QString &_head : { "rgb(", "rgba(", "(" })
    {
        if (rgb.startsWith(_head, Qt::CaseInsensitive))
        {
            rgb.remove(0, _head.length());
            rgb.resize(rgb.size() - 1);
            _count = rgb.count(",");

            if ((_head == "(" && (_count == 2 || _count == 3))
                    || (_head == "rgb(" && _count == 2)
                    || (_head == "rgba(" && _count == 3))
            {
                if (toColor(rgb.split(","), clrVal))
                {
                    m_color = QColor(clrVal[0], clrVal[1], clrVal[2], clrVal[3]);
                    setColorValue({ 1, 2, 3, 4, 5, 6 });
                }
            }
            break;
        }
    }
}

void ColorConvert::convertFromHex()
{
    QString hex = m_table->item(2, 1)->text().trimmed();
    m_color = QColor(hex);
    setColorValue({ 0, 1, 3, 4, 5, 6 });
}

void ColorConvert::convertFromDec()
{
    bool ok;
    QString dec = m_table->item(3, 1)->text().trimmed();
    uint _hex = dec.toUInt(&ok, 10);
    if (ok)
    {
        m_color = QColor(_hex);
        setColorValue({ 0, 1, 2, 4, 5, 6 });
    }
}

void ColorConvert::convertFromGL()
{
    QString glClr = m_table->item(4, 1)->text().trimmed();
    if (!glClr.endsWith(")"))
        return;

    int _count;
    uchar clrF[4] = { 0, 0, 0, 255 };

    for (const QString &_head : { "glColor4f(", "glColor3f(", "(" })
    {
        if (glClr.startsWith(_head, Qt::CaseInsensitive))
        {
            glClr.remove(0, _head.length());
            glClr.resize(glClr.size() - 1);
            _count = glClr.count(",");

            if ((_head == "(" && (_count == 2 || _count == 3))
                    || (_head == "glColor3f(" && _count == 2)
                    || (_head == "glColor4f(" && _count == 3))
            {
                if (toColorF(glClr.remove("f").split(","), clrF))
                {
                    m_color = QColor(clrF[0], clrF[1], clrF[2], clrF[3]);
                    setColorValue({ 0, 1, 2, 3, 5, 6 });
                }
            }
            break;
        }
    }
}

void ColorConvert::setColorValue(const QVector<int> &usedRows)
{
    if (!m_color.isValid())
        return;

    for (int row : usedRows)
    {
        switch (row)
        {
        case 0:
            if (m_color.alphaF() < 1.0f)
            {
                m_table->item(row, 1)->setText(QString("rgba(%1, %2, %3, %4)").
                                             arg(m_color.red()).
                                             arg(m_color.green()).
                                             arg(m_color.blue()).
                                             arg(m_color.alpha()));
            }
            else
            {
                m_table->item(row, 1)->setText(QString("rgb(%1, %2, %3)").
                                             arg(m_color.red()).
                                             arg(m_color.green()).
                                             arg(m_color.blue()));
            }
            break;
        case 1:
            m_pmDecoration.fill(m_color);
            m_table->item(row, 1)->setData(Qt::DecorationRole, m_pmDecoration);
            break;
        case 2:
            m_table->item(row, 1)->setText(m_color.name(m_color.alphaF() < 1.0f ? QColor::HexArgb : QColor::HexRgb));
            break;
        case 3:
            m_table->item(row, 1)->setText(QString::number(m_color.rgba(), 10));
            break;
        case 4:
            m_table->item(row, 1)->setText(QString("glColor4f(%1f, %2f, %3f, %4f)").
                                         arg(m_color.redF(), 0, 'f', 2, 0).
                                         arg(m_color.greenF(), 0, 'f', 2, 0).
                                         arg(m_color.blueF(), 0, 'f', 2, 0).
                                         arg(m_color.alphaF(), 0, 'f', 2, 0));
            break;
        case 5:
            m_table->item(row, 1)->setText(QString("(%1, %2, %3, %4)").
                                         arg(m_color.cyan()).
                                         arg(m_color.magenta()).
                                         arg(m_color.yellow()).
                                         arg(m_color.black()));
            break;
        case 6:
            m_table->item(row, 1)->setText(QString("(%1, %2, %3)").
                                         arg(m_color.hue()).
                                         arg(m_color.saturation()).
                                         arg(m_color.value()));
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }
}

void ColorConvert::slot_PickFinished(bool isUseful, const QColor &color)
{
    if (topLevelWidget()->isHidden()) {
        topLevelWidget()->show();
//        topLevelWidget()->activateWindow();
    }

    ui->toolButtonPick->setEnabled(true);

    if (!isUseful)
        return;

    m_color = color;
    m_pmDecoration.fill(m_color);
    m_table->item(1, 1)->setData(Qt::DecorationRole, m_pmDecoration);
}

void ColorConvert::on_toolButtonPick_clicked()
{
    int delayTime;
    if (ui->checkBoxIsHideWindow->isChecked())
    {
        topLevelWidget()->hide();
        delayTime = 300;
    }
    else
    {
        ui->toolButtonPick->setEnabled(false);
        delayTime = 50;
    }
    QTimer::singleShot(delayTime, [=](){
        if (m_scrPicker->grabDesktopPixmap())
        {
//            m_scrPicker->setAttribute(Qt::WA_TranslucentBackground, true);
            m_scrPicker->show();
            m_scrPicker->raise();
        }
    });
}

void ColorConvert::onTableCellChanged(int row, int)
{
    m_table->blockSignals(true);
    switch (row)
    {
    case 0:
        convertFromRgb();
        break;
    case 1:
        setColorValue({ 0, 2, 3, 4, 5, 6 });
        break;
    case 2:
        convertFromHex();
        break;
    case 3:
        convertFromDec();
        break;
    case 4:
        convertFromGL();
        break;
    default:
        break;
    }
    m_table->blockSignals(false);
}

void ColorConvert::on_toolButtonPal_clicked()
{
    s_clrDlg->exec();
    QColor color = s_clrDlg->selectedColor();
//        QColor color = QColorDialog::getColor(Qt::white, nullptr, QString(), QColorDialog::ShowAlphaChannel);
    if (color.isValid())
    {
        m_color = color;
        setColorValue({ 0, 1, 3, 4, 5, 6 });
    }
}
