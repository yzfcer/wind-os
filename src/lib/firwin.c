/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: fir.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ����Fir�˲��������ɺ�����������firwin����������������ܲ����Ϳ��Եõ��˲�������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.11.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.11.05
** �衡��: 
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


static double kaiser(int i,int n,double beta)//�������ı��ʽ���ӣ����ñ����������ʽ
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

/*���ش������ı��ʽ
n:���ڳ���
type:ѡ�񴰺���������
beta:���ɿ�������ϵ��*/
static double window(int type,int n,int i,double beta)
{    
    int k;
    double pi,w;
    
    pi=4.0*atan(1.0);//pi=PI;
    w=1.0;

    switch(type)
    {
    case 1:
        {    w=1.0;//���δ�
            break;
        }
    case 2:
        {    k=(n-2)/10;
            if(i<=k)
            w=0.5*(1.0-cos(i*pi/(k+1)));//ͼ����
            break;
        }
    case 3:
        {    w=1.0-fabs(1.0-2*i/(n-1.0));//���Ǵ�
            break;
        }
    case 4:
        {    w=0.5*(1.0-cos(2*i*pi/(n-1)));//������
            break;
        }
    case 5:
        {    w=0.54-0.46*cos(2*i*pi/(n-1));//������
            break;
        }
    case 6:
        {    w=0.42-0.5*cos(2*i*pi/(n-1))+0.08*cos(4*i*pi/(n-1));//����������
            break;
        }
    case 7:
        {    w=kaiser(i,n,beta);//������
            break;
        }
    }
    return(w);
}




/*n:�˲����Ľ���,n������ʱ��������Ƹ����˲���
band:�˲�������1��2��3��4�ֱ��Ӧ��ͨ����ͨ����ͨ������
fln:ͨ���±߽�Ƶ��
fhn:ͨ���ϱ߽�Ƶ��
wn:������������1-7�ֱ��Ӧ7�в�ͬ�Ĵ�����
h:����Ϊn+1,����˲�����ϵ��
fst32_fsobj:����Ƶ��
beta:����˲������ÿ�ɪ�����ͱ�ʾ���ɿ�������ϵ������������������Ч*/

void firGen(int n,int band,double fln,double fhn,int wn,double *h,double beta)
{
    int i,n2,mid;
    double s,pi,wc1,wc2,delay,fs;
    pi=4.0*atan(1.0);//pi=PI;

    if((n%2)==0)/*�������n��ż��*/
    {    n2=n/2+1;/**/
        mid=1;//
    }
    else
    {    n2=n/2;//n������,�򴰿ڳ���Ϊż��
        mid=0;//
    }
    delay=n/2.0;
    wc1=2.0*pi*fln;//
    if(band>=3) /*���ж��û���������ݣ����band��������3*/
    {    wc2=2.0*pi*fhn;    }

    switch(band)
    {
    case 1:
        {    for(i=0;i<=n2;i++)
            {    s=i-delay;//
                h[i]=(sin(wc1*s/fs)/(pi*s))*window(wn,n+1,i,beta);//��ͨ,���ڳ���=����+1����Ϊn+1
                h[n-i]=h[i];
            }
            if(mid==1)    h[n/2]=wc1/pi;//nΪż��ʱ�������м�ֵϵ��
            break;
        }
    case 2:    
        {    for(i=0;i<=n2;i++)
            {
                s=i-delay;
                h[i]=(sin(pi*s)-sin(wc1*s/fs))/(pi*s);//��ͨ-//��
                h[i]=h[i]*window(wn,n+1,i,beta);
                h[n-i]=h[i];
            }
            if(mid==1)    h[n/2]=1.0-wc1/pi;//��
            break;
        }

    case 3:
        {    for(i=0;i<n2;i++)
            {    s=i-delay;
                h[i]=(sin(wc2*s/fs)-sin(wc1*s/fs))/(pi*s);//��ͨ-//��
                h[i]=h[i]*window(wn,n+1,i,beta);
                h[n-i]=h[i];
            }
            if(mid==1)    h[n/2]=(wc2-wc1)/pi;//��
            break;
        }
    case 4:
        {    for(i=0;i<=n2;i++)
            {    s=i-delay;
                h[i]=(sin(wc1*s/fs)+sin(pi*s)-sin(wc2*s/fs))/(pi*s);//����-//��
                h[i]=h[i]*window(wn,n+1,i,beta);
                h[n-i]=h[i];
            }
            if(mid==1)    h[n/2]=(wc1+pi-wc2)/pi;
            break;
        }
    }
}












