#ifndef ONENOCTAVEFILTERS_H
#define ONENOCTAVEFILTERS_H

#include <QList>
#include "biquad.h"
#include "datasharer.h"

class OneNOctaveFilters
{
public:
    OneNOctaveFilters(DataSharer*);
    QList<Biquad*> filters_FcLow_stage1;
    QList<Biquad*> filters_FcLow_stage2;
    QList<Biquad*> filters_FcLow_stage3;
    QList<Biquad*> filters_FcHigh_stage1;
    QList<Biquad*> filters_FcHigh_stage2;
    QList<Biquad*> filters_FcHigh_stage3;
    DataSharer *m_data;
};

#endif // ONENOCTAVEFILTERS_H
