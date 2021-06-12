#include "downvscvsixwidget.h"
#include "ui_downvscvsixwidget.h"
#include <QDebug>
#include <QMessageBox>
//#include "Python.h"

DownVscVsixWidget::DownVscVsixWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownVscVsixWidget)
{
    ui->setupUi(this);
    ui->lineEditWebUrl->setText("https://marketplace.visualstudio.com/items?itemName=VisualStudioExptTeam.vscodeintellicode");
    ui->lineEditVersion->setText("1.2.14");
}

DownVscVsixWidget::~DownVscVsixWidget()
{
    delete ui;
}

void DownVscVsixWidget::on_pushButtonDownload_clicked()
{
    if (ui->lineEditWebUrl->text().isEmpty() || ui->lineEditVersion->text().isEmpty())
    {
        QMessageBox::warning(this, "warning", QStringLiteral("两个输入框内容不能为空。"));
        return;
    }
#ifdef PYTHONPATH
    Py_Initialize();
    Py_SetPythonHome(L"C:/Python37-32");
    if (!Py_IsInitialized())
        return;

    PyObject *pModule = PyImport_ImportModule("get_vsix_url");
    if (!pModule)
    {
        qDebug("ImportModule failed!");
        return;
    }

    PyObject *pFun = PyObject_GetAttrString(pModule, "stradd");

    if (!pFun || !PyCallable_Check(pFun))
    {
        qDebug("Get function failed!");
        return;
    }

    auto website = ui->lineEditWebUrl->text().toUtf8();
//    website.constData();
    auto versionNum = ui->lineEditVersion->text().toUtf8();

    //    PyObject *objArgu = Py_BuildValue(website, versionNum);
    PyObject *args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, Py_BuildValue("website", "1.9.4"));
    PyTuple_SetItem(args, 1, Py_BuildValue("version", "https://marketplace.visualstudio.com/items?itemName=ipatalas.vscode-postfix-ts"));
//    PyTuple_SetItem(args, 0, Py_BuildValue("website", website.toStdString().c_str()));
//    PyTuple_SetItem(args, 1, Py_BuildValue("version", versionNum.toStdString().c_str()));

    PyObject *pReturn = PyObject_CallObject(pFun, args);

    if (PyUnicode_Check(pReturn))
    {
        const char *s = PyUnicode_AsUTF8(pReturn);
        qDebug() << "Download filename:" << QLatin1String(s);  //打印输出
    }

    // PyObject_CallFunction(pFun, website, versionNum);
    Py_Finalize();
    //注释掉的这部分是另一种获得test模块中的hello函数的方法
    //    PyObject* pDict = PyModule_GetDict(pModule);
    //       if (!pDict) {
    //           printf("Cant find dictionary.\n");
    //           return -1;
    //       }
    //    PyObject* pFunhello = PyDict_GetItemString(pDict, "hello");
#endif // PYTHONPATH
}

void DownVscVsixWidget::on_pushButtonGenUrl_clicked()
{
    if (ui->lineEditWebUrl->text().isEmpty() || ui->lineEditVersion->text().isEmpty())
    {
        QMessageBox::warning(this, "warning", QStringLiteral("两个输入框内容不能为空。"));
        return;
    }

    QStringList spList = ui->lineEditWebUrl->text().split('=');
    QStringList allName = spList.last().split('.');
    if (spList.size() != 2 && allName.size() != 2)
    {
        qDebug("Error: website is incorrect.");
        return;
    }

    QString publisher = allName.at(0);
    QString extension = allName.at(1);
    QString version = ui->lineEditVersion->text();
    QString fileName = QString("%1.%2-%3.vsix").arg(publisher).arg(extension).arg(version);
    QString downloadUrl = QString("https://%1.gallery.vsassets.io/_apis/public/gallery/publisher/%1"
                                  "/extension/%2/%3/assetbyname"
                                  "/Microsoft.VisualStudio.Services.VSIXPackage").arg(publisher).arg(extension).arg(version);

    ui->lineEditDownloadUrl->setText(downloadUrl);
}
