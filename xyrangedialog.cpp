#include <QFile>
#include <QTextStream>

#include "xyrangedialog.h"

XYRangeDialog::XYRangeDialog(QWidget *parent, float x1, float x2)
    : QDialog(parent)
{
    // Window configuration
    setWindowTitle("Change Range Axis");

    // Regular Expression to only accept number
    QRegExpValidator* rxv = new QRegExpValidator(QRegExp("[+-]?\\d*[\\.,]?\\d+"), this);

    // Create Bottom Value : Label and Edit
    QLabel *value1 = new QLabel (QString("Bottom value: "));
    editLine1 = new QLineEdit(this);
    editLine1->setText(QString::number(x1));
    editLine1->selectAll();
    editLine1->setValidator(rxv);

    // Create Bottom Value : Label and Edit
    QLabel *value2 = new QLabel (QString("Top Value: "));
    editLine2 = new QLineEdit(this);
    editLine2->setText(QString::number(x2));
    editLine2->setValidator(rxv);

    // Create Buttons Box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    // Create And fill Layout
    QGridLayout *m_layout = new QGridLayout;
    m_layout->addWidget(value1,0,0);
    m_layout->addWidget(editLine1,0,1);
    m_layout->addWidget(value2,1,0);
    m_layout->addWidget(editLine2,1,1);
    m_layout->addWidget(buttonBox,2,0,1,2);

    // Connections
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(m_layout);
}


void XYRangeDialog::accept() // Re-definition of QDialog::accept()
{
    val1 = editLine1->text().toDouble();
    val2 = editLine2->text().toDouble();
    done(Accepted);
}


