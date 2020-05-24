/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : fir.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : 关于Fir滤波器的生成函数集，对于firwin函数，输入相关性能参数就可以得到滤波器参数
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.11.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.11.05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include"math.h"


static double bessel0(double x)
{
    int i;
    double d,y,d2,sum;
    y=x/2.0;
    d=1.0;
    sum=1.0;
    for(i=1;i<=25;i++)
    {    
        d=d*y/i;
        d2=d*d;
        sum=sum+d2;
        if(d2<sum*(1.0e-8)) break;
    }
    return(sum);
}

//Because the expression of caser window is complex, the expression of Bessel window is called
static double kaiser(int i,int n,double beta)
{
    double a,w,a2,b1,b2,beta1;
    b1=bessel0(beta);
    a=2.0*i/(double)(n-1)-1.0;
    a2=a*a;
    beta1=beta*sqrt(1.0-a2);
    b2=bessel0(beta1);
    w=b2/b1;
    return(w);
}

/*Returns the expression of a window function
n:Window length
type:Select the type of window function
beta:Coefficient to generate Kaiser window*/
static double window(int type,int n,int i,double beta)
{    
    int k;
    double pi,w;

    pi=4.0*atan(1.0);//pi=PI;
    w=1.0;

    switch(type)
    {
    case 1:
        w=1.0;//Rectangular window
        break;
    case 2:
        k=(n-2)/10;
        if(i<=k)
        w=0.5*(1.0-cos(i*pi/(k+1)));//Tukey window
        break;
    case 3:
        w=1.0-fabs(1.0-2*i/(n-1.0));//Triangular window
        break;
    case 4:
        w=0.5*(1.0-cos(2*i*pi/(n-1)));//Hanning window
        break;
    case 5:
        w=0.54-0.46*cos(2*i*pi/(n-1));//hamming window
        break;
    case 6:
        w=0.42-0.5*cos(2*i*pi/(n-1))+0.08*cos(4*i*pi/(n-1));//blackman window
        break;
    case 7:
        w=kaiser(i,n,beta);//Kaiser window
        break;
    default:
        w = 0.0;
        break;
    }
    return(w);
}




/*n:The order of the filter, n is an odd number can be used to design a variety of filters
band:Filter types 1, 2, 3 and 4 correspond to low pass, high pass, band pass and band stop respectively
fln:Lower boundary frequency of passband
fhn:Upper boundary frequency of passband
wn:Types 1-7 of window functions correspond to different window functions in 7
h:The length is n + 1, which stores the coefficients of the filter
fs:sampling frequency
beta:If the filter adopts Kaiser window, it means the coefficient of generating Kaiser window, which is invalid for other window functions*/

void firGen(int n,int band,double fln,double fhn,int wn,double *h,double beta)
{
    int i,n2,mid;
    double s,pi,wc1,wc2,delay,fs;
    pi=4.0*atan(1.0);//pi=PI;

    if((n%2)==0)
    {    
        n2=n/2+1;
        mid=1;//
    }
    else
    {    
        n2=n/2;
        mid=0;
    }
    delay=n/2.0;
    wc1=2.0*pi*fln;//
    if(band>=3)
        wc2=2.0*pi*fhn;    

    switch(band)
    {
    case 1:
        for(i=0;i<=n2;i++)
        {    
            s=i-delay;//
            h[i]=(sin(wc1*s/fs)/(pi*s))*window(wn,n+1,i,beta);//low pass
            h[n-i]=h[i];
        }
        if(mid==1)    
            h[n/2]=wc1/pi;
        break;
    case 2:    
        for(i=0;i<=n2;i++)
        {
            s=i-delay;
            h[i]=(sin(pi*s)-sin(wc1*s/fs))/(pi*s);//high pass
            h[i]=h[i]*window(wn,n+1,i,beta);
            h[n-i]=h[i];
        }
        if(mid==1)    
            h[n/2]=1.0-wc1/pi;//对
        break;

    case 3:
        for(i=0;i<n2;i++)
        {    
            s=i-delay;
            h[i]=(sin(wc2*s/fs)-sin(wc1*s/fs))/(pi*s);//band pass
            h[i]=h[i]*window(wn,n+1,i,beta);
            h[n-i]=h[i];
        }
        if(mid==1)    
            h[n/2]=(wc2-wc1)/pi;//对
        break;
    case 4:
        for(i=0;i<=n2;i++)
        {    
            s=i-delay;
            h[i]=(sin(wc1*s/fs)+sin(pi*s)-sin(wc2*s/fs))/(pi*s);//band stop
            h[i]=h[i]*window(wn,n+1,i,beta);
            h[n-i]=h[i];
        }
        if(mid==1)    
            h[n/2]=(wc1+pi-wc2)/pi;
        break;
        }
}












