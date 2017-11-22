#ifndef XYRANGEDIALOG_H
#define XYRANGEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QRegExpValidator>
#include <QDialogButtonBox>
#include <QLabel>

class XYRangeDialog  : public QDialog
{
    Q_OBJECT

public:
    XYRangeDialog(QWidget *parent=0, float x1=0, float x2=0);
    // void setDefaultValues(float,float);
    // ~XYRangeDialog();

    QLineEdit *editLine1;
    QLineEdit *editLine2;
    double val1, val2;

public slots:
    void accept();
};

#endif // XYRANGEDIALOG_H
