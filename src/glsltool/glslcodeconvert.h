#ifndef GLSLCODECONVERT_H
#define GLSLCODECONVERT_H

#include <QWidget>

namespace Ui {
class GlslCodeConvert;
}

class GlslCodeConvert : public QWidget
{
    Q_OBJECT

public:
    explicit GlslCodeConvert(QWidget *parent = 0);
    ~GlslCodeConvert();

private slots:
    void onTextCodeChanged();

private:
    Ui::GlslCodeConvert *ui;
};

#endif // GLSLCODECONVERT_H
