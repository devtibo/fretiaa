#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class AboutDialog: public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = 0);
    QDialog *dial = new QDialog;

public slots:
    void exitApp();
};

#endif // ABOUTDIALOG_H
