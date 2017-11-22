#include "gainconfigdial.h"
#include <QGridLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "QLabel"

GainConfigDial::GainConfigDial(float g1_Value,QString g1_Unit,float g2_Value, QString g2_Unit, float gSensi_Value, QString gSensi_Unit,QWidget *parent) : QDialog(parent)

{

    // Regular expression to only accept numbers
    QRegExpValidator* rxv = new QRegExpValidator(QRegExp("[+-]?\\d*[\\.,]?\\d+"), this);

    // Amplication Label, Edit and combobox
    QLabel *lab_G1 = new QLabel("Amplification gain");
    ed_G1->selectAll(); // set focus and allox to directly change value
    ed_G1->setValidator(rxv);
    ed_G1->setText(QString::number(g1_Value)); // set current gain
    cbx_G1->addItem("dB");
    cbx_G1->addItem("dBu");
    cbx_G1->addItem("dBV");
    cbx_G1->setCurrentText(g1_Unit);

    // Car Input Gain  Label, Edit and combobox
    QLabel *lab_G2 = new QLabel("Input card gain");
    ed_G2->setValidator(rxv);
    ed_G2->setText(QString::number(g2_Value)); // set current gain
    cbx_G2->addItem("dB");
    cbx_G2->addItem("dBu");
    cbx_G2->addItem("dBV");
    cbx_G2->setCurrentText(g2_Unit);

    // Mic Sennsitivty Label, Edit and combobox
    QLabel *lab_Sensi = new QLabel("Mic sensitivity");
    ed_GSensi->setValidator(rxv);
    ed_GSensi->setText(QString::number(gSensi_Value)); // set current seb sensitivity
    cbx_GSensi->setCurrentText(gSensi_Unit);
    cbx_GSensi->addItem("V/Pa");
    cbx_GSensi->addItem("mV/Pa");

    // Create Apply/Cancel Buttons in GroupButton
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);

    // Create and Fill combobox Layouts
    QVBoxLayout *Hlayout = new QVBoxLayout;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lab_G1,0,0);
    layout->addWidget(lab_G2,1,0);
    layout->addWidget(lab_Sensi,2,0);
    layout->addWidget(ed_G1,0,1);
    layout->addWidget(ed_G2,1,1);
    layout->addWidget(ed_GSensi,2,1);
    layout->addWidget(cbx_G1,0,2);
    layout->addWidget(cbx_G2,1,2);
    layout->addWidget(cbx_GSensi,2,2);

    //Create and add button to frame
    QFrame *frame1 = new QFrame;
    frame1->setLayout(layout);
    Hlayout->addWidget(frame1);
    Hlayout->addWidget(buttonBox);
    dial->setLayout(Hlayout);

    // Configure and execute dialogBox
    dial->setWindowTitle("Gain configuration");
    dial->setModal(true);
    dial->setWindowFlags(Qt::Tool);

    // Connection
    connect(buttonBox, SIGNAL (accepted()),dial, SLOT (accept()));
    connect(buttonBox, SIGNAL (rejected()),dial, SLOT(reject()));

    dial->exec();
}

