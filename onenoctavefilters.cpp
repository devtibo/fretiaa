#include "onenoctavefilters.h"
#include <math.h>
#include <QDebug>
//#define DEBUG // Allow to save coefficients in csv file to be tested with other applciation

//TODO A TESTER EN PROFONDEUR !!!

OneNOctaveFilters::OneNOctaveFilters( DataSharer *data,QWidget* )
{
#ifdef DEBUG
    QFile f( "filters.m" );
    f.open( QIODevice::WriteOnly );
    QTextStream stream(&f);
#endif
    m_data = data;

    int b = 3; // 1/b octave filter (b=1 for octave filters)

    int base = 10; // or 2
    float G;
    if (base == 10)
        G = pow(10, 3.0/10.0);
    else
        G = pow(10,2.0);

    float fc_min = 31.5;
    //float fc_max = 3150;

    float f_ref = 1000.0;
    int indx_low;
    // int indx_high;
    if ( (b%2) | (b==1))
    {
        indx_low = round(b * log(fc_min/f_ref)/log(G));
        // indx_high = round( b * log(fc_max/f_ref)/log(G));
    }
    else
    {
        indx_low = round( (2*b * log(fc_min/f_ref)/log(G)-1)/2);
        // indx_high = round( (2*b * log(fc_max/f_ref)/log(G)-1)/2);
    }


    float f_middle[30]={0};
    float f_c1[30]={0};
    float f_c2[30]={0};
    int indx = indx_low;
    int i=0;
    bool pass = true;
    while(pass)
    {
        if ( (b%2) | (b==1)) {
            f_middle[i] = pow(G, (1.0*indx/b)) * f_ref;
        }
        else{
            f_middle[i] = pow(G,(2.0*indx+1)/(2*b)) * f_ref;}

        pass =  f_middle[i] < (m_data->fs/2.0);
        //qDebug("%f : %f !! %d", f_c[i],fs/2.0, f_c[i] < (fs/2.0));
        if(pass==1)
        {
            f_c1[i] = pow(G,-1.0/(2.0*b)) * f_middle[i];
            f_c2[i] = pow(G,1.0/(2.0*b)) * f_middle[i];
            //qDebug("%f %f %f",f_c1[i], f_middle[i],f_c2[i]);
            //qDebug("%d",i);


            filters_FcHigh_stage1.append( new Biquad(bq_type_lowpass, f_c2[i] / m_data->fs, 0.707, 0));
            filters_FcLow_stage1.append ( new Biquad(bq_type_highpass, f_c1[i] / m_data->fs, 0.707, 0));

            /*qDebug("ah=[%f %f %f];bh=[1 %f %f]", filters_FcHigh_stage1.at(i)->a0,filters_FcHigh_stage1.at(i)->a1,filters_FcHigh_stage1.at(i)->a2,filters_FcHigh_stage1.at(i)->b1, filters_FcHigh_stage1.at(i)->b2);
                    qDebug("al=[%f %f %f];bl=[1 %f %f]", filters_FcLow_stage1.at(i)->a0,filters_FcLow_stage1.at(i)->a1,filters_FcLow_stage1.at(i)->a2,filters_FcLow_stage1.at(i)->b1, filters_FcLow_stage1.at(i)->b2);*/

            filters_FcHigh_stage2.append( new Biquad(bq_type_lowpass, f_c2[i] / m_data->fs, 0.707, 0));
            filters_FcLow_stage2.append ( new Biquad(bq_type_highpass, f_c1[i] / m_data->fs, 0.707, 0));

            filters_FcHigh_stage3.append( new Biquad(bq_type_lowpass, f_c2[i] / m_data->fs, 0.707, 0));
            filters_FcLow_stage3.append ( new Biquad(bq_type_highpass, f_c1[i] / m_data->fs, 0.707, 0));


#ifdef DEBUG
            stream << "a_low(" << i+1 <<",:) =[" <<filters_FcHigh_stage1.at(i)->a0 << "," << filters_FcHigh_stage1.at(i)->a1 << "," << filters_FcHigh_stage1.at(i)->a2 <<  "]; b_low(" << i+1 << ",:) = " << "[1," << filters_FcHigh_stage1.at(i)->b1 << "," << filters_FcHigh_stage1.at(i)->b2 << "];" <<endl;
            stream << "a_high(" << i+1 << ",:) =[" << filters_FcLow_stage1.at(i)->a0 << "," << filters_FcLow_stage1.at(i)->a1 << "," << filters_FcLow_stage1.at(i)->a2 <<  "]; b_high(" << i+1 << ",:) = " << "[1," << filters_FcLow_stage1.at(i)->b1 << "," << filters_FcLow_stage1.at(i)->b2 << "];" << endl;
#endif


        }
        indx++; i++;
    }
    i--;

    m_data->numOctaveFilters=i;

#ifdef DEBUG
    f.close();
#endif

}

