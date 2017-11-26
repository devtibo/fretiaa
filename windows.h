#ifndef WINDOWS_H
#define WINDOWS_H

#include <QString>
#include <QVector>

class Windows
{    
public:
    enum { hanning = 1 , other = 0 };
    Windows(int,int);
    QVector<double> data;
    QVector<double> getWin();
private :

};

#endif // WINDOWS_H
