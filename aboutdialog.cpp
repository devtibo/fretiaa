#include "aboutdialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QDialogButtonBox>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *)
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;

    /* Add icon to layout */
    QPixmap pixmap2(":/icons/iconFretiaa.png");
    QLabel *m_label = new QLabel;
    m_label->setPixmap(pixmap2);
    hLayout->addWidget(m_label);

    /* Create Frame to put text on it */
    QFrame *vFrame = new QFrame;
    vFrame->setLayout(vLayout);

    /* Create and fil the frame with text*/
    QLabel *name = new QLabel("FReTiAA");  // Software Title
    QFont f = font();
    f.setBold(true);
    f.setPointSize(12);
    name->setFont(f);
    vLayout->addWidget(name);

    QLabel *name_details = new QLabel(" ++ Free Real Time Audio Analyzer ++");  //detailled soft title
    QFont f2 = font();
    f2.setPointSize(11);
    f2.setItalic(true);
    name_details->setFont(f2);
    vLayout->addWidget(name_details);

    QLabel *version = new QLabel(QString("Version %1.%2.%3 (build with QT v.%4)").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD).arg(QT_VERSION_STR));     // soft version
    vLayout->addWidget(version);

    QLabel *legacy = new QLabel("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");     // declaration about the soft
    legacy->setWordWrap(true);
    vLayout->addWidget(legacy);

    QLabel *authors = new QLabel("copyright (c) 2017, Thibaut FUX, thibaut.fux@hotmail.fr");     // author
    vLayout->addWidget(authors);

    // Add text to layout
    hLayout->addWidget(vFrame);

    /*Add Apply/Close Button */
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    vLayout->addWidget(buttonBox);

    // Configure the dialolg box
    this->setWindowTitle("About PReTiAA");
    this->setModal(true);
    this->setWindowFlags(Qt::Tool);
    this->setLayout(hLayout);


    //Conenction
    connect(buttonBox,SIGNAL(accepted()),this, SLOT(accept()));


    this->exec();
}
