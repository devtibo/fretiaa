#ifndef GAINCONFIGDIAL_H
#define GAINCONFIGDIAL_H

#include <QDialog>
#include <QComboBox>
#include <QString>
#include <QLineEdit>

class GainConfigDial: public QDialog
{
    Q_OBJECT

public:
    // Constructor
    GainConfigDial( float,QString,float, QString, float, QString,QWidget *parent=0);

    //Edits and ComboBox
    QLineEdit *ed_G1 = new QLineEdit;
    QLineEdit *ed_G2 = new QLineEdit;
    QLineEdit *ed_GSensi = new QLineEdit;
    QComboBox *cbx_G1 = new QComboBox;
    QComboBox *cbx_G2 = new QComboBox;
    QComboBox *cbx_GSensi = new QComboBox;

private:

public slots:

};

#endif // GAINCONFIGDIAL_H
