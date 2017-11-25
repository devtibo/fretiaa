#include "windows.h"

Windows::Windows(int winName, int N)
{
    switch(winName)
    {
    case Windows::hanning :
        for (int i=0; i <N;i++)
            data.append(0.5 *(1.0 - cos( (2.0*M_PI * i)/(N-1.0))));
        break;
    }
}

QVector<double> Windows::getWin()
{
    return data;
}

